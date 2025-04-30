/*
 *  Copyright (C) 2024 Seyond Inc.
 *
 *  License: Apache License
 *
 *  $Id$
 */

#include <signal.h>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include "src/driver/ros2_driver_adapter.hpp"

static void shutdown_callback(int sig) {
  rclcpp::shutdown();
}


int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);

  signal(SIGINT, shutdown_callback);
  std::shared_ptr<ROSNode> ros_driver_ptr = std::make_shared<ROSNode>();
  ros_driver_ptr->init();
  ros_driver_ptr->start();
  ros_driver_ptr->spin();
  return 0;
}
