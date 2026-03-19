# ESP32 IoT Light Sensor Pipeline

A simple end-to-end IoT project built with ESP32 (ESP-IDF).  
The device reads light levels from a photoresistor, processes the data, and publishes it over MQTT to a Docker-based broker.

---

## Features

- Read analog light sensor (photoresistor) using ADC  
- Convert raw values to percentage  
- Detect **day/night status**  
- Connect to WiFi using ESP-IDF  
- Publish data using MQTT  
- Run MQTT broker locally using Docker (Mosquitto)  
- Subscribe to MQTT data in real time using a Python client

---

## Architecture

```
Photoresistor → ESP32 → WiFi → MQTT → Docker (Mosquitto) → python Subscriber
```

---

## Example MQTT Messages

### Topic: `light-sensor/data`

```json
{"raw":2832,"percent":31,"status":"night"}
```


### Topic: `light-sensor/status`
```
night
```

```
### Example Python Subscriber Output
```

```
[14:32:10] Connected to MQTT broker: Success
[14:32:10] Subscribed to topics
[14:32:11] 📊 DATA | raw=977 | 77% | day
[14:32:11] 🌙 STATUS | day
```

---

## Setup

### 1. ESP32 (ESP-IDF)

Update WiFi credentials in main.c:

#define WIFI_SSID "YOUR_WIFI"
#define WIFI_PASS "YOUR_PASSWORD"

Set MQTT broker IP (your computer’s local IP):

#define MQTT_BROKER_URI "mqtt://YOUR_PC_IP:1883"

Build and flash:

```bash
idf.py build
idf.py -p COMX flash monitor
```

### 2. MQTT Broker (Docker)

Make sure Docker is running, then start Mosquitto:

```bash
docker compose up -d
```
### 3. Subscribe to data

Listen to sensor data:

```bash
docker run --rm -it eclipse-mosquitto:2 mosquitto_sub -h host.docker.internal -p 1883 -t "light-sensor/data"
```

Listen to status:

```bash
docker run --rm -it eclipse-mosquitto:2 mosquitto_sub -h host.docker.internal -p 1883 -t "light-sensor/status"
```

### 4. Python Subscriber

Install dependencies:

```bash
pip install -r subscriber/requirements.txt
```
Run the subscriber:

```bash
python subscriber/subscriber.py
```

---

## Tech Stack

- ESP32 (ESP-IDF, C)

- MQTT (Mosquitto)

- Docker

- Python

- WiFi networking

---

## What I learned

- Event-driven WiFi handling in ESP-IDF

- ADC sensor reading and basic signal processing

- MQTT publish/subscribe pattern

- Running infrastructure (Mosquitto) in Docker

- Building a Python MQTT subscriber for real-time data consumption

- Building a complete IoT data pipeline
---

## Future improvements

- Add MQTT connection event handling

- Store data in a database (e.g. InfluxDB)

- Build a dashboard (Grafana / web app)

- Add more sensors

- Implement OTA updates