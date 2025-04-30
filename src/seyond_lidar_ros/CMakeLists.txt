cmake_minimum_required(VERSION 3.5)
project(seyond)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

add_compile_options(-Ofast)

# Check and build Seyond SDK if needed
set(SEYOND_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/seyond_sdk/)
set(SEYOND_SDK_LIB_PATH ${SEYOND_SDK_PATH}/lib/libinnolidarsdkclient.a)

if(NOT EXISTS ${SEYOND_SDK_LIB_PATH})
  message(STATUS "Seyond SDK not built yet. Building it now...")
  
  execute_process(
    COMMAND bash -c "cd ${SEYOND_SDK_PATH}/build && ./build_unix.sh"
    RESULT_VARIABLE SDK_BUILD_RESULT
    OUTPUT_VARIABLE SDK_BUILD_OUTPUT
    ERROR_VARIABLE SDK_BUILD_ERROR
  )
  
  if(NOT SDK_BUILD_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to build Seyond SDK: ${SDK_BUILD_ERROR}")
  else()
    message(STATUS "Seyond SDK built successfully")
  endif()
else()
  message(STATUS "Seyond SDK already built")
endif()

#-------------------------------------
# Point Type (PointXYZI, PointXYZIT)
#-------------------------------------
set(POINT_TYPE PointXYZIT)

if(${POINT_TYPE} STREQUAL "PointXYZI")
  add_definitions(-DENABLE_XYZI)
elseif(${POINT_TYPE} STREQUAL "PointXYZIT")
  add_definitions(-DENABLE_XYZIT)
endif()


if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wno-format-extra-args -Wno-format -Wno-format-security)
endif()

# find dependencies
find_package(PCL REQUIRED)
find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/SeyondPacket.msg"
  "msg/SeyondScan.msg"
  DEPENDENCIES std_msgs
)

include_directories(./)

# seyond_node
ament_auto_add_executable(seyond_node
  node/seyond_node.cc
  src/driver/driver_lidar.cc
)


if(${rosidl_cmake_VERSION} VERSION_LESS 2.5.0)
  rosidl_target_interfaces(seyond_node
  ${PROJECT_NAME} "rosidl_typesupport_cpp")
else()
  rosidl_get_typesupport_target(
          cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
  target_link_libraries(seyond_node "${cpp_typesupport_target}")
endif()

target_include_directories(seyond_node
  PRIVATE
  src/driver
  ${SEYOND_SDK_PATH}
  ${SEYOND_SDK_PATH}/apps
  ${SEYOND_SDK_PATH}/src
  ${PCL_INCLUDE_DIRS}
)

target_link_directories(seyond_node
  PRIVATE
  ${SEYOND_SDK_PATH}/lib
)

target_link_libraries(seyond_node
  innolidarsdkclient.a
  innolidarsdkcommon.a
  innolidarutils.a
  ${PCL_LIBRARIES}
  ${YAML_CPP_LIBRARIES}
)

ament_target_dependencies(seyond_node
  rclcpp
  PCL
  std_msgs
  sensor_msgs
  message_filters
  pcl_msgs
  pcl_conversions
)

# seyond_test
ament_auto_add_executable(seyond_test
  node/seyond_test.cc
)

if(${rosidl_cmake_VERSION} VERSION_LESS 2.5.0)
  rosidl_target_interfaces(seyond_test
  ${PROJECT_NAME} "rosidl_typesupport_cpp")
else()
  rosidl_get_typesupport_target(
          cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
  target_link_libraries(seyond_test "${cpp_typesupport_target}")
endif()

ament_target_dependencies(seyond_test
  rclcpp
  PCL
  std_msgs
  sensor_msgs
)

set_target_properties(seyond_node PROPERTIES
  BUILD_WITH_INSTALL_RPATH FALSE
  LINK_FLAGS "-Wl,-rpath,$ORIGIN/")

# install group
ament_auto_package(
  INSTALL_TO_SHARE
  launch
  config
  rviz
)
