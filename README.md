# Introduction

**seyond_lidar_ros** is a driver package for the Seyond LiDAR, compatible with both ROS and ROS2.

This project is built on the inno-lidar-sdk and serves as a demonstration for customers, providing a practical reference on how to utilize the inno-lidar-sdk.

## Directory structure

```text
├── build.bash                          // compile script
├── create_deb.bash                     // create deb script
├── doc                                 // documentation
├── src
│   └── seyond_lidar_ros                // seyond ros package
├── LICENSE
├── AGENTS.md
├── CHANGELOG.md
└── README.md
```

## Lidar supported

- Falcon-K1

- Falcon-K2

- Robin-W

- Robin-ELITE(Robin-E1X)

## Environment and Dependencies

The following official versions are verified to support

| Distro   | Platform     | Release data   | EOL data      |
| -------- | ------------ | -------------- | ------------- |
| Melodic  | Ubuntu 18.04 | May 23rd, 2018 | June 2023     |
| Noetic   | Ubuntu 20.04 | May 23rd, 2020 | May 2025      |
| Foxy     | Ubuntu 20.04 | June 5th, 2020 | June 2023     |
| Galactic | Ubuntu 20.04 | May 23rd, 2021 | December 2022 |
| Humble   | Ubuntu 22.04 | May 23rd, 2022 | May 2027      |
| Iron     | Ubuntu 22.04 | May 23rd, 2023 | December 2024 |
| Jazzy    | Ubuntu 24.04 | May 23rd, 2024 | May 2029      |

## Yaml

Installation:

```bash
sudo apt-get update
sudo apt-get install -y  libyaml-cpp-dev
```

## Config params

please refer to [config.yaml](/src/seyond_lidar_ros/config/config.yaml), support multiple lidars input

## ROS Command Line Parameters

| Parameter          | Default Value | description                                                                             |
| ------------------ | ------------- | --------------------------------------------------------------------------------------- |
| config_path        | ""            | config_path, if use this param, other params will become invalid                        |
| log_level          | info          | limit log from lidar, can choose from (info warn error)                                 |
| replay_rosbag      | false         | replay rosbag packet flag                                                               |
| packet_mode        | true          | packet mode enable                                                                      |
| publish_pointcloud | true          | publish pointcloud                                                                      |
| frame_id           | seyond        | -                                                                                       |
| lidar_name         | seyond        | lidar name                                                                              |
| lidar_ip           | 172.168.1.10  | -                                                                                       |
| port               | 8010          | tcp port                                                                                |
| udp_port           | 8010          | udp port                                                                                |
| reflectance_mode   | true          | 0:intensity mode 1:reflectance mode                                                     |
| multiple_return    | 1             | lidar detection echo mode                                                               |
| continue_live      | false         | fatal error encountered, restart driver                                                 |
| pcap_file          | ""            | path to pcap file for playback                                                          |
| hv_table_file      | ""            | path of hv table file, only for generic lidar                                           |
| packet_rate        | 10000         | file playback rate, if value <= 100 : value MB/s, if value > 100 : value / 10000 x      |
| file_rewind        | 0             | number of file replays 0:no rewind -1: unlimited times                                  |
| max_range          | 2000          | display point maximum distance (m)                                                      |
| min_range          | 0.4           | display point minimum distance (m)                                                      |
| name_value_pairs   | ""            | some settings of lidar are consistent with the usage of inno_pc_client                  |
| coordinate_mode    | 3             | convert the xyz direction of a point cloud, x/y/z, 0:up/right/forward 3:forward/left/up |
| transform_enable   | false         | transform enable                                                                        |
| x                  | 0.0           | translation X (m)                                                                       |
| y                  | 0.0           | translation Y (m)                                                                       |
| z                  | 0.0           | translation Z (m)                                                                       |
| pitch              | 0.0           | pitch (rad)                                                                             |
| yaw                | 0.0           | yaw (rad)                                                                               |
| roll               | 0.0           | roll (rad)                                                                              |
| transform_matrix   | ""            | transform matrix string, if not empty, priority is higher than x/y/z/pitch/yaw/roll     |

## Getting the source

This repository uses a Git submodule (`inno-lidar-sdk`). Clone with submodules using one of the following methods.

**Clone with submodules (recommended):**

```bash
git clone --recurse-submodules <repository-url>
cd seyond_ros_driver
```

**If you have already cloned without submodules:**

```bash
git submodule update --init --recursive
```

## Development

Pre-commit runs markdownlint, shellcheck, clang-format, clang-tidy, cpplint, and others. **clang-tidy** runs via LLVM's `run-clang-tidy.py` (in `scripts/`) with parallel jobs; it requires a compilation database—build with `colcon build --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON` so that `build/seyond/compile_commands.json` exists—otherwise the hook will fail.

## Quick start

If you have your own ROS/ROS2 workspace, please refer to [ROS package build](src/seyond_lidar_ros/README.md)

### Compile

```bash
source /opt/ros/<ROS_DISTRO>/setup.sh
./build.bash
```

### Run

```bash
source install/setup.bash
ros2 launch seyond seyond.launch.xml
```

## Additional guides

[Live lidar](doc/01_how_to_connect_live_lidar.md)

[Parse pcap file](doc/03_how_to_parse_pcap_data.md)

[Point type](doc/02_how_to_change_point_type.md)

[Record & Replay](doc/04_how_to_record_data.md)

[Transform pointcloud](doc/05_how_to_enable_transform.md)

[Frame Test](doc/06_how_to_use_test_node.md)

[Create deb](doc/08_how_to_create_deb.md)

[Fuse multiple lidars](doc/07_how_to_fuse_multiple_lidars.md)
