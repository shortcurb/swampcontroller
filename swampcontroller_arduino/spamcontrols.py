import paho.mqtt.client as mqtt
import time,random,os
from dotenv import load_dotenv
load_dotenv()


# Define the MQTT broker details
broker_ip = os.environ['broker_ip']
broker_port = os.environ['broker_port']
username = os.environ['username']
password = os.environ['password']
topic = os.environ['topic']

# Callback function when the client connects to the broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to broker")
    else:
        print("Connection failed with code", rc)

# Create an MQTT client instance
client = mqtt.Client()

# Set the username and password
client.username_pw_set(username, password)

# Assign the on_connect callback function
client.on_connect = on_connect

# Connect to the MQTT broker
client.connect(broker_ip, broker_port, 60)

# Function to publish a message
def publish_message(topic,message):
    result = client.publish(topic, message)
    if result.rc == mqtt.MQTT_ERR_SUCCESS:
        print(f"published {message}")
    else:
        print("Failed to publish message")

# Example usage: publish a message
# Uncomment the line below to publish a message
# publish_message("Hello, this is a test message")

a = ['SWAMP_HIGH','SWAMP_LOW','FAN_HIGH','FAN_LOW','PUMP_ON','OFF'
]

def waiter(length):
     for w in range(length,0,-1):
            print(w)
            time.sleep(1)

def looper():
    while True:
        i = random.randint(0,len(a)-1)
        publish_message(topic,a[i])
        for w in range(7,0,-1):
            print(w)
            time.sleep(1)
#looper() 
a = 7
#publish_message(topic,'PUMP_ON')
#publish_message(topic,'FAN_LOW')
#waiter(a)
#publish_message(topic,'FAN_HIGH')
#waiter(a)
#waiter(a)
publish_message(topic,'SWAMP_LOW')
waiter(a)
#publish_message(topic, 'SWAMP_HIGH')
publish_message(topic,'OFF')

# Start the MQTT client loop to process network traffic and dispatch callbacks
client.loop_start()

# Stop the loop and disconnect after some time
# Uncomment the lines below to stop the loop and disconnect
# import time
# time.sleep(10)
# client.loop_stop()
# client.disconnect()

# swampcommandfeed is a MQTT subscription that looks like this:
#Adafruit_MQTT_Subscribe swampcommandfeed = Adafruit_MQTT_Subscribe(&mqtt, mqttuser "/swampcommand");

# swampcommandresponse is an MQTT publish that looks like this:
#Adafruit_MQTT_Publish swampresponsefeed = Adafruit_MQTT_Publish(&mqtt, mqttuser "/swampresponse");
'''
if swampcommandfeed.lastread == 'SWAMP_HIGH':
    pins = [1,1,1]
elif swampcommandfeed.lastread == "SWAMP_LOW":
    pins = [1,1,0]

setRelays(pins)
swampcommandresponse.publish(f"pump:{pins[0]}, fan trigger:{pins[1]}, fan select:{pins[2]}")


def setRelays(pins):
    digitalWrite(pin1,pins[0])
    digitalWrite(pin2,pins[1])
    digitalWrite(pin3,pins[2])

'''