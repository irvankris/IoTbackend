# IoTbackend
collection of source code about IoT and its backend-system 

bagaimana IoT membaca sensor dan mengirimkan data ke collector , server,  sampai ke database.

perjalanan :
1. mem-program IoT berbasis ESP32 (arduino) dengan komunikasi data via WiFi, 

    yang mengirimkan nilai dari sensor internal ke MQTT Broker
    (mqtt_internaltemp_pub01_deepSleep.ino)
    
    yang mengirimkan nilai dari sensor BMP280 i2c, temperatur dan pressure, ke MQTT Broker
    (mqtt_BMP280_pub01_deepSleep.ino)

2. memanfaatkan MQTT broker public dan gratis, test.mosquitto.org. lalu menggunakan aplikasi MQTT Client (windows dan android)
    https://www.youtube.com/watch?v=o0WfWDUqaPs
3. membangun service Subscriber (nodeJS) data IoT untuk mendapatkan data sensor dan mem-forward ke time-series-database. 
    (mtestMosq01.js)
    
    
    
PYTHON mqtt client :

menyediakan service http, untuk mempublish message ke suatu topic di mqtt server

C:\Python310\python.exe X:\PREK\python\MQTT\mqhttp03.py 

merupakan service yang membaca topic dengan wildcard, sehingga dapat menerima published_message dari beberapa device.
juga memberikan service http_to_mqttpublish, sebagai PoC interface aplikasi berbasis API http berkomunikasi ke device.

refer ke youtube https://www.youtube.com/watch?v=EoQvo3xu1Jk


--------------------------------------------------------------------------------------------------------------------

C:\Python310\python.exe X:\PREK\python\MQTT\mqhttp01.py 

script ini connect ke mq_server dengan user = userku01
ini adalah mem-publish message "hello world",
ke topic "/ujimqtt/userku02/from"
untuk menguji :
http://127.0.0.1:8000/mqtt?topic=ujimqtt%2Fuserku02%2Ffrom&message=hello_world

script ini juga men-subscribe ke topic "/ujimqtt/userku01/to" (userku01 di dalam topic, menggunakan nama user, yaitu  userku01)
message yang diterima di topic tersebut, di-print ke stdout.

untuk mensimulasikan kirim terima message antara userku01 dan userku02,
menggunakan dua service http dengan port berbeda, semisal userku01 di port 8080 dan userku02 di port 9000


C:\Python310\python.exe X:\PREK\python\MQTT\mqhttp02.py 

untuk menguji http://127.0.0.1:9000/mqtt?topic=ujimqtt%2Fuserku01%2Ffrom&message=hello_world

------------------------------------------------------------------------------------------------------------------------------------------



    
