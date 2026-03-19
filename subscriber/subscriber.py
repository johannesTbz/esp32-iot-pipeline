import json
from datetime import datetime
import paho.mqtt.client as mqtt

BROKER_HOST = "localhost"
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
        except json.JSONDecodeError:
            print(f"[{timestamp()}] ❌ Invalid JSON: {payload}")

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