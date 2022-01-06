# IoTbackend
collection of source code about IoT and its backend-system 

bagaimana IoT membaca sensor dan mengirimkan data ke collector , server,  sampai ke database.

perjalanan :
1. mem-program IoT berbasis ESP32 (arduino) dengan komunikasi data via WiFi, yang mengirimkan nilai dari sensor internal ke MQTT Broker
    (mqtt_internaltemp_pub01_deepSleep.ino)
2. memanfaatkan MQTT broker public dan gratis, test.mosquitto.org. lalu menggunakan aplikasi MQTT Client (windows dan android)
    https://www.youtube.com/watch?v=o0WfWDUqaPs
3. membangun service Subscriber (nodeJS) data IoT untuk mendapatkan data sensor dan mem-forward ke database. 
    (mtestMosq01.js)
    

    
