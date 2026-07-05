#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include <algorithm>
#include <vector>

class TofScanPublisher : public rclcpp::Node {
public:
    TofScanPublisher() : Node("tof_scan_publisher") {
        scan_pub_ = this->create_publisher<sensor_msgs::msg::LaserScan>("/scan", 10);
        
        subscription_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "/joint_states", 10, std::bind(&TofScanPublisher::joint_state_callback, this, std::placeholders::_1));
    }

private:
    void joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr msg) {
        double dist = 0.0;
        int angle = 0;
        bool found_dist = false, found_angle = false;

        for (size_t i = 0; i < msg->name.size(); ++i) {
            if (msg->name[i] == "tof_sweep_distance") {
                dist = msg->position[i];
                found_dist = true;
            } else if (msg->name[i] == "tof_sweep_angle") {
                angle = static_cast<int>(msg->position[i]);
                found_angle = true;
            }
        }

        if (found_dist && found_angle) {
            publish_scan(angle, dist);
        }
    }

    void publish_scan(int current_angle, double current_dist) {
        auto scan = sensor_msgs::msg::LaserScan();
        scan.header.stamp = this->now();
        scan.header.frame_id = "tof_link";
        scan.angle_min = 0.0;
        scan.angle_max = 3.14159; 
        scan.angle_increment = 0.01745; 
        scan.range_min = 0.03;
        scan.range_max = 1.2;
        
        // 1. Fill the ENTIRE array with infinity
        scan.ranges.assign(181, std::numeric_limits<float>::infinity());
        
        // 2. ONLY set the current angle's distance
        if (current_angle >= 0 && current_angle <= 180) {
            scan.ranges[current_angle] = static_cast<float>(current_dist) / 1000.0f;
        }
        
        scan_pub_->publish(scan);
    }

    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr scan_pub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr subscription_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TofScanPublisher>());
    rclcpp::shutdown();
    return 0;
}