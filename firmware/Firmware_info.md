# Firmware Information

## Overview
This folder will contain the Arduino code (.ino files) for the ESP8266 microcontroller.

## Components

### Main Files (To be added):
- `esp8266_ddos_simulator.ino` - Main Arduino sketch
- `attack_modules.h` - Attack implementation code
- `web_interface.h` - Web server and UI code
- `config.h` - Configuration settings

## Based On
This project uses/adapts the ESP8266 Deauther firmware by SpacehuhnTech:
- Repository: https://github.com/SpacehuhnTech/esp8266_deauther
- Used for educational and research purposes
- Modified for portable DDoS attack simulation

## How to Use

### Requirements
- Arduino IDE (1.8.x or higher)
- ESP8266 board package
- Required libraries:
  - ESP8266WiFi
  - ESP8266WebServer
  - DNSServer

### Installation Steps
1. Open Arduino IDE
2. Install ESP8266 board support via Board Manager
3. Open the .ino file
4. Select Board: "NodeMCU 1.0 (ESP-12E Module)"
5. Select correct COM port
6. Click Upload

## Acknowledgments
Special thanks to SpacehuhnTech for the original ESP8266 Deauther project.
