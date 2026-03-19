/**
 *  Copyright (C) 2025 - Seyond Inc.
 *
 *  All Rights Reserved.
 *
 *  $Id$
 */

#include "driver_lidar.h"
#include "seyond/msg/seyond_packet.hpp"
#include "seyond/msg/seyond_scan.hpp"
#include "utils/inno_lidar_log.h"

#include <rclcpp/rclcpp.hpp>

#include <sensor_msgs/msg/point_cloud2.hpp>

#include <pcl_conversions/pcl_conversions.h>

#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace seyond_node
{
class SeyondNode : public rclcpp::Node
{
public:
  explicit SeyondNode(const rclcpp::NodeOptions & options) : Node("seyond_node", options)
  {
    bool publish_pointcloud = declare_parameter<bool>("publish_pointcloud", true);
    log_level_ = declare_parameter<std::string>("log_level", "info");
    lidar_config_.replay_rosbag = declare_parameter<bool>("replay_rosbag", false);
    lidar_config_.packet_mode = declare_parameter<bool>("packet_mode", true);
    lidar_config_.frame_id = declare_parameter<std::string>("frame_id", "seyond");
    lidar_config_.lidar_name = declare_parameter<std::string>("lidar_name", "seyond");

    lidar_config_.lidar_ip = declare_parameter<std::string>("lidar_ip", "172.168.1.10");
    lidar_config_.port = static_cast<int32_t>(declare_parameter<int32_t>("port", 8010));
    lidar_config_.udp_port = static_cast<int32_t>(declare_parameter<int32_t>("udp_port", 8010));

    lidar_config_.reflectance_mode = declare_parameter<bool>("reflectance_mode", true);
    lidar_config_.multiple_return =
      static_cast<int32_t>(declare_parameter<int32_t>("multiple_return", 1));

    lidar_config_.continue_live = declare_parameter<bool>("continue_live", false);
    lidar_config_.pcap_file = declare_parameter<std::string>("pcap_file", "");
    lidar_config_.hv_table_file = declare_parameter<std::string>("hv_table_file", "");
    lidar_config_.packet_rate =
      static_cast<int32_t>(declare_parameter<int32_t>("packet_rate", 10000));

    lidar_config_.file_rewind = static_cast<int32_t>(declare_parameter<int32_t>("file_rewind", 0));
    lidar_config_.max_range = declare_parameter<double>("max_range", 2000.0);  // unit: meter
    lidar_config_.min_range = declare_parameter<double>("min_range", 0.4);     // unit: meter
    lidar_config_.name_value_pairs = declare_parameter<std::string>("name_value_pairs", "");
    lidar_config_.coordinate_mode =
      static_cast<int32_t>(declare_parameter<int32_t>("coordinate_mode", 3));
    lidar_config_.transform_enable = declare_parameter<bool>("transform_enable", false);
    lidar_config_.x = declare_parameter<double>("x", 0.0);
    lidar_config_.y = declare_parameter<double>("y", 0.0);
    lidar_config_.z = declare_parameter<double>("z", 0.0);
    lidar_config_.pitch = declare_parameter<double>("pitch", 0.0);
    lidar_config_.yaw = declare_parameter<double>("yaw", 0.0);
    lidar_config_.roll = declare_parameter<double>("roll", 0.0);
    lidar_config_.transform_matrix = declare_parameter<std::string>("transform_matrix", "");

    seyond::DriverLidar::init_log_s(
      log_level_, std::bind(
                    &SeyondNode::rosLogCallback, this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3));
    driver_ptr_ = std::make_unique<seyond::DriverLidar>(lidar_config_);
    inno_scan_msg_ = std::make_unique<seyond::msg::SeyondScan>();

    inno_frame_pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>(
      "seyond_points", rclcpp::SensorDataQoS());
    driver_ptr_->register_publish_frame_callback(
      std::bind(&SeyondNode::publishFrame, this, std::placeholders::_1, std::placeholders::_2));

    if (lidar_config_.packet_mode) {
      inno_pkt_pub_ = this->create_publisher<seyond::msg::SeyondScan>("seyond_packets", 100);
      inno_pkt_sub_ = this->create_subscription<seyond::msg::SeyondScan>(
        "seyond_packets", 100,
        std::bind(&SeyondNode::subscribePacket, this, std::placeholders::_1));
      driver_ptr_->register_publish_packet_callback(std::bind(
        &SeyondNode::publishPacket, this, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4));
    }
    if (!publish_pointcloud) {
      inno_frame_pub_.reset();
      inno_pkt_sub_.reset();
    }

    driver_ptr_->start_lidar();
  }

  ~SeyondNode()
  {
    driver_ptr_->stop_lidar();
    driver_ptr_.reset();
  }

  SeyondNode(const SeyondNode &) = delete;
  SeyondNode & operator=(const SeyondNode &) = delete;
  SeyondNode(SeyondNode &&) = delete;
  SeyondNode & operator=(SeyondNode &&) = delete;

  void subscribePacket(const seyond::msg::SeyondScan::SharedPtr msg)
  {
    for (const auto & pkt : msg->packets) {
      if (pkt.type == seyond::msg::SeyondPacket::PACKET_TYPE_HVTABLE) {
        if (lidar_config_.replay_rosbag && !driver_ptr_->anglehv_table_init_) {
          driver_ptr_->anglehv_table_.resize(pkt.data.size());
          std::memcpy(driver_ptr_->anglehv_table_.data(), pkt.data.data(), pkt.data.size());
          driver_ptr_->anglehv_table_init_ = true;
        }
      } else {
        driver_ptr_->convert_and_parse(reinterpret_cast<const int8_t *>(pkt.data.data()));
      }
    }

    sensor_msgs::msg::PointCloud2 ros_msg;
    driver_ptr_->transform_pointcloud();
    pcl::toROSMsg(*driver_ptr_->pcl_pc_ptr, ros_msg);
    ros_msg.header = msg->header;
    ros_msg.width = driver_ptr_->pcl_pc_ptr->width;
    ros_msg.height = driver_ptr_->pcl_pc_ptr->height;
    inno_frame_pub_->publish(ros_msg);
    driver_ptr_->pcl_pc_ptr->clear();
  }

  void publishPacket(const int8_t * pkt, uint64_t pkt_len, double timestamp, bool next_idx)
  {
    if (next_idx) {
      inno_scan_msg_->header.stamp = inno_scan_msg_->packets.front().stamp;
      inno_scan_msg_->header.frame_id = lidar_config_.frame_id;
      if (driver_ptr_->anglehv_table_init_) {
        seyond::msg::SeyondPacket msg;
        msg.type = msg.PACKET_TYPE_HVTABLE;
        msg.data.resize(driver_ptr_->anglehv_table_.size());
        std::memcpy(
          msg.data.data(), driver_ptr_->anglehv_table_.data(), driver_ptr_->anglehv_table_.size());
        inno_scan_msg_->packets.emplace_back(msg);
      }
      inno_pkt_pub_->publish(*inno_scan_msg_);
      inno_scan_msg_ = std::make_unique<seyond::msg::SeyondScan>();
    }
    seyond::msg::SeyondPacket msg;
    msg.stamp = rclcpp::Time(static_cast<int64_t>(timestamp * 1000));
    // msg.stamp = node_ptr_->get_clock()->now();
    msg.type = msg.PACKET_TYPE_POINTS;
    msg.data.resize(pkt_len);
    std::memcpy(msg.data.data(), pkt, pkt_len);
    inno_scan_msg_->packets.emplace_back(msg);
  }

  void publishFrame(const pcl::PointCloud<SeyondPoint> & frame, double timestamp)
  {
    sensor_msgs::msg::PointCloud2 ros_msg;
    pcl::toROSMsg(frame, ros_msg);
    ros_msg.header.frame_id = lidar_config_.frame_id;

    // Use timestamp from the first point in the frame if available
    if (!frame.points.empty()) {
      double point_timestamp = frame.points.front().timestamp;
      rclcpp::Time stamp(static_cast<int64_t>(point_timestamp));
      ros_msg.header.stamp = stamp;
    } else {
      // Fallback to the provided timestamp if the frame is empty
      ros_msg.header.stamp = rclcpp::Time(static_cast<int64_t>(timestamp * 1000));
    }
    ros_msg.width = frame.width;
    ros_msg.height = frame.height;
    inno_frame_pub_->publish(ros_msg);
  }

  void rosLogCallback(int32_t level, const char * header2, const char * msg)
  {
    switch (level) {
      case INNO_LOG_LEVEL_FATAL:
      case INNO_LOG_LEVEL_CRITICAL:
        RCLCPP_FATAL(this->get_logger(), "%s %s", header2, msg);
        break;
      case INNO_LOG_LEVEL_ERROR:
      case INNO_LOG_LEVEL_TEMP:
        RCLCPP_ERROR(this->get_logger(), "%s %s", header2, msg);
        break;
      case INNO_LOG_LEVEL_WARNING:
      case INNO_LOG_LEVEL_DEBUG:
        RCLCPP_WARN(this->get_logger(), "%s %s", header2, msg);
        break;
      case INNO_LOG_LEVEL_INFO:
        RCLCPP_INFO(this->get_logger(), "%s %s", header2, msg);
        break;
      case INNO_LOG_LEVEL_TRACE:
      case INNO_LOG_LEVEL_DETAIL:
      default:
        RCLCPP_DEBUG(this->get_logger(), "%s %s", header2, msg);
    }
  }

private:
  std::string log_level_;
  seyond::LidarConfig lidar_config_;
  std::unique_ptr<seyond::DriverLidar> driver_ptr_;

  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr inno_frame_pub_;
  rclcpp::Publisher<seyond::msg::SeyondScan>::SharedPtr inno_pkt_pub_;
  rclcpp::Subscription<seyond::msg::SeyondScan>::SharedPtr inno_pkt_sub_;

  std::unique_ptr<seyond::msg::SeyondScan> inno_scan_msg_;
};
}  // namespace seyond_node

#include <rclcpp_components/register_node_macro.hpp>

RCLCPP_COMPONENTS_REGISTER_NODE(seyond_node::SeyondNode)
