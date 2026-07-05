#ifndef JR_BOT_HARDWARE_MOTOR_HPP
#define JR_BOT_HARDWARE_MOTOR_HPP

#include <string>

class Motor {
   public:
    std::string name_;
    
    // ROS 2 Hardware Interfaces
    double state_pos_;      // Fake encoder position (rad)
    double state_vel_;      // Fake encoder velocity (rad/s)
    double cmd_vel_;        // Target velocity from diff_drive_controller (rad/s)
    
    // Hardware constraints passed from URDF
    double max_rpm_;        // Used to map rad/s to PWM

    Motor() = default;

    void setup(const std::string& name, double max_rpm);
    
    /**
     * @brief Integrates the commanded velocity over dt to simulate encoder feedback.
     */
    void integrate_open_loop(double dt_seconds);

    /**
     * @brief Maps the commanded velocity (rad/s) to the ESP32 PWM scale (-255 to 255).
     */
    int16_t get_pwm_command();

    void reset_state();
};

#endif  // JR_BOT_HARDWARE_MOTOR_HPP