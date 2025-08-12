#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include <vector>
#include <numeric>
#include <cmath>

class ImuCalibrateNode : public rclcpp::Node
{
public:
    ImuCalibrateNode() : Node("imu_calibrate_node")
    {
        subscription_ = this->create_subscription<sensor_msgs::msg::Imu>(
            "/imu_sensor_broadcaster/imu", 10, std::bind(&ImuCalibrateNode::imu_callback, this, std::placeholders::_1));
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

        if (angular_velocity_x_.size() > 500)
        {
            calculate_and_print_covariance();
            clear_data();
        }
    }

    void calculate_and_print_covariance()
    {
        RCLCPP_INFO(this->get_logger(), "Calculating covariances...");

        // Angular velocity covariance matrix
        double cov_w[9];
        cov_w[0] = calculate_variance(angular_velocity_x_);                        // cov(x,x)
        cov_w[1] = calculate_covariance(angular_velocity_x_, angular_velocity_y_); // cov(x,y)
        cov_w[2] = calculate_covariance(angular_velocity_x_, angular_velocity_z_); // cov(x,z)
        cov_w[3] = calculate_covariance(angular_velocity_y_, angular_velocity_x_); // cov(y,x)
        cov_w[4] = calculate_variance(angular_velocity_y_);                        // cov(y,y)
        cov_w[5] = calculate_covariance(angular_velocity_y_, angular_velocity_z_); // cov(y,z)
        cov_w[6] = calculate_covariance(angular_velocity_z_, angular_velocity_x_); // cov(z,x)
        cov_w[7] = calculate_covariance(angular_velocity_z_, angular_velocity_y_); // cov(z,y)
        cov_w[8] = calculate_variance(angular_velocity_z_);                        // cov(z,z)

        // Linear acceleration covariance matrix
        double cov_a[9];
        cov_a[0] = calculate_variance(linear_acceleration_x_);                           // cov(x,x)
        cov_a[1] = calculate_covariance(linear_acceleration_x_, linear_acceleration_y_); // cov(x,y)
        cov_a[2] = calculate_covariance(linear_acceleration_x_, linear_acceleration_z_); // cov(x,z)
        cov_a[3] = calculate_covariance(linear_acceleration_y_, linear_acceleration_x_); // cov(y,x)
        cov_a[4] = calculate_variance(linear_acceleration_y_);                           // cov(y,y)
        cov_a[5] = calculate_covariance(linear_acceleration_y_, linear_acceleration_z_); // cov(y,z)
        cov_a[6] = calculate_covariance(linear_acceleration_z_, linear_acceleration_x_); // cov(z,x)
        cov_a[7] = calculate_covariance(linear_acceleration_z_, linear_acceleration_y_); // cov(z,y)
        cov_a[8] = calculate_variance(linear_acceleration_z_);                           // cov(z,z)

        RCLCPP_INFO(this->get_logger(), "Angular Velocity Covariance: [%f, %f, %f, %f, %f, %f, %f, %f, %f]",
                    cov_w[0], cov_w[1], cov_w[2], cov_w[3], cov_w[4], cov_w[5], cov_w[6], cov_w[7], cov_w[8]);
        RCLCPP_INFO(this->get_logger(), "Linear Acceleration Covariance: [%f, %f, %f, %f, %f, %f, %f, %f, %f]",
                    cov_a[0], cov_a[1], cov_a[2], cov_a[3], cov_a[4], cov_a[5], cov_a[6], cov_a[7], cov_a[8]);
    }

    double calculate_covariance(const std::vector<double> &data1, const std::vector<double> &data2)
    {
        if (data1.size() < 2 || data1.size() != data2.size())
        {
            return 0.0;
        }

        double sum1 = std::accumulate(data1.begin(), data1.end(), 0.0);
        double mean1 = sum1 / data1.size();

        double sum2 = std::accumulate(data2.begin(), data2.end(), 0.0);
        double mean2 = sum2 / data2.size();

        double inner_prod = std::inner_product(data1.begin(), data1.end(), data2.begin(), 0.0);
        double covariance = (inner_prod / data1.size()) - (mean1 * mean2);

        return covariance;
    }

    double calculate_variance(const std::vector<double> &data)
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

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ImuCalibrateNode>());
    rclcpp::shutdown();
    return 0;
}
