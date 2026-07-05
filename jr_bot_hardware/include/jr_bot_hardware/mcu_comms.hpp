#ifndef JR_BOT_HARDWARE__MCU_COMMS_HPP_
#define JR_BOT_HARDWARE__MCU_COMMS_HPP_

// Change the include at the top:
#include <libserial/SerialPort.h>
#include <vector>
#include <string>
#include "bot_speak.h"

#define MCU_USB_COMM_PAYLOAD_SIZE 32
#define MCU_USB_COMM_MAX_PACKET_SIZE (BOT_SPEAK_MIN_PACKET_SIZE + MCU_USB_COMM_PAYLOAD_SIZE)

class MCUComms {
   private:
    // Change this to SerialPort
    LibSerial::SerialPort serial_port;
    int timeout_ms_;

   public:
    MCUComms() = default;
    ~MCUComms();

    bool connect(const std::string& serial_device, int32_t timeout_ms);
    void disconnect();
    bool connected();

    std::string sanitize_msg(const std::string& msg);

    template <typename T>
    int req_data(uint8_t request_id, std::vector<T>& pcbData);

    template <typename T>
    int send_data(uint8_t command_id, const std::vector<T>& data);
};

#endif  // JR_BOT_HARDWARE__MCU_COMMS_HPP_