/*
 *  Copyright (C) 2024 Seyond Inc.
 *
 *  License: Apache License
 *
 *  $Id$
 */

#include "src/test/ros2_test.hpp"

#include <rclcpp/rclcpp.hpp>

#include <csignal>
#include <memory>

static void shutdown_callback(int sig)
{
  (void)sig;
  rclcpp::shutdown();
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  (void)signal(SIGINT, shutdown_callback);

  std::shared_ptr<ROSDemo> ros_driver_ptr{std::make_shared<ROSDemo>()};
  ros_driver_ptr->init();
  ros_driver_ptr->spin();
  return 0;
}
