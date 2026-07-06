#include "jr_bot_hardware/tof_laser_scan_broadcaster.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/rclcpp.hpp"
#include "pluginlib/class_list_macros.hpp"

namespace jr_bot_controllers {

controller_interface::InterfaceConfiguration
TofLaserScanBroadcaster::command_interface_configuration() const {
    return controller_interface::InterfaceConfiguration{
        controller_interface::interface_configuration_type::NONE};
}

controller_interface::InterfaceConfiguration
TofLaserScanBroadcaster::state_interface_configuration() const {
    controller_interface::InterfaceConfiguration config;
    config.type = controller_interface::interface_configuration_type::INDIVIDUAL;
    // Format is always: <joint_name>/<interface_name>
    config.names = {"tof_sweep_distance/position", "tof_sweep_angle/position"};
    return config;
}

controller_interface::CallbackReturn TofLaserScanBroadcaster::on_init() {
    try {
        scan_pub_ = get_node()->create_publisher<sensor_msgs::msg::LaserScan>("scan", 10);
    } catch (const std::exception& e) {
        RCLCPP_ERROR(get_node()->get_logger(), "Exception during init: %s", e.what());
        return controller_interface::CallbackReturn::ERROR;
    }
    return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn TofLaserScanBroadcaster::on_configure(
    const rclcpp_lifecycle::State& /*previous_state*/) {
    
    // Configure standard static LiDAR parameters
    scan_msg_.header.frame_id = "tof_link"; // Make sure this matches your URDF
    scan_msg_.angle_min = 0.0;
    scan_msg_.angle_max = M_PI; // 180 degrees in rad
    scan_msg_.angle_increment = M_PI / 180.0; // 1 degree resolution
    
    // Adjust these based on your specific ToF sensor specs (e.g., VL53L0X)
    scan_msg_.range_min = 0.03; // 3 cm
    scan_msg_.range_max = 2.0;  // 2 meters
    
    // 181 buckets for 0-180 degrees inclusive. Initialize with Infinity.
    scan_msg_.ranges.assign(181, std::numeric_limits<float>::infinity());
    
    return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn TofLaserScanBroadcaster::on_activate(
    const rclcpp_lifecycle::State& /*previous_state*/) {
    
    sweep_start_time_ = get_node()->now();
    first_reading_ = true;
    scan_msg_.ranges.assign(181, std::numeric_limits<float>::infinity());
    return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn TofLaserScanBroadcaster::on_deactivate(
    const rclcpp_lifecycle::State& /*previous_state*/) {
    return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::return_type TofLaserScanBroadcaster::update(
    const rclcpp::Time& time, const rclcpp::Duration& /*period*/) {
    
    if (state_interfaces_.size() != 2) {
        return controller_interface::return_type::ERROR;
    }

    double dist_mm = state_interfaces_[0].get_value();
    double angle_deg = state_interfaces_[1].get_value();

    if (first_reading_) {
        sweep_start_time_ = time;
        first_reading_ = false;
    }

    // 1. Bucket the current reading
    // Constrain the angle between 0 and 180 to avoid array out-of-bounds
    int bucket = std::clamp(static_cast<int>(std::round(angle_deg)), 0, 180);
    
    // Convert mm to meters for ROS standards
    float dist_m = static_cast<float>(dist_mm / 1000.0);
    
    // Filter out zero-readings (often indicates sensor error)
    if (dist_m > scan_msg_.range_min && dist_m < scan_msg_.range_max) {
        scan_msg_.ranges[bucket] = dist_m;
    }

    // 2. Check for Sweep Completion
    double dt = (time - sweep_start_time_).seconds();
    bool sweep_finished = false;

    // Trigger if we reach the physical edges (with a 1 second debounce so it doesn't multi-fire at the edge)
    if ((angle_deg <= 2.0 || angle_deg >= 178.0) && dt > 1.0) {
        sweep_finished = true;
    } 
    // Fallback trigger: If the servo gets stuck or skips, force a publish after 2.5s
    else if (dt > 2.5) {
        sweep_finished = true;
    }

    // 3. Publish and Reset
    if (sweep_finished) {
        scan_msg_.header.stamp = time;
        scan_pub_->publish(scan_msg_);

        // Wipe the slate clean for the next sweep
        scan_msg_.ranges.assign(181, std::numeric_limits<float>::infinity());
        sweep_start_time_ = time;
    }

    return controller_interface::return_type::OK;
}

}  // namespace jr_bot_controllers

PLUGINLIB_EXPORT_CLASS(jr_bot_controllers::TofLaserScanBroadcaster, controller_interface::ControllerInterface)