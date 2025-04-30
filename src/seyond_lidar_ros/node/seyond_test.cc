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
#include "src/test/ros2_test.hpp"

static void shutdown_callback(int sig) {
  rclcpp::shutdown();
}


int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);

  signal(SIGINT, shutdown_callback);

  std::shared_ptr<ROSDemo> ros_driver_ptr = std::make_shared<ROSDemo>();
  ros_driver_ptr->init();
  ros_driver_ptr->spin();
  return 0;
}
