#ifndef TOF_LASER_SCAN_BROADCASTER_HPP_
#define TOF_LASER_SCAN_BROADCASTER_HPP_

#include <memory>
#include <vector>

#include "controller_interface/controller_interface.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

namespace jr_bot_controllers {

class TofLaserScanBroadcaster : public controller_interface::ControllerInterface {
public:
    controller_interface::InterfaceConfiguration command_interface_configuration() const override;
    controller_interface::InterfaceConfiguration state_interface_configuration() const override;

    controller_interface::CallbackReturn on_init() override;
    controller_interface::CallbackReturn on_configure(const rclcpp_lifecycle::State& previous_state) override;
    controller_interface::CallbackReturn on_activate(const rclcpp_lifecycle::State& previous_state) override;
    controller_interface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State& previous_state) override;

    controller_interface::return_type update(const rclcpp::Time& time, const rclcpp::Duration& period) override;

private:
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr scan_pub_;
    sensor_msgs::msg::LaserScan scan_msg_;
    
    rclcpp::Time sweep_start_time_;
    
    // To ignore junk data during startup
    bool first_reading_ = true;
};

}  // namespace jr_bot_controllers

#endif  // TOF_LASER_SCAN_BROADCASTER_HPP_