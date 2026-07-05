#include "jr_bot_hardware/motor.hpp"
#include <cmath>
#include <algorithm>

void Motor::setup(const std::string& name, double max_rpm) {
    name_ = name;
    max_rpm_ = max_rpm;
    reset_state();
}

void Motor::integrate_open_loop(double dt_seconds) {
    // 1. Loopback the command to pretend the motor instantly reached the target velocity
    state_vel_ = cmd_vel_;
    
    // 2. Integrate the velocity to simulate a physical encoder ticking up
    state_pos_ += (state_vel_ * dt_seconds);
}

int16_t Motor::get_pwm_command() {
    // Convert rad/s to RPM
    double target_rpm = cmd_vel_ * (60.0 / (2.0 * M_PI));
    
    // Normalize to a percentage of max speed (-1.0 to 1.0)
    double speed_ratio = target_rpm / max_rpm_;
    
    // Clamp the ratio to prevent sending > 255 PWM
    speed_ratio = std::clamp(speed_ratio, -1.0, 1.0);
    
    // Scale to the 8-bit PWM range expected by the ESP32
    return static_cast<int16_t>(speed_ratio * 255.0);
}

void Motor::reset_state() {
    state_pos_ = 0.0;
    state_vel_ = 0.0;
    cmd_vel_ = 0.0;
}