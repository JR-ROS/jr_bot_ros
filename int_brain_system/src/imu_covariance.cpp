#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include <vector>
#include <numeric>
#include <cmath>

class ImuCovarianceNode : public rclcpp::Node
{
public:
    ImuCovarianceNode() : Node("imu_covariance_node")
    {
        subscription_ = this->create_subscription<sensor_msgs::msg::Imu>(
            "/imu_sensor_broadcaster/imu", 10, std::bind(&ImuCovarianceNode::imu_callback, this, std::placeholders::_1));
    }

private:
    void imu_callback(const sensor_msgs::msg::Imu::SharedPtr msg)
    {
        // Collect angular velocity and linear acceleration data
        angular_velocity_x_.push_back(msg->angular_velocity.x);
        angular_velocity_y_.push_back(msg->angular_velocity.y);
        angular_velocity_z_.push_back(msg->angular_velocity.z);
        linear_acceleration_x_.push_back(msg->linear_acceleration.x);
        linear_acceleration_y_.push_back(msg->linear_acceleration.y);
        linear_acceleration_z_.push_back(msg->linear_acceleration.z);

        if (angular_velocity_x_.size() > 100)
        {
            calculate_and_print_covariance();
            // Optional: clear data to re-calculate for the next batch
            // clear_data();
        }
    }

    void calculate_and_print_covariance()
    {
        RCLCPP_INFO(this->get_logger(), "Calculating covariances...");

        // Angular velocity covariance
        double cov_wx = calculate_variance(angular_velocity_x_);
        double cov_wy = calculate_variance(angular_velocity_y_);
        double cov_wz = calculate_variance(angular_velocity_z_);

        // Linear acceleration covariance
        double cov_ax = calculate_variance(linear_acceleration_x_);
        double cov_ay = calculate_variance(linear_acceleration_y_);
        double cov_az = calculate_variance(linear_acceleration_z_);

        RCLCPP_INFO(this->get_logger(), "Angular Velocity Covariance: [%f, %f, %f]", cov_wx, cov_wy, cov_wz);
        RCLCPP_INFO(this->get_logger(), "Linear Acceleration Covariance: [%f, %f, %f]", cov_ax, cov_ay, cov_az);
    }

    double calculate_variance(const std::vector<double>& data)
    {
        if (data.size() < 2)
        {
            return 0.0;
        }

        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        double mean = sum / data.size();
        double sq_sum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);
        double variance = (sq_sum / data.size()) - (mean * mean);

        return variance;
    }

    void clear_data()
    {
        angular_velocity_x_.clear();
        angular_velocity_y_.clear();
        angular_velocity_z_.clear();
        linear_acceleration_x_.clear();
        linear_acceleration_y_.clear();
        linear_acceleration_z_.clear();
    }

    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr subscription_;
    std::vector<double> angular_velocity_x_, angular_velocity_y_, angular_velocity_z_;
    std::vector<double> linear_acceleration_x_, linear_acceleration_y_, linear_acceleration_z_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ImuCovarianceNode>());
    rclcpp::shutdown();
    return 0;
}
