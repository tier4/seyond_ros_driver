# 04_how_to_record_data

## 4.1 Related Parameters

**packet_mode**: Enable packet topic to publish packet data. Default is `true`.

**replay_rosbag**: Enable to replay packet data for parsing recorded packets.

## 4.2 Record

```bash
// record packet bag
ros2 bag record /seyond_packets
// record pointcloud
ros2 bag record /seyond_points
```

## 4.3 Replay

```bash
// replay packet bag
ros2 bag play <packet_rosbag> -l
// enable the driver to parse packet
ros2 launch seyond seyond.launch.xml replay_rosbag:=true
```
