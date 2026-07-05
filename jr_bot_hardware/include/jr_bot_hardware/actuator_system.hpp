#ifndef JR_BOT_HARDWARE__ACTUATOR_SYSTEM_HPP_
#define JR_BOT_HARDWARE__ACTUATOR_SYSTEM_HPP_

#include <memory>
#include <string>
#include <vector>

#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "jr_bot_hardware/mcu_comms.hpp"
#include "jr_bot_hardware/imu.hpp"
#include "jr_bot_hardware/motor.hpp"
#include "jr_bot_messages.hpp"
#include "rclcpp/clock.hpp"
#include "rclcpp/duration.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"

namespace jr_bot_hardware {
class JrBotHardware : public hardware_interface::SystemInterface {
    struct Config {
        std::string device_addr = "";
        int timeout_ms = 0;
    };

   public:
    RCLCPP_SHARED_PTR_DEFINITIONS(JrBotHardware)

    hardware_interface::CallbackReturn on_init(
        const hardware_interface::HardwareInfo& info) override;

    std::vector<hardware_interface::StateInterface> export_state_interfaces()
        override;

    std::vector<hardware_interface::CommandInterface>
    export_command_interfaces() override;

    hardware_interface::CallbackReturn on_configure(
        const rclcpp_lifecycle::State& previous_state) override;

    hardware_interface::CallbackReturn on_cleanup(
        const rclcpp_lifecycle::State& previous_state) override;

    hardware_interface::CallbackReturn on_activate(
        const rclcpp_lifecycle::State& previous_state) override;

    hardware_interface::CallbackReturn on_deactivate(
        const rclcpp_lifecycle::State& previous_state) override;

    hardware_interface::return_type read(
        const rclcpp::Time& time, const rclcpp::Duration& period) override;

    hardware_interface::return_type write(
        const rclcpp::Time& time, const rclcpp::Duration& period) override;

   private:
    MCUComms comms_;
    Config cfg_;
    Motor motors[2]; // Reduced from 4 to 2 for Differential Drive
    Imu imu_;

    // ros2_control requires all interfaces to be doubles. 
    // Booleans will be represented as 1.0 (true) and 0.0 (false).
    
    double tof_distance_ = 0.0;
    double tof_angle_ = 0.0;
    
    double ir_left_ = 0.0;
    double ir_right_ = 0.0;
    
    double user_led_cmd_ = 0.0;
};

}  // namespace jr_bot_hardware

#endif  // JR_BOT_HARDWARE__ACTUATOR_SYSTEM_HPP_