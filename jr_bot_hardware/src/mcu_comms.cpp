#include "jr_bot_hardware/mcu_comms.hpp"
#include "jr_bot_hardware/jr_bot_messages.hpp" // The distilled header we created earlier

#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>

MCUComms::~MCUComms() {
    if (connected()) {
        disconnect();
    }
}

bool MCUComms::connect(const std::string& serial_device, int32_t timeout_ms) {
    timeout_ms_ = timeout_ms;

    try {
        serial_port.Open(serial_device);
        // Standardize serial port settings for ESP32 UART
        serial_port.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
        serial_port.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
        serial_port.SetParity(LibSerial::Parity::PARITY_NONE);
        serial_port.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
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

    uint8_t dataBuffer[256];
    uint8_t responseBuffer[256];
    static uint8_t unpacked_frame_buffer[MCU_USB_COMM_PAYLOAD_SIZE];

    uint32_t dataLength;
    uint32_t numberElements;
    std::size_t bytesToRead;

    DataFrame_TypeDef requestFrame = {
        .frameID = request_id, .timestamp = 0, .dataLength = 0, .data = NULL};

    DataFrame_TypeDef responseFrame = {.data = unpacked_frame_buffer};

    // Route the request to expect the correctly sized response from jr_bot_messages.h
    switch (request_id) {
        case REQUEST_IMU_ACCEL_RAW:
            bytesToRead = BOT_SPEAK_MIN_PACKET_SIZE + SERIALIZED_IMU_ACCEL_RAW_BYTES;
            numberElements = UNSERIALIZED_IMU_ACCEL_RAW_SIZE;
            break;

        case REQUEST_IMU_GYRO_RAW:
            bytesToRead = BOT_SPEAK_MIN_PACKET_SIZE + SERIALIZED_IMU_GYRO_RAW_BYTES;
            numberElements = UNSERIALIZED_IMU_GYRO_RAW_SIZE;
            break;

        case REQUEST_TOF_STATE:
            bytesToRead = BOT_SPEAK_MIN_PACKET_SIZE + SERIALIZED_TOF_STATE_BYTES;
            numberElements = UNSERIALIZED_TOF_STATE_SIZE;
            break;

        case REQUEST_IR_STATES:
            bytesToRead = BOT_SPEAK_MIN_PACKET_SIZE + SERIALIZED_IR_STATES_BYTES;
            numberElements = UNSERIALIZED_IR_STATES_SIZE;
            break;

        default:
            std::cerr << "Unknown request ID: " << request_id << std::endl;
            return 1;
    }

    T* readData = new T[numberElements];

    try {
        botSpeak_packFrame(&requestFrame, dataBuffer, &dataLength);
        serial_port.write((const char*)dataBuffer, dataLength);

        // wait for a response
        serial_port.read((char*)responseBuffer, bytesToRead);

        // once we get a response, parse it
        int result = botSpeak_unpackFrame(&responseFrame, responseBuffer, bytesToRead);

        if (result != 0) {
            std::cerr << "Failed to unpack frame: " << result << std::endl;
            delete[] readData;
            return 1;
        }

        // Deserialize standard C++ types
        botSpeak_deserialize(readData, &numberElements, sizeof(T),
                             responseFrame.data, responseFrame.dataLength);

        pcbData.clear();
        for (uint32_t i = 0; i < numberElements; ++i) {
            pcbData.push_back(static_cast<T>(readData[i]));
        }
    }
    catch (const LibSerial::ReadTimeout& ex) {
        std::cerr << "Read timeout: " << ex.what() << std::endl;
        delete[] readData;
        return 1;
    }

    delete[] readData;
    return 0;
}

template <typename T>
int MCUComms::send_data(uint8_t command_id, const std::vector<T>& data) {
    serial_port.FlushIOBuffers();  

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

    serial_port.write((const char*)frameBuffer, frameLength);

    delete[] trasmitBuffer;
    return 0;
}


// --- Explicit Template Instantiations ---
// This tells the compiler to pre-build these specific versions of the templates

// For req_data (Receiving from ESP32)
template int MCUComms::req_data<float>(uint8_t request_id, std::vector<float>& pcbData);
template int MCUComms::req_data<uint16_t>(uint8_t request_id, std::vector<uint16_t>& pcbData); // For ToF
template int MCUComms::req_data<bool>(uint8_t request_id, std::vector<bool>& pcbData);     // For IRs

// For send_data (Sending to ESP32)
template int MCUComms::send_data<int16_t>(uint8_t command_id, const std::vector<int16_t>& data); // For Motor PWM
template int MCUComms::send_data<bool>(uint8_t command_id, const std::vector<bool>& data);       // For User LED