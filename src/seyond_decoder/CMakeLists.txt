cmake_minimum_required(VERSION 3.14)
project(seyond_decoder)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(PCL REQUIRED COMPONENTS common io)
find_package(Eigen3 REQUIRED)

# Generate messages
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/SeyondPacket.msg"
  "msg/SeyondScan.msg"
  DEPENDENCIES builtin_interfaces std_msgs
)

# Include directories
include_directories(
  include
  ${PCL_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
)

# Add SDK include directories
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../seyond_lidar_ros/src/seyond_sdk/src
)

# Create library
add_library(${PROJECT_NAME}_lib SHARED
  src/seyond_decoder.cpp
)

target_include_directories(${PROJECT_NAME}_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Link SDK library
find_library(INNO_SDK_CLIENT_LIB 
  NAMES innolidarsdkclient
  PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../seyond_lidar_ros/src/seyond_sdk/lib
  NO_DEFAULT_PATH
)

find_library(INNO_SDK_COMMON_LIB 
  NAMES innolidarsdkcommon
  PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../seyond_lidar_ros/src/seyond_sdk/lib
  NO_DEFAULT_PATH
)

find_library(INNO_UTILS_LIB 
  NAMES innolidarutils
  PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../seyond_lidar_ros/src/seyond_sdk/lib
  NO_DEFAULT_PATH
)

target_link_libraries(${PROJECT_NAME}_lib
  ${INNO_SDK_CLIENT_LIB}
  ${INNO_SDK_COMMON_LIB}
  ${INNO_UTILS_LIB}
  ${PCL_LIBRARIES}
)

ament_target_dependencies(${PROJECT_NAME}_lib
  rclcpp
  sensor_msgs
  pcl_ros
  pcl_conversions
)

# Link with message generation
rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} rosidl_typesupport_cpp)
target_link_libraries(${PROJECT_NAME}_lib "${cpp_typesupport_target}")

# Export library
ament_export_targets(${PROJECT_NAME}Targets HAS_LIBRARY_TARGET)
ament_export_dependencies(
  rosidl_default_runtime
  rclcpp
  sensor_msgs
  pcl_ros
  pcl_conversions
  PCL
  Eigen3
)

# Install library
install(
  TARGETS ${PROJECT_NAME}_lib
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

# Install headers
install(
  DIRECTORY include/
  DESTINATION include
)

# Build example if requested
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
  add_executable(seyond_decoder_example
    example/decoder_example.cpp
  )
  
  target_link_libraries(seyond_decoder_example
    ${PROJECT_NAME}_lib
  )
  
  ament_target_dependencies(seyond_decoder_example
    rclcpp
    sensor_msgs
  )
  
  # Build bag_manipulate integration example
  find_package(rosbag2_cpp QUIET)
  find_package(rosbag2_storage QUIET)
  
  if(rosbag2_cpp_FOUND AND rosbag2_storage_FOUND)
    add_executable(bag_manipulate_integration
      example/bag_manipulate_integration.cpp
    )
    
    target_link_libraries(bag_manipulate_integration
      ${PROJECT_NAME}_lib
    )
    
    ament_target_dependencies(bag_manipulate_integration
      rclcpp
      sensor_msgs
      rosbag2_cpp
      rosbag2_storage
    )
    
    install(TARGETS
      bag_manipulate_integration
      DESTINATION lib/${PROJECT_NAME}
    )
  else()
    message(STATUS "rosbag2 packages not found, skipping bag_manipulate_integration example")
  endif()
  
  install(TARGETS
    seyond_decoder_example
    DESTINATION lib/${PROJECT_NAME}
  )
endif()

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()