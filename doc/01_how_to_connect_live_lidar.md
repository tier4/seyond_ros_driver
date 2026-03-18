# 01_how_to_connect_live_lidar

## 1.1 Launch file

```
seyond.launch.xml              // launch with config.yaml
test.py                        // launch test node
```

## 1.2 Connect to lidar using config.yaml

1. Create a new YAML file or use the installed config.yaml file, and modify or input the necessary configurations as needed.

```yaml
/**:
  ros__parameters:
    log_level: info                                     # Log level: info, warn, error

    lidar_name: seyond                                  # Lidar name
    lidar_ip: 172.168.1.10                              # Lidar ip
    port: 8010                                          # Lidar port
    udp_port: 8010                                      # Udp port
```

2. Start the Driver

```bash
ros2 launch seyond seyond.launch.xml
ros2 launch seyond seyond.launch.xml config_path:=<config_file_path>
```

## 1.3 Start using command line parameters

```bash
ros2 launch seyond seyond.launch.xml lidar_ip:=172.168.1.10 udp_port:=8010
```
