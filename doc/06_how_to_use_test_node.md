# 06_how_to_use_test_node

> **Note:** The test node (`seyond_test`) build is currently disabled in CMakeLists.txt. To use the test node, uncomment the `seyond_test` section in CMakeLists.txt and rebuild.

## 6.1 test.py

| Parameter        | Default Value | description                        |
| ---------------- | ------------- | ---------------------------------- |
| frame_topic      | iv_points     | frame topic to be tested           |
| packet_topic     | iv_packets    | packet topic to be tested          |
| hz               | false         | print frame hz(include points num) |
| window           | 30            | hz window size                     |
| stamp            | false         | print frame stamp                  |
| packet_size      | false         | print frame packet size            |
| packet_loss_rate | false         | print packet loss rate             |

## 6.2 Usage

```bash
// print frame hz and average points per frame
ros2 launch seyond test.py hz:=true window:=30

// print frame timestamp
ros2 launch seyond test.py stamp:=true

// print the packets num (need to enable packet_mode)
ros2 launch seyond test.py packet_size:=true
```
