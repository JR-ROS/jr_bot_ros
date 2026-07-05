#include "jr_bot_hardware/actuator_system.hpp"

#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>

#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "pluginlib/class_list_macros.hpp"
#include "jr_bot_hardware/imu.hpp"
#include "rclcpp/rclcpp.hpp"

namespace jr_bot_hardware {

hardware_interface::CallbackReturn JrBotHardware::on_init(
    const hardware_interface::HardwareInfo& info) {
    
    if (hardware_interface::SystemInterface::on_init(info) !=
        hardware_interface::CallbackReturn::SUCCESS) {
        return hardware_interface::CallbackReturn::ERROR;
    }

    info_ = info;

    cfg_.device_addr = info_.hardware_parameters["device_addr"];
    cfg_.timeout_ms = std::stoi(info_.hardware_parameters["timeout_ms"]);

    // Initialize the 2 differential drive motors
    int index = 0;
    for (const hardware_interface::ComponentInfo& joint : info_.joints) {
        if (joint.name == "left_wheel_joint" || joint.name == "right_wheel_joint") {
            const auto command_interface = joint.command_interfaces[0];

            if (command_interface.name != hardware_interface::HW_IF_VELOCITY) {
                RCLCPP_FATAL(
                    rclcpp::get_logger("JrBotHardware"),
                    "Joint '%s' has '%s' command interface. '%s' expected.",
                    joint.name.c_str(), command_interface.name.c_str(),
                    hardware_interface::HW_IF_VELOCITY);
                return hardware_interface::CallbackReturn::ERROR;
            }

            // Fetch the motor's max RPM from the URDF to calculate PWM scaling
            double max_rpm = std::stod(joint.parameters.at("max_rpm"));
            
            motors[index++].setup(joint.name, max_rpm);
            
            RCLCPP_INFO(rclcpp::get_logger("JrBotHardware"),
                        "Motor '%s' initialized with max_rpm: %f",
                        joint.name.c_str(), max_rpm);
        }
    }

    return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface>
JrBotHardware::export_state_interfaces() {
    std::vector<hardware_interface::StateInterface> state_interfaces;

    // Export fake encoder states for the motors
    for (size_t i = 0; i < info_.joints.size(); ++i) {
        state_interfaces.emplace_back(hardware_interface::StateInterface(
            info_.joints[i].name, hardware_interface::HW_IF_POSITION,
            &motors[i].state_pos_));

        state_interfaces.emplace_back(hardware_interface::StateInterface(
            info_.joints[i].name, hardware_interface::HW_IF_VELOCITY,
            &motors[i].state_vel_));
    }

    // Export 6-DOF IMU states
    for (auto& sensor : info_.sensors) {
        if (sensor.name == "imu_sensor") {
            state_interfaces.emplace_back(hardware_interface::StateInterface(
                sensor.name, "angular_velocity.x", &imu_.angular_velocity_[0]));
            state_interfaces.emplace_back(hardware_interface::StateInterface(
                sensor.name, "angular_velocity.y", &imu_.angular_velocity_[1]));
            state_interfaces.emplace_back(hardware_interface::StateInterface(
                sensor.name, "angular_velocity.z", &imu_.angular_velocity_[2]));

            state_interfaces.emplace_back(hardware_interface::StateInterface(
                sensor.name, "linear_acceleration.x", &imu_.linear_acceleration_[0]));
            state_interfaces.emplace_back(hardware_interface::StateInterface(
                sensor.name, "linear_acceleration.y", &imu_.linear_acceleration_[1]));
            state_interfaces.emplace_back(hardware_interface::StateInterface(
                sensor.name, "linear_acceleration.z", &imu_.linear_acceleration_[2]));
        }
    }

    // Export Custom ToF Interfaces
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "tof_sweep", "distance", &tof_distance_));
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "tof_sweep", "angle", &tof_angle_));

    // Export IR Sensor Interfaces
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "ir_sensor_left", "state", &ir_left_));
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "ir_sensor_right", "state", &ir_right_));

    // Add to export_state_interfaces() for the IMU
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "imu_sensor", "orientation.w", &imu_.orientation_[0])); // imu_.orientation_[0] should be 1.0
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "imu_sensor", "orientation.x", &imu_.orientation_[1]));
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "imu_sensor", "orientation.y", &imu_.orientation_[2]));
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "imu_sensor", "orientation.z", &imu_.orientation_[3]));

    return state_interfaces;
}

std::vector<hardware_interface::CommandInterface>
JrBotHardware::export_command_interfaces() {
    std::vector<hardware_interface::CommandInterface> command_interfaces;

    for (size_t i = 0; i < info_.joints.size(); ++i) {
        command_interfaces.emplace_back(hardware_interface::CommandInterface(
            info_.joints[i].name, hardware_interface::HW_IF_VELOCITY,
            &motors[i].cmd_vel_));
    }

    // Export User LED Command Interface
    command_interfaces.emplace_back(hardware_interface::CommandInterface(
        "user_led", "state", &user_led_cmd_));

    return command_interfaces;
}

hardware_interface::CallbackReturn JrBotHardware::on_configure(
    const rclcpp_lifecycle::State& /*previous_state*/) {
    
    RCLCPP_INFO(rclcpp::get_logger("JrBotHardware"), "Configuring ...please wait...");
    
    if (comms_.connected()) {
        comms_.disconnect();
    }
    if (!comms_.connect(cfg_.device_addr, cfg_.timeout_ms)) {
        RCLCPP_ERROR(rclcpp::get_logger("JrBotHardware"),
                     "Failed to connect to JR BOT at %s",
                     cfg_.device_addr.c_str());
        return hardware_interface::CallbackReturn::ERROR;
    }

    // RCLCPP_INFO(rclcpp::get_logger("JrBotHardware"), "Waiting for ESP32 FreeRTOS to boot...");
    // std::this_thread::sleep_for(std::chrono::seconds(2));

    RCLCPP_INFO(rclcpp::get_logger("JrBotHardware"), "Successfully configured!");
    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn JrBotHardware::on_cleanup(
    const rclcpp_lifecycle::State& /*previous_state*/) {
    
    RCLCPP_INFO(rclcpp::get_logger("JrBotHardware"), "Cleaning up...");

    motors[0].reset_state();
    motors[1].reset_state();

    if (comms_.connected()) {
        comms_.disconnect();
    }

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn JrBotHardware::on_activate(
    const rclcpp_lifecycle::State& /*previous_state*/) {
    
    RCLCPP_INFO(rclcpp::get_logger("JrBotHardware"), "Activating...");
    if (!comms_.connected()) {
        return hardware_interface::CallbackReturn::ERROR;
    }
    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn JrBotHardware::on_deactivate(
    const rclcpp_lifecycle::State& /*previous_state*/) {
    
    RCLCPP_INFO(rclcpp::get_logger("JrBotHardware"), "Deactivating...");
    
    // Safety: Send 0 PWM to motors before shutting down
    std::vector<int16_t> stop_cmds = {0, 0};
    comms_.send_data(MOTOR_DESIRED_PWM, stop_cmds);

    if (comms_.connected()) {
        comms_.disconnect();
    }

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::return_type JrBotHardware::read(
    const rclcpp::Time& /*time*/, const rclcpp::Duration& period) {
    
    if (!comms_.connected()) {
        return hardware_interface::return_type::ERROR;
    }

    // 1. Simulate Encoder Feedback (Open-Loop Odometry)
    double dt = period.seconds();
    for (auto& motor : motors) {
        motor.integrate_open_loop(dt);
    }

    // 2. Fetch Real IMU Data
    std::vector<float> imu_accel;
    if (comms_.req_data(REQUEST_IMU_ACCEL_RAW, imu_accel) == 0 && imu_accel.size() == 3) {
        imu_.linear_acceleration_[0] = imu_accel[0];
        imu_.linear_acceleration_[1] = imu_accel[1];
        imu_.linear_acceleration_[2] = imu_accel[2];
    } else {
        RCLCPP_WARN(rclcpp::get_logger("JrBotHardware"), "Failed to read IMU ACCEL");
    }

    std::vector<float> imu_gyro;
    if (comms_.req_data(REQUEST_IMU_GYRO_RAW, imu_gyro) == 0 && imu_gyro.size() == 3) {
        imu_.angular_velocity_[0] = imu_gyro[0];
        imu_.angular_velocity_[1] = imu_gyro[1];
        imu_.angular_velocity_[2] = imu_gyro[2];
    } else {
        RCLCPP_WARN(rclcpp::get_logger("JrBotHardware"), "Failed to read IMU GYRO");
    }

    // 3. Fetch ToF Sweep Data
    std::vector<uint16_t> tof_data;
    if (comms_.req_data(REQUEST_TOF_STATE, tof_data) == 0 && tof_data.size() == 2) {
        tof_distance_ = static_cast<double>(tof_data[0]);
        tof_angle_    = static_cast<double>(tof_data[1]);
    } else {
        RCLCPP_WARN(rclcpp::get_logger("JrBotHardware"), "Failed to read ToF State");
    }

    // 4. Fetch IR Sensor Data
    std::vector<bool> ir_data;
    if (comms_.req_data(REQUEST_IR_STATES, ir_data) == 0 && ir_data.size() == 2) {
        ir_left_  = ir_data[0] ? 1.0 : 0.0;
        ir_right_ = ir_data[1] ? 1.0 : 0.0;
    } else {
        RCLCPP_WARN(rclcpp::get_logger("JrBotHardware"), "Failed to read IR States");
    }

    return hardware_interface::return_type::OK;
}

hardware_interface::return_type JrBotHardware::write(
    const rclcpp::Time& /*time*/, const rclcpp::Duration& /*period*/) {
    
    if (!comms_.connected()) {
        return hardware_interface::return_type::ERROR;
    }

    // Convert commanded velocities (rad/s) directly into 8-bit PWM values
    std::vector<int16_t> pwm_commands;
    for (auto& motor : motors) {
        pwm_commands.push_back(motor.get_pwm_command());
    }

    // Transmit to ESP32
    if (comms_.send_data(MOTOR_DESIRED_PWM, pwm_commands) != 0) {
        RCLCPP_ERROR(rclcpp::get_logger("JrBotHardware"), "Failed to write motor PWMs");
        return hardware_interface::return_type::ERROR;
    }

    // Send User LED Command (Threshold at 0.5 to convert double to bool)
    std::vector<bool> led_cmd;
    led_cmd.push_back(user_led_cmd_ > 0.5);
    
    if (comms_.send_data(USER_DEFINED_LED, led_cmd) != 0) {
        RCLCPP_ERROR(rclcpp::get_logger("JrBotHardware"), "Failed to write User LED state");
        // We won't return ERROR here so we don't crash the motor loop if the LED drops a packet
    }

    return hardware_interface::return_type::OK;
}

}  // namespace jr_bot_hardware

PLUGINLIB_EXPORT_CLASS(
  jr_bot_hardware::JrBotHardware, hardware_interface::SystemInterface)