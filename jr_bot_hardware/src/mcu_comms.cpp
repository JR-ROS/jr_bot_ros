#include "jr_bot_hardware/mcu_comms.hpp"
#include "jr_bot_hardware/jr_bot_messages.hpp" 

#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>

MCUComms::~MCUComms() {
    if (connected()) {
        disconnect();
    }
}

bool MCUComms::connect(const std::string& serial_device, int32_t timeout_ms) {
    timeout_ms_ = timeout_ms;

    try {
        serial_port.Open(serial_device);
        serial_port.SetBaudRate(LibSerial::BaudRate::BAUD_2000000);
        serial_port.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
        serial_port.SetParity(LibSerial::Parity::PARITY_NONE);
        serial_port.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
        
        // Release hardware lines so the ESP32 exits the bootloader 
        serial_port.SetDTR(false);
        serial_port.SetRTS(false);
        
    } catch (const LibSerial::OpenFailed& e) {
        std::cerr << "Failed to open serial port: " << e.what() << std::endl;
        return false;
    }

    return true;
}

void MCUComms::disconnect() { serial_port.Close(); }

bool MCUComms::connected() { return serial_port.IsOpen(); }

std::string MCUComms::sanitize_msg(const std::string& msg) {
    std::string sanitized_msg = msg;
    sanitized_msg.erase(std::remove(sanitized_msg.begin(), sanitized_msg.end(), '\r'), sanitized_msg.end());
    sanitized_msg.erase(std::remove(sanitized_msg.begin(), sanitized_msg.end(), '\n'), sanitized_msg.end());
    return sanitized_msg;
}

template <typename T>
int MCUComms::req_data(uint8_t request_id, std::vector<T>& pcbData) {
    serial_port.FlushIOBuffers();

    // --- 1. Pack and Transmit Request ---
    uint8_t txBuffer[256];
    uint32_t txLength;
    DataFrame_TypeDef requestFrame = {
        .frameID = request_id, .timestamp = 0, .dataLength = 0, .data = NULL};

    if (botSpeak_packFrame(&requestFrame, txBuffer, &txLength) != 0) {
        return 1;
    }

    try {
        std::vector<uint8_t> tx_data(txBuffer, txBuffer + txLength);
        serial_port.Write(tx_data);
        serial_port.DrainWriteBuffer();

        // --- 2. Robust Receive State Machine ---
        std::vector<uint8_t> rx_buffer;
        uint8_t byte = 0;
        
        // Step A: Sync to Start Byte
        // We read byte-by-byte, throwing away garbage until we hit START_BYTE
        while (true) {
            serial_port.ReadByte(byte, timeout_ms_);
            if (byte == START_BYTE) {
                rx_buffer.push_back(byte);
                break;
            }
        }

        // Step B: Read Header
        // Now that we are synced, grab the 12-byte header in one highly efficient block
        LibSerial::DataBuffer header_chunk;
        serial_port.Read(header_chunk, 12, timeout_ms_);
        rx_buffer.insert(rx_buffer.end(), header_chunk.begin(), header_chunk.end());

        // Step C: Extract Payload Length dynamically
        uint32_t payload_len = 0;
        std::memcpy(&payload_len, &rx_buffer[5], sizeof(uint32_t));

        // Sanity check to prevent out-of-memory crashes from corrupted packets
        if (payload_len > MCU_USB_COMM_PAYLOAD_SIZE) {
            std::cerr << "Corrupted payload length: " << payload_len << std::endl;
            return 1;
        }

        // Step D: Read Payload + End Byte
        LibSerial::DataBuffer body_chunk;
        serial_port.Read(body_chunk, payload_len + 1, timeout_ms_);
        rx_buffer.insert(rx_buffer.end(), body_chunk.begin(), body_chunk.end());

        // --- 3. Unpack and Deserialize ---
        static uint8_t unpacked_frame_buffer[MCU_USB_COMM_PAYLOAD_SIZE];
        DataFrame_TypeDef responseFrame = {.data = unpacked_frame_buffer};
        
        if (botSpeak_unpackFrame(&responseFrame, rx_buffer.data(), rx_buffer.size()) != 0) {
            std::cerr << "Failed to unpack frame ID: " << (int)request_id << std::endl;
            return 1;
        }

        uint32_t numberElements = 0;
        
        // Allocate buffer based on dynamic length, not a hardcoded switch statement!
        T* readData = new T[payload_len / sizeof(T) + 1]; 

        botSpeak_deserialize(readData, &numberElements, sizeof(T),
                             responseFrame.data, responseFrame.dataLength);

        pcbData.clear();
        for (uint32_t i = 0; i < numberElements; ++i) {
            pcbData.push_back(readData[i]);
        }

        delete[] readData;
        return 0;

    } catch (const LibSerial::ReadTimeout&) {
        // Soft-fail on timeout: Skip this control loop cycle instead of crashing the stream
        std::cerr << "Read timeout for ID: " << (int)request_id << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Hardware exception: " << e.what() << std::endl;
        return 1;
    }
}

template <typename T>
int MCUComms::send_data(uint8_t command_id, const std::vector<T>& data) {
    T* trasmitBuffer = new T[data.size()];
    std::copy(data.begin(), data.end(), trasmitBuffer);

    uint32_t dataLength;
    uint32_t frameLength;
    uint8_t serializedDataBuffer[256];
    uint8_t frameBuffer[256];

    botSpeak_serialize(trasmitBuffer, data.size(), sizeof(T),
                       serializedDataBuffer, &dataLength);

    DataFrame_TypeDef frame = {.frameID = command_id,
                               .timestamp = 0,
                               .dataLength = dataLength,
                               .data = serializedDataBuffer};

    if (botSpeak_packFrame(&frame, frameBuffer, &frameLength) != 0) {
        std::cerr << "Failed to pack frame" << std::endl;
        delete[] trasmitBuffer;
        return 1;
    }

    try {
        std::vector<uint8_t> tx_data(frameBuffer, frameBuffer + frameLength);
        serial_port.Write(tx_data);
        serial_port.DrainWriteBuffer();
    } catch (...) {
        delete[] trasmitBuffer;
        return 1;
    }

    delete[] trasmitBuffer;
    return 0;
}


// --- Explicit Template Instantiations ---
template int MCUComms::req_data<float>(uint8_t request_id, std::vector<float>& pcbData);
template int MCUComms::req_data<uint16_t>(uint8_t request_id, std::vector<uint16_t>& pcbData); 
template int MCUComms::req_data<bool>(uint8_t request_id, std::vector<bool>& pcbData);     

template int MCUComms::send_data<int16_t>(uint8_t command_id, const std::vector<int16_t>& data); 
template int MCUComms::send_data<bool>(uint8_t command_id, const std::vector<bool>& data);