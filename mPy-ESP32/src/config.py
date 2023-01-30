# -*-coding:utf-8-*-
import ubinascii
import machine
from secret import *

# Default MQTT server to connect to
username = "ESP32-C3-username"
password = "ESP32-C3-password"
SERVER = "192.168.10.173"
CLIENT_ID = ubinascii.hexlify(machine.unique_id())
TOPIC = b"TestTopic"  # bytes对象，为网络通讯设计
