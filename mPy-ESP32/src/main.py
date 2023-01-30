# -*-coding:utf-8-*-
import time
from machine import Pin
import network
from umqtt import MQTTClient
from config import *

ConnectionError = OSError

# ESP8266 ESP-12 modules have blue, active-low LED on GPIO2, replace
# with something else if needed.
led = Pin(13, Pin.OUT, value=1)


def connect_wifi(ssid, passwd):
    print("connecting wifi!")
    global wlan
    wlan = network.WLAN(network.STA_IF)  # create a wlan object
    wlan.active(True)  # Activate the network interface
    wlan.disconnect()  # Disconnect the last connected WiFi
    wlan.connect(ssid, passwd)  # connect wifi
    while (wlan.ifconfig()[0] == '0.0.0.0'):
        time.sleep(1)
    # print(wlan.ifconfig())
    wlan_info = wlan.ifconfig()
    print("Wifi is connected with the following information:")
    print("IP address : " + wlan_info[0])
    print("Subnet mask : " + wlan_info[1])
    print("Gateway : " + wlan_info[2])
    print("DNS : " + wlan_info[3])


state = 0


def sub_cb(topic, msg):
    global state
    print((topic, msg))
    if msg == b"off":
        led.value(0)
        state = 1
        print("1")
    elif msg == b"on":
        led.value(1)
        state = 0
        print("0")
    elif msg == b"toggle":
        # LED is inversed, so setting it to current state
        # value will make it toggle
        led.value(state)
        state = 1 - state


# Catch exceptions,stop program if interrupted accidentally in the 'try'
try:
    connect_wifi(SSID, PASSWORD)
    server = SERVER
    client = MQTTClient(CLIENT_ID, server, 0, username, password,
                        keepalive=30, ssl=False, ssl_params={})  # create a mqtt client
    client.set_callback(sub_cb)  # set callback
    client.connect()  # connect mqtt
    client.subscribe(TOPIC)  # client subscribes to a topic
    client.publish(TOPIC, "Hi")  # client publish to a topic
    print("Connected to %s, subscribed to %s topic" % (server, TOPIC))
    # print(f"Connected to {server}, subscribed to {TOPIC} topic") # mPy不支持

    while True:
        try:
            client.wait_msg()  # wait message
        except (OSError, ConnectionError) as e:
            if e.args[0] == 104 or e.args[0] == -1:
                time.sleep(3)
                print("Connection break! Reconnecting...")
                client.connect()  # reconnect
                client.subscribe(TOPIC)
            else:
                raise
finally:
    # if (c is not None):
    #     c.disconnect()
    wlan.disconnect()
    wlan.active(False)
