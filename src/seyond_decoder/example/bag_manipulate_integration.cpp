/*
 *  Copyright (C) 2025 Seyond Inc.
 *
 *  License: Apache License
 *
 *  Example of using SeyondDecoder in bag_manipulate context
 */

#include <rclcpp/rclcpp.hpp>
#include <rosbag2_cpp/reader.hpp>
#include <rosbag2_cpp/writer.hpp>
#include <rclcpp/serialization.hpp>
#include "seyond_decoder/msg/seyond_scan.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "seyond_decoder/seyond_decoder.hpp"
#include <iostream>
#include <memory>

class SeyondBagConverter {
public:
  SeyondBagConverter(const std::string& input_bag, const std::string& output_bag,
                    const seyond::DecoderConfig& config = seyond::DecoderConfig())
    : input_bag_path_(input_bag), output_bag_path_(output_bag), decoder_(config) {
    std::cout << "SeyondBagConverter initialized" << std::endl;
    std::cout << "Input bag: " << input_bag_path_ << std::endl;
    std::cout << "Output bag: " << output_bag_path_ << std::endl;
  }
  
  void process(const std::string& seyond_topic, const std::string& pointcloud_topic) {
    // Setup reader
    rosbag2_cpp::Reader reader;
    rosbag2_storage::StorageOptions read_storage_options;
    read_storage_options.uri = input_bag_path_;
    read_storage_options.storage_id = "mcap";
    
    rosbag2_cpp::ConverterOptions converter_options{
      rmw_get_serialization_format(),
      rmw_get_serialization_format()
    };
    
    reader.open(read_storage_options, converter_options);
    
    // Setup writer
    rosbag2_cpp::Writer writer;
    rosbag2_storage::StorageOptions write_storage_options;
    write_storage_options.uri = output_bag_path_;
    write_storage_options.storage_id = "mcap";
    
    writer.open(write_storage_options, converter_options);
    
    // Copy all existing topics
    auto topics_and_types = reader.get_all_topics_and_types();
    bool has_seyond_topic = false;
    
    for (const auto& topic_metadata : topics_and_types) {
      if (topic_metadata.name == seyond_topic) {
        has_seyond_topic = true;
        // Skip creating the original SeyondScan topic in output
        std::cout << "Found SeyondScan topic: " << topic_metadata.name << std::endl;
      } else {
        // Copy other topics as-is
        rosbag2_storage::TopicMetadata new_topic_metadata;
        new_topic_metadata.name = topic_metadata.name;
        new_topic_metadata.type = topic_metadata.type;
        new_topic_metadata.serialization_format = topic_metadata.serialization_format;
        new_topic_metadata.offered_qos_profiles = topic_metadata.offered_qos_profiles;
        writer.create_topic(new_topic_metadata);
      }
    }
    
    // Add new PointCloud2 topic
    if (has_seyond_topic) {
      rosbag2_storage::TopicMetadata pointcloud_topic_metadata;
      pointcloud_topic_metadata.name = pointcloud_topic;
      pointcloud_topic_metadata.type = "sensor_msgs/msg/PointCloud2";
      pointcloud_topic_metadata.serialization_format = rmw_get_serialization_format();
      writer.create_topic(pointcloud_topic_metadata);
      std::cout << "Created PointCloud2 topic: " << pointcloud_topic << std::endl;
    }
    
    // Serializers
    rclcpp::Serialization<seyond_decoder::msg::SeyondScan> seyond_serializer;
    rclcpp::Serialization<sensor_msgs::msg::PointCloud2> pointcloud_serializer;
    
    // Process messages
    size_t message_count = 0;
    size_t converted_count = 0;
    
    while (reader.has_next()) {
      auto bag_message = reader.read_next();
      message_count++;
      
      if (bag_message->topic_name == seyond_topic) {
        // Deserialize SeyondScan
        seyond_decoder::msg::SeyondScan scan_msg;
        rclcpp::SerializedMessage serialized_msg(*bag_message->serialized_data);
        seyond_serializer.deserialize_message(&serialized_msg, &scan_msg);
        
        // Convert to PointCloud2
        auto pointcloud_msg = decoder_.convert(scan_msg);
        
        if (pointcloud_msg) {
          // Serialize and write PointCloud2
          rclcpp::SerializedMessage serialized_pointcloud;
          pointcloud_serializer.serialize_message(pointcloud_msg.get(), &serialized_pointcloud);
          
          auto converted_message = std::make_shared<rosbag2_storage::SerializedBagMessage>();
          converted_message->topic_name = pointcloud_topic;
          converted_message->time_stamp = bag_message->time_stamp;
          converted_message->serialized_data = std::make_shared<rcutils_uint8_array_t>();
          *converted_message->serialized_data = serialized_pointcloud.release_rcl_serialized_message();
          
          writer.write(converted_message);
          converted_count++;
        }
      } else {
        // Write other messages as-is
        auto copied_message = std::make_shared<rosbag2_storage::SerializedBagMessage>();
        copied_message->topic_name = bag_message->topic_name;
        copied_message->time_stamp = bag_message->time_stamp;
        copied_message->serialized_data = bag_message->serialized_data;
        writer.write(copied_message);
      }
      
      if (message_count % 1000 == 0) {
        std::cout << "Processed " << message_count << " messages, converted " 
                  << converted_count << " SeyondScan messages" << std::endl;
      }
    }
    
    std::cout << "\nConversion completed!" << std::endl;
    std::cout << "Total messages: " << message_count << std::endl;
    std::cout << "Converted SeyondScan messages: " << converted_count << std::endl;
  }
  
private:
  std::string input_bag_path_;
  std::string output_bag_path_;
  seyond::SeyondDecoder decoder_;
};

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <input_bag> <output_bag> [seyond_topic] [pointcloud_topic]" << std::endl;
    return 1;
  }
  
  std::string input_bag = argv[1];
  std::string output_bag = argv[2];
  std::string seyond_topic = (argc > 3) ? argv[3] : "/seyond_scan";
  std::string pointcloud_topic = (argc > 4) ? argv[4] : "/pointcloud";
  
  // Initialize ROS2
  rclcpp::init(argc, argv);
  
  try {
    // Configure decoder
    seyond::DecoderConfig config;
    config.max_range = 200.0;
    config.min_range = 0.3;
    config.coordinate_mode = 0;
    config.use_reflectance = true;
    config.frame_id = "lidar";
    
    // Create and run converter
    SeyondBagConverter converter(input_bag, output_bag, config);
    converter.process(seyond_topic, pointcloud_topic);
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    rclcpp::shutdown();
    return 1;
  }
  
  rclcpp::shutdown();
  return 0;
}