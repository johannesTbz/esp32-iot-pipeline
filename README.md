# ESP32 IoT Pipeline

This project is a small IoT system built with ESP-IDF on an ESP32-C6.

## Features
- Reads light level from a photoresistor (LDR)
- Uses ADC (Analog to Digital Converter)
- Logs sensor data over serial

## Hardware
- ESP32-C6-WROOM-1
- Photoresistor module (LDR)

## Current Output Example
Light level raw: 3120

## Next Steps
- Connect ESP32 to WiFi
- Send data using MQTT
- Run MQTT broker in Docker
- Add Python backend subscriber

## Goal
Build a complete IoT pipeline:
Sensor → ESP32 → WiFi → MQTT → Docker → Backend