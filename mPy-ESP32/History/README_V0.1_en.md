# Project name: ESP8266_MQTT_LED

This is a simple project that demonstrates how to control an LED using MQTT protocol on an ESP8266 microcontroller.

## Hardware

- ESP8266 ESP-12 module
- LED
- Resistor (220 ohm)
- Breadboard
- Jumper wires

## Software

- MicroPython
- umqtt.simple library

## Wiring

Connect the LED to the ESP8266 as follows:

- Connect the anode (positive, longer leg) of the LED to pin 13 of the ESP8266
- Connect the cathode (negative, shorter leg) of the LED to a 220 ohm resistor
- Connect the other end of the resistor to GND of the ESP8266

## Usage

1. Change the `SSID` and `PASSWORD` variables in the `config.py` file to match your Wi-Fi network's credentials.
2. Change the `SERVER`, `CLIENT_ID`, `username`, `password`, `TOPIC` in the `config.py` file to match your MQTT broker's information.
3. Upload the `main.py` and `config.py` to the ESP8266.
4. Run the `main.py` on the ESP8266.
5. The ESP8266 will connect to your Wi-Fi network and the MQTT broker.
6. Subscribe to the `TOPIC` in the `config.py` file using an MQTT client.
7. Send "on" or "off" message to the `TOPIC` to turn the LED on or off.
8. Send "toggle" message to the `TOPIC` to toggle the LED.

## Note

- Make sure you are using a compatible version of MicroPython for your ESP8266.
- The `config.py` file contains all the configuration settings for the Wi-Fi and MQTT connection, you need to modify the variable in this file to match your setting.
- The LED is connected to pin 13, if you want to use another pin, you need to change the pin number in the `main.py` file.

## Reference

- MicroPython documentation: https://docs.micropython.org/en/latest/esp8266/esp8266/tutorial/intro.html
- umqtt.simple library documentation: https://github.com/micropython/micropython-lib/tree/master/umqtt.simple
- MQTT protocol documentation: http://mqtt.org/documentation

 
