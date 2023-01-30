## MQTT



创建MQTT客户端：

```python
client = MQTTClient(CLIENT_ID, server, 0, username, password, keepalive=30, ssl=False, ssl_params={})  # create a mqtt client
```

连接到服务器：

```python
client.connect()  # connect mqtt
```

设置回调函数，如果接收到消息执行回调函数：

```python
client.set_callback(sub_cb)  # set callback
```

订阅主题：

```python
client.subscribe(TOPIC)  # client subscribes to a topic
```

发布主题：

```python
client.publish(TOPIC, "Hi")  # client publish to a topic
```

检查订阅信息：

```python
client.wait_msg
client.check_msg
# 有啥区别？不知道。
```



```python

```



```python

```



```python

```

