from paho.mqtt import client as mqtt_client

broker = '10.0.0.101'
port = 1883
TOPIC_RECEIVE = "NODEMCU_RECEIVE"
# TOPIC_PUBLISH = "NODEMCU_PUBLISH"
client_id = 'MQTTClientIHM'
username = 'aluno'
password = '@luno*123'

class ClientMQTT():

    def __init__(self):
        print("Starting Connection...")
        self.payload_received = ''
        self.topic_received = ''

    def connect_mqtt(self) -> mqtt_client:
        def on_connect(client, userdata, flags, rc):
            if rc == 0:
                print("Connected to MQTT Broker!")
            else:
                print("Failed to connect, return code %d\n", rc)

        client = mqtt_client.Client(client_id)
        client.username_pw_set(username, password)
        client.on_connect = on_connect
        client.connect(broker, port)
        return client


    def subscribe(self, client: mqtt_client):
        def on_message(client, userdata, msg):
            print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")
            self.payload_received = msg.payload.decode()
            self.topic_received = msg.topic

        client.subscribe(TOPIC_RECEIVE)
        # client.subscribe(TOPIC_PUBLISH)
        client.on_message = on_message


    def run(self):
        client = self.connect_mqtt()
        self.subscribe(client)
        client.loop_start()