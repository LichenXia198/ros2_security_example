#include "rclcpp/rclcpp.hpp"
#include "rcl_interfaces/msg/log.hpp"
#include "std_msgs/msg/string.hpp"

class Listener : public rclcpp::Node
{
public:
    Listener(const std::string& name, const std::string& topic, bool received, const rclcpp::NodeOptions& options = rclcpp::NodeOptions())
    : Node(name, options), first_message_received_(received)
    {
        subscription_ = this->create_subscription<std_msgs::msg::String>(
        topic, 10, std::bind(&Listener::topic_callback, this, std::placeholders::_1));
    }

private:
    void topic_callback(const std_msgs::msg::String::SharedPtr msg)
    {
        if (!first_message_received_)
        {
            first_message_received_ = true;
            
            RCLCPP_INFO(this->get_logger(), "Create");
            setenv("ROS_SECURITY_ENCLAVE_OVERRIDE", "/py_sros2_test_topic/talker2", 1);

            auto context = std::make_shared<rclcpp::Context>();
            context->init(0, nullptr);
            rclcpp::NodeOptions options;
            options.context(context);
            auto node = std::make_shared<rclcpp::Node>("talker2", "", options);

            // Create a publishe
            auto publisher = node->create_publisher<std_msgs::msg::String>("chatter", 10);

            // Create a message to publish
            std_msgs::msg::String message;
            message.data = "Hello, ROS2!";

            // Publish the message
            publisher->publish(message);

            rclcpp::spin(node);
            rclcpp::shutdown();

        }

        RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
    }


    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    bool first_message_received_;
    // std::shared_ptr<Listener> second_listener_;
    //std::thread second_listener_thread_;
};

int main(int argc, char * argv[])
{
    
    rclcpp::init(argc, argv);
    
    auto listener = std::make_shared<Listener>("first_listener", "topic", false);

    rclcpp::spin(listener);

    rclcpp::shutdown();
    return 0;
}

