项目名称：ESP8266控制LED

项目描述：这是一个使用ESP8266控制LED灯的项目。它使用了MQTT协议通过Wi-Fi连接将消息发送到服务器，从而控制LED灯的亮灭。

硬件要求：ESP8266开发板、LED灯、电阻、杜邦线。

软件要求：uPyCraft IDE、Python3

使用方法：

1. 使用uPyCraft IDE将代码烧录到ESP8266开发板上。
2. 使用电阻将LED灯连接到ESP8266开发板的GPIO2口上。
3. 使用Wi-Fi连接ESP8266开发板。
4. 通过MQTT协议发送消息来控制LED灯。

代码说明：

1. 代码中使用了machine模块来控制GPIO口。
2. 代码中使用了umqtt模块来连接MQTT服务器。
3. 代码中使用了network模块来连接Wi-Fi。
4. 代码中使用了config模块来存储Wi-Fi和MQTT服务器的配置。
5. 代码中使用了sub_cb函数来订阅主题并处理接收到的消息，从而控制LED灯的亮灭。

注意事项：

1. 代码中使用了一些第三方模块，请确保已经安装这些模块。
2. 如果更改了Wi-Fi或MQTT服务器的配置，请在config.py文件中修改。
3. ESP8266 ESP-12 模块默认使用 GPIO2 引脚作为 LED 灯，如果需要使用其他引脚请在代码中进行修改。
4. 由于 MicroPython 的限制，连接 MQTT 服务器时不支持 SSL 加密。
5. 如果程序意外终止，请确保断开 WiFi 和 MQTT 连接。
