# Documentation Fixes Report

## Summary

Inconsistencies between documentation and source code were identified and fixed. The source code was treated as the source of truth.

## Fixes Applied

### README.md

| Issue                                  | Before                                                          | After                                                                 |
| -------------------------------------- | --------------------------------------------------------------- | --------------------------------------------------------------------- |
| `packet_mode` default value            | `false`                                                         | `true` (matches seyond_node.cc:35, config.yaml:5)                     |
| `aggregate_num` parameter listed       | `aggregate_num \| 20`                                           | Removed (parameter does not exist in source code)                     |
| `frame_topic` parameter listed         | `frame_topic \| iv_points`                                      | Removed (topic is hardcoded as `seyond_points` in seyond_node.cc:70)  |
| `packet_topic` parameter listed        | `packet_topic \| iv_packets`                                    | Removed (topic is hardcoded as `seyond_packets` in seyond_node.cc:75) |
| `publish_pointcloud` parameter missing | Not listed                                                      | Added (exists in seyond_node.cc:32, config.yaml:6)                    |
| Launch command                         | `roslaunch seyond start.launch` / `ros2 launch seyond start.py` | `ros2 launch seyond seyond.launch.xml` (only existing launch file)    |
| Transform doc link                     | `05_how_to_enable_transform.md`                                 | `doc/05_how_to_enable_transform.md` (missing `doc/` prefix)           |
| Directory structure                    | Listed `README_CN.md`                                           | Removed `README_CN.md`, added `AGENTS.md`                             |

### doc/01_how_to_connect_live_lidar.md

| Issue              | Before                                                                                    | After                                                |
| ------------------ | ----------------------------------------------------------------------------------------- | ---------------------------------------------------- |
| Launch file names  | `start.{launch,py}`, `start_with_config.{launch,py}`                                      | `seyond.launch.xml`, `test.py`                       |
| config.yaml format | Multi-lidar YAML format with `frame_topic: /iv_points`                                    | ROS 2 parameter format (`/**:` / `ros__parameters:`) |
| Launch commands    | Referenced `start.launch`, `start_with_config.launch`, `start.py`, `start_with_config.py` | `ros2 launch seyond seyond.launch.xml`               |
| ROS 1 commands     | Included `roslaunch` commands                                                             | Removed (ROS 1 not supported in current codebase)    |

### doc/03_how_to_parse_pcap_data.md

| Issue              | Before                                                                              | After                                  |
| ------------------ | ----------------------------------------------------------------------------------- | -------------------------------------- |
| config.yaml format | Multi-lidar YAML format with `packet_topic: /iv_packets`, `frame_topic: /iv_points` | ROS 2 parameter format                 |
| Launch commands    | `start_with_config.launch`, `start_with_config.py`, `start.launch`, `start.py`      | `ros2 launch seyond seyond.launch.xml` |
| ROS 1 commands     | Included `roslaunch` commands                                                       | Removed                                |

### doc/04_how_to_record_data.md

| Issue                     | Before                                                | After                                                              |
| ------------------------- | ----------------------------------------------------- | ------------------------------------------------------------------ |
| `aggregate_num` parameter | Documented with description                           | Removed (does not exist in source code)                            |
| Topic names               | `/iv_packets`, `/iv_points`                           | `/seyond_packets`, `/seyond_points` (matches seyond_node.cc:70,75) |
| Launch command for replay | `ros2 launch seyond start.launch replay_rosbag:=true` | `ros2 launch seyond seyond.launch.xml replay_rosbag:=true`         |
| ROS 1 commands            | Included `rosbag` / `roslaunch` commands              | Removed                                                            |

### doc/05_how_to_enable_transform.md

| Issue                      | Before                                                  | After                          |
| -------------------------- | ------------------------------------------------------- | ------------------------------ |
| config.yaml format         | Multi-lidar YAML format with `frame_topic: /iv_points`  | ROS 2 parameter format         |
| Dynamic adjustment (ROS 1) | Described `rosrun rqt_reconfigure` with lidar1_x naming | Removed ROS 1 specific content |

### doc/06_how_to_use_test_node.md

| Issue                  | Before                                           | After                                                             |
| ---------------------- | ------------------------------------------------ | ----------------------------------------------------------------- |
| Test node build status | No mention of build status                       | Added note that `seyond_test` build is disabled in CMakeLists.txt |
| Topic default values   | `/iv_points`, `/iv_packets` (with leading slash) | `iv_points`, `iv_packets` (matches test.py:18,23)                 |
| ROS 1 commands         | Included `roslaunch` commands                    | Removed                                                           |

### doc/07_how_to_fuse_multiple_lidars.md

| Issue                            | Before                             | After                                                           |
| -------------------------------- | ---------------------------------- | --------------------------------------------------------------- |
| `fusion_enable` / `fusion_topic` | Documented as available parameters | Noted as not supported (parameters do not exist in source code) |
| Multi-lidar config example       | Full multi-lidar YAML config       | Removed (not applicable)                                        |

## Reference

- Source of truth for parameters: `src/seyond_lidar_ros/src/driver/seyond_node.cc`
- Source of truth for config format: `src/seyond_lidar_ros/config/config.yaml`
- Source of truth for launch files: `src/seyond_lidar_ros/launch/`
