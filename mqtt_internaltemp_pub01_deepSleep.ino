/*********
initial script.


important things:  number one , ITS WORK!
number two, Think while its working.

"sing penting, nomer siji, mlaku dhisik"
"nomor loro, dipikir karo mlaku"

MQTT di ESP32.

mem-publish internal sensor yang ada di ESP32,
- temperature
- Hall Sensor (dikirim sebagai "humidity")

setelah proses kirim, maka ESP32 akan deep-sleep sebelum restart kembali.

*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_system.h"

#include <Wire.h>




void IRAM_ATTR resetModule(){
    ets_printf("reboot\n");
    //esp_restart_noos();
    esp_restart();
}


// Replace the next variables with your SSID/Password combination
//const char* ssid = "mySSID";
//const char* password = "myWIFIpass";





//MODEL B

const char* mqtt_server = "test.mosquitto.org";
#define mqtt_port 1883
#define MQTT_USER "user001"
#define MQTT_PASSWORD "pass001"



long delaynum01 = 300000; /* dalam milisec 5 menit = 5*60*1000 */
long delaysend01 = 20000; /*retry attempt mqtt*/
//long delaysend01 = 10000;
long delaysleep = 120 * 1000000;
//long delaysleep = 30 * 1000000;

hw_timer_t *timer = NULL;
int retrycon = 30;



String prefixKU = "mytopic007";
String CLIENTIDKU = "Incoming";

String idKU = "id01 ";


#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//uncomment the following lines if you're using SPI
/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI
float temperature = 0;
float humidity = 0;

char payloadku[12];
char tempString[10];
char tempTopic[128];
char humString[8];

    
// LED Pin
const int ledPin = 4;

String uint64ToString(uint64_t input) {
  String result = "";
  uint8_t base = 10;

  do {
    char c = input % base;
    input /= base;

    if (c < 10)
      c +='0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while ((WiFi.status() != WL_CONNECTED) ) {
    if (retrycon==0){
        Serial.println("gak bisa konek Wifi!");
        delay(1000);
        ESP.deepSleep(delaysleep);
    }
      
    retrycon--; 
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());





  
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == (prefixKU+ "/"+CLIENTIDKU+"/mqtt/output")) {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected

String str = prefixKU+ "/"+CLIENTIDKU+"/mqtt/output"; 
 
// Length (with one extra character for the null terminator)
int str_len = str.length() + 1; 
 
// Prepare the character array (the buffer) 
char char_array01[str_len];
// Copy it over 
str.toCharArray(char_array01, str_len);

str = CLIENTIDKU; 
str_len = str.length() + 1; 
char char_array02[str_len];
str.toCharArray(char_array02, str_len);
 


  
  while (!client.connected()) {

    if (retrycon==0){
        Serial.println("gak bisa konek Wifi!");
        delay(1000);
        ESP.deepSleep(delaysleep);
    }

    retrycon--; 
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(char_array02,MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(char_array01);
          timerWrite(timer, 0); //mereset watchdog bahwa mesin masih OK, setiap terima dari server
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(delaysend01);
      
    }
  }
}

void senddata() {

String str;
int str_len;
    
    timerWrite(timer, 0); //mereset watchdog bahwa mesin masih OK, setiap terima dari server
    memset(tempString, 0, sizeof(tempString));
    memset(payloadku, 0, sizeof(payloadku));
    memset(humString, 0, sizeof(humString));     
    payloadku[0]= '\0';
    humString[0]= '\0';
    tempString[0]= '\0';


  float shuntvoltage1 = 0;
  float busvoltage1 = 0;
  float current_mA1 = 0;
  float loadwatt1 = 0;


    // Temperature in Celsius
    //temperature = bme.readTemperature();   
    temperature = (temprature_sens_read() - 32) / 1.8 ;
    // Uncomment the next line to set temperature in Fahrenheit 
    // (and comment the previous temperature line)
    //temperature = 1.8 * bme.readTemperature() + 32; // Temperature in Fahrenheit
    
    // Convert the value to a char array
	// ini adalah topic yang dikirim ke server MQTT
    str = prefixKU+ "/"+CLIENTIDKU+"/mqtt/temperature"; 
    str_len = str.length() + 1; 
    str.toCharArray(tempTopic, str_len);
    memset(tempString, 0, sizeof(tempString));
    memset(payloadku, 0, sizeof(payloadku));
    tempString[0]= '\0';
    payloadku[0]= '\0';
    strcat(payloadku, "N:");
    dtostrf(temperature, 1, 2, tempString);
    strcat(payloadku, tempString);    
    Serial.print("temperature: ");
    Serial.println(payloadku);
    client.publish(tempTopic, payloadku,true);


 
    //humidity = bme.readHumidity();
    humidity = hallRead();
    // Convert the value to a char array

 
	// ini adalah topic yang dikirim ke server MQTT  
    str = prefixKU+ "/"+CLIENTIDKU+"/mqtt/humidity"; 
    str_len = str.length() + 1; 
    str.toCharArray(tempTopic, str_len);
    memset(tempString, 0, sizeof(tempString));
    memset(payloadku, 0, sizeof(payloadku));
    tempString[0]= '\0';
    payloadku[0]= '\0';
    strcat(payloadku, "N:");
    dtostrf(humidity, 1, 2, tempString);
    strcat(payloadku, tempString);    
    Serial.print("humidity: ");
    Serial.println(payloadku);
    client.publish(tempTopic, payloadku,true);

    


  
}

void setup() {
  Serial.begin(115200);
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  //status = bme.begin();  

//  if (!bme.begin(0x76)) {
//    Serial.println("Could not find a valid BME280 sensor, check wiring!");
//    while (1);
//  }

    timer = timerBegin(0, 80, true); //timer 0, div 80
    timerAttachInterrupt(timer, &resetModule, true);
    timerAlarmWrite(timer, (delaynum01*1000)+1000000, false); //set time in us
    timerAlarmEnable(timer); //enable interrupt
  

  idKU = uint64ToString(ESP.getEfuseMac());
  //idKU.remove(0, 6);
  CLIENTIDKU = idKU;
  //CLIENTIDKU = "167461624524336";

  setup_wifi();




 
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);

  if (!client.connected()) {
    if (retrycon==0){
        delay(1000);
        ESP.deepSleep(delaysleep);
    }

    reconnect();
  }


  senddata();
  Serial.println("going into deep sleep");
  delay(1000);
  ESP.deepSleep(delaysleep);

  
}



void loop() {

/*
String str = CLIENTIDKU+"/localhost/mqtt/temperature"; 
 
// Length (with one extra character for the null terminator)
int str_len = str.length() + 1; 
 
// Prepare the character array (the buffer) 
char char_array01[str_len];
// Copy it over 
str.toCharArray(char_array01, str_len);

str = CLIENTIDKU+"/localhost/mqtt/humidity"; 
str_len = str.length() + 1; 
char char_array02[str_len];
str.toCharArray(char_array02, str_len);
*/
 
  


}
