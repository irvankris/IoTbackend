## untuk menguji http://127.0.0.1:9000/mqtt?topic=ujimqttku%2Fto%2Fuser44&message=hello_world
# C:\Python310\Scripts\pip3.exe install flask
# C:\Python310\Scripts\pip3.exe install paho-mqtt
# C:\Python310\python.exe X:\PREK\python\MQTT\mqhttp03.py 

from flask import Flask, request
import paho.mqtt.client as mq

import sys
 
#surpress flask logging 
import logging
log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR) 
  
 
import time

def waktuku():
    return int(time.time() * 1000) 


 
 
def print_to_stderr(*a):
 
    # Here a is the array holding the objects
    # passed as the argument of the function
    print(*a, file=sys.stderr)
 
def print_to_stdout(*a):
 
    # Here a is the array holding the objects
    # passed as the argument of the function
    print(*a, file=sys.stdout)
 




#ipmqtt = 'broker.emqx.io' 
#ipmqtt = 'test.mosquitto.org' 
#ipmqtt = 'broker.hivemq.com'
ipmqtt = '192.168.1.9'

portmqtt = 1883 



usermqtt = 'servicenya'
passmqtt = 'servicenyajuga'

#publish dan subscribed , bersilangan , seperti TX ke RX
publisedtopic = 'ujimqttku/' + 'userku01' + '/to'
#publisedtopic = 'ujimqttku/' + usermqtt + '/from'
subcribedtopic = 'ujimqttku/from/#' 
heartbeattopic = 'hbmqttku/statusnya/#' 

iphttp = '0.0.0.0'
porthttp = 9000

app = Flask(__name__)

#MQConnected                #Use global variable
MQConnected = False                #Signal connection 

mqtt = mq.Client("restMQTT"+usermqtt)

def on_connect(mqttku, userdata, flags, rc):
    mqttku.subscribe(subcribedtopic)
    mqttku.subscribe(heartbeattopic)
    
    if rc == 0:
  
        #print_to_stdout("Connected to broker")
        
        global MQConnected
        #=False:
        #if MQConnected==False:    
        #    mqttku.subscribe(subcribedtopic)
        #    print_to_stdout(subcribedtopic + ' > subscribed')
        
        MQConnected = True                #Signal connection 
  
    else:
  
        print_to_stdout("Connection failed")


    #mqttku.subscribe(subcribedtopic)
    #print_to_stdout(str(waktuku())+ ' '+ publisedtopic)
    #mqttku.publish(publisedtopic, "STARTING SERVER")
    #mqttku.publish(publisedtopic, "CONNECTED")


def on_message(mqtt, userdata, msg):
    #mqttku.publish(publisedtopic, "MESSAGE")
    print_to_stdout(f"Message received [{msg.topic}]: {msg.payload}")



@app.route('/mqtt', methods=['GET'])
def get_id():
      
  topic = request.args.get('topic')    
  message = request.args.get('message')    
  
  mqtt.publish(topic, message)
  print_to_stdout('publish|| '+topic+ '||' +  message)
  
  #mqtt.publish(publisedtopic, message)
  #print_to_stdout('publish|| '+publisedtopic+ '||' +  message)
  return topic + '||' +  message
  
  
  
  
if __name__ == '__main__': 
  #mqtt = mq.Client("restMQTT")
  mqtt.username_pw_set(usermqtt,passmqtt)
  mqtt.on_connect = on_connect
  mqtt.on_message = on_message  
  mqtt.connect(ipmqtt, portmqtt, 15)  
  mqtt.loop_start()

  
  ##app.run(host='0.0.0.0', port=port)  
  app.run(host=iphttp, port=porthttp, debug=False)
  
  