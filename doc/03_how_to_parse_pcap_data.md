# 03_how_to_parse_pcap_data

## 3.1 Related Parameters

**pcap_file**: pcap file path

**packet_rate**(optional)：Playback rate for the file, default is 10,000 (1.0x).

- When packet_rate=0, playback at maximum speed.
- When packet_rate<=100, playback according to file size at a speed of <packet_rate> MB/s.
- When packet_rate>100, playback at a rate of (<packet_rate>/10,000.0)x.

**file_rewind**: Number of replays, default is 0, which means play once only.

- When file_rewind>=0, replay <file_rewind> times.
- When file_rewind=-1, loop playback.

**Note:** lidar_ip and udp_port need to match the lidar data in the pcap file.

## 3.2 config.yaml

```yaml
/**:
  ros__parameters:
    log_level: info                                     # Log level: info, warn, error

    frame_id: seyond                                    # Frame id
    lidar_name: seyond                                  # Lidar name
    lidar_ip: 172.168.1.10                              # Lidar ip
    udp_port: 8010                                      # Udp port

    pcap_file: '/home/demo/demo.pcap'                   # Pcap file
    packet_rate: 10000                                  # Packet rate
    file_rewind: 0                                      # File rewind
```

```bash
ros2 launch seyond seyond.launch.xml
```

## 3.3 Start using command line parameters

```bash
ros2 launch seyond seyond.launch.xml pcap_file:=<pcap_file_path> lidar_ip:=<lidar_ip> udp_port:=<udp_port>
```
