import paho.mqtt.client as mqtt
import mysql.connector
from mysql.connector import Error
from datetime import datetime
import os
import time


add_temp = ("INSERT INTO temp "
               "(temp, time) "
               "VALUES (%s, %s)")
add_moist = ("INSERT INTO moisture "
               "(moisture, time) "
               "VALUES (%s, %s)")
add_light = ("INSERT INTO light "
               "(light, time) "
               "VALUES (%s, %s)")
                                             
# Define event callbacks
def on_connect(client, userdata, flags, rc):
    print("rc: " + str(rc))

def on_message(client, obj, msg):
    if msg.payload != b'Hello from ESP32':
        try:
            cnx = mysql.connector.connect(user='root', password='1234',
                                host='104.196.173.138',
                                database='sensorData')
            cursor = cnx.cursor()
            data = (msg.payload, datetime.now())
            if msg.topic == temp:
                print(msg.topic)
                cursor.execute(add_temp, data)
            if msg.topic == moist:
                print(msg.topic)
                cursor.execute(add_moist, data)
            if msg.topic == light:
                print(msg.topic)
                cursor.execute(add_light, data)
            cnx.commit()
            cursor.close()
            cnx.close()
        except Error as e:
            print(e)
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

def on_publish(client, obj, mid):
    print("mid: " + str(mid))

def on_subscribe(client, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(client, obj, level, string):
    print(string)

mqttc = mqtt.Client()
# Assign event callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_subscribe = on_subscribe

# Uncomment to enable debug messages
#mqttc.on_log = on_log

# Parse CLOUDMQTT_URL (or fallback to localhost)
url_str = os.environ.get('m15.cloudmqtt.com')
temp = 'Sensors/Temperature'
moist = 'Sensors/Light'
light = 'Sensors/Moisture'
# Connect
mqttc.username_pw_set('python', '1234')
mqttc.connect('m15.cloudmqtt.com',11252) #mqtt TCP port

# Start subscribe, with QoS level 0
mqttc.subscribe(temp, 0)
mqttc.subscribe(moist,0)
mqttc.subscribe(light,0)

# Publish a message
# mqttc.publish(topic, "my message")

# Continue the network loop, exit when an error occurs
rc = 0
while rc == 0:
    rc = mqttc.loop()
print("rc: " + str(rc))
