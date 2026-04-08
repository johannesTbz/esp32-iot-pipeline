# ESP32 IoT Light Sensor Pipeline

A simple end-to-end IoT project built with ESP32 (ESP-IDF).  
The device reads light levels from a photoresistor, processes the data, and publishes it over MQTT to a Docker-based backend that stores it in influxDB.

---

## Features

- Read analog light sensor (photoresistor) using ADC  
- Convert raw values to percentage  
- Detect **day/night status**  
- Connect to WiFi using ESP-IDF  
- Publish data using MQTT  
- Run MQTT broker locally using Docker (Mosquitto)  
- Run a Python MQTT subscriber as a containerized microservice
- Store time-series sensor data in InfluxDB
- Start the backend services with Docker Compose

---

## Architecture

```
Photoresistor → ESP32 → WiFi → MQTT → Docker Compose → Mosquitto → python Subscriber → InfluxDB
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

### Example Python Subscriber Output

```
[14:32:10] Connected to MQTT broker: Success
[14:32:10] Subscribed to topics
[14:32:11] 📊 DATA | raw=977 | 77% | day
[14:32:11] 🌙 STATUS | day
[14:32:11]✅ Wrote to InfluxDB
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
### 2. Environment variables

Create a `.env` file in the project root

### 3. Backend services (Docker Compose)

Make sure Docker is running, then start the backend services:

```bash
docker compose --env-file .env up --buildd
```
This starts:

- mqtt (Mosquitto broker)
- subscriber (Python MQTT subscriber)
- `influxdb` (InfluxDB time-series database)

InfluxDB UI is available at `http://localhost:8086`

### 4. Optional: Subscribe using MQTT CLI

Listen to sensor data:

```bash
docker run --rm -it eclipse-mosquitto:2 mosquitto_sub -h host.docker.internal -p 1883 -t "light-sensor/data"
```

Listen to status:

```bash
docker run --rm -it eclipse-mosquitto:2 mosquitto_sub -h host.docker.internal -p 1883 -t "light-sensor/status"
```

### 5. Optional: Run the Python Subscriber

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

- Docker Compose

- Python (paho-mqtt, influxdb-client)

- InfluxDB

- WiFi networking

---

## What I learned

- Event-driven WiFi handling in ESP-IDF

- ADC sensor reading and basic signal processing

- MQTT publish/subscribe pattern

- Running infrastructure (Mosquitto, InfluxDB) in Docker

- Building a Python MQTT subscriber for real-time data consumption

- Containerizing a Python microservice with a Dockerfile

- Using Docker Compose to orchestrate multiple services

- Writing time-series data to InfluxDB from Python

- Managing environment variables securely with `.env` and Docker Compose

- Building a complete end-to-end IoT data pipeline
---

## Future improvements

- Build a Grafana dashboard to visualize sensor data

- Add more sensors

- Implement OTA updates

- Add MQTT authentication