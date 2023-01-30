此目录用于存放mosquitto的配置文件及指令示意。

- `mosquitto.conf`配置文件，存放于/etc/mosquitto目录下。



开启LED

```shell
mosquitto_pub -u ESP32-C3-username -P ESP32-C3-password -t TestTopic -h 192.168.10.173 -m "on"
```

关闭LED

```shell
mosquitto_pub -u ESP32-C3-username -P ESP32-C3-password -t TestTopic -h 192.168.10.173 -m "on"
```

LED状态变更

```shell
mosquitto_pub -u ESP32-C3-username -P ESP32-C3-password -t TestTopic -h 192.168.10.173 -m "toggle"
```
