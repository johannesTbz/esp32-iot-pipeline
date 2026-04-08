import json
import os
from dotenv import load_dotenv
from datetime import datetime
import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS

# load_dotenv()

# InfluxDB config
BUCKET = os.getenv('INFLUXDB_BUCKET')
influx_client = InfluxDBClient(url=os.getenv('INFLUXDB_URL'),
                token=os.getenv('INFLUXDB_TOKEN'), org=os.getenv('INFLUXDB_ORG'))
write_api = influx_client.write_api(write_options=SYNCHRONOUS)

BROKER_HOST = "mqtt"
BROKER_PORT = 1883

TOPIC_DATA = "light-sensor/data"
TOPIC_STATUS = "light-sensor/status"


def timestamp():
    return datetime.now().strftime("%H:%M:%S")


def on_connect(client, userdata, flags, reason_code, properties=None):
    print(f"[{timestamp()}] Connected to MQTT broker: {reason_code}")
    client.subscribe(TOPIC_DATA)
    client.subscribe(TOPIC_STATUS)
    print(f"[{timestamp()}] Subscribed to topics")


def on_message(client, userdata, msg):
    payload = msg.payload.decode()

    if msg.topic == TOPIC_DATA:
        try:
            data = json.loads(payload)
            print(
                f"[{timestamp()}] 📊 DATA | "
                f"raw={data.get('raw')} | "
                f"{data.get('percent')}% | "
                f"{data.get('status')}"
            )
            point = Point(TOPIC_DATA).tag("location", "Stockholm").field("light", data.get('percent') )
            write_api.write(bucket=BUCKET, record=point)
            print(f"[{timestamp()}] ✅ Wrote to InfluxDB")
        except json.JSONDecodeError:
            print(f"[{timestamp()}] ❌ Invalid JSON: {payload}")
        except Exception as e:
            print(f"[{timestamp()}] ❌ Error: {e}")

    elif msg.topic == TOPIC_STATUS:
        print(f"[{timestamp()}] 🌙 STATUS | {payload}")
    


def main():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(BROKER_HOST, BROKER_PORT, 60)
    client.loop_forever()


if __name__ == "__main__":
    main()