/*********
initial script.


important things:  number one , ITS WORK!
number two, Think while its working.

"sing penting, nomer siji, mlaku dhisik"
"nomor loro, dipikir karo mlaku"

MQTT di ESP32.

mem-publish sensor BME280 terhubung menggunakan I2C,
- temperature
- pressure

setelah proses kirim, maka ESP32 akan deep-sleep sebelum restart kembali.

*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_system.h"

#include <Wire.h>

// BMP280 I2C address is 0x76(108)
#define Addr 0x76



void IRAM_ATTR resetModule(){
    ets_printf("reboot\n");
    //esp_restart_noos();
    esp_restart();
}


/ Replace the next variables with your SSID/Password combination
const char* ssid = "mySSID";
const char* password = "myWIFIpass";





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


float temperature = 0;


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


  unsigned int b1[24];
  unsigned int data[8];
  for (int i = 0; i < 24; i++)
  {
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select data register
    Wire.write((136 + i));
    // Stop I2C Transmission
    Wire.endTransmission();

    // Request 1 byte of data
    Wire.requestFrom(Addr, 1);

    // Read 1 byte of data
    if (Wire.available() == 1)
    {
      b1[i] = Wire.read();
    }
  }
  // Convert the data
  // temp coefficients
  unsigned int dig_T1 = (b1[0] & 0xFF) + ((b1[1] & 0xFF) * 256);
  int dig_T2 = b1[2] + (b1[3] * 256);
  int dig_T3 = b1[4] + (b1[5] * 256);

  // pressure coefficients
  unsigned int dig_P1 = (b1[6] & 0xFF) + ((b1[7] & 0xFF) * 256);
  int dig_P2 = b1[8] + (b1[9] * 256);
  int dig_P3 = b1[10] + (b1[11] * 256);
  int dig_P4 = b1[12] + (b1[13] * 256);
  int dig_P5 = b1[14] + (b1[15] * 256);
  int dig_P6 = b1[16] + (b1[17] * 256);
  int dig_P7 = b1[18] + (b1[19] * 256);
  int dig_P8 = b1[20] + (b1[21] * 256);
  int dig_P9 = b1[22] + (b1[23] * 256);

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select control measurement register
  Wire.write(0xF4);
  // Normal mode, temp and pressure over sampling rate = 1 ; 001 001 11 , osrs_t osrs_p mode
  //normal mode untuk sistem dengan power supply kontinyu
  // Forced mode, temp and pressure over sampling rate = 1 ; 010 111 10 , osrs_t osrs_p mode
  //forced mode untuk sistem dengan power supply tidak kontinyu (dengan power gating)
//  Wire.write(0x27);
  Wire.write(0x5E);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select config register
  Wire.write(0xF5);
  // Stand_by time = 1000ms
  Wire.write(0xA0);
  // Stop I2C Transmission
  Wire.endTransmission();

  for (int i = 0; i < 8; i++)
  {
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select data register
    Wire.write((247 + i));
    // Stop I2C Transmission
    Wire.endTransmission();

    // Request 1 byte of data
    Wire.requestFrom(Addr, 1);

    // Read 1 byte of data
    if (Wire.available() == 1)
    {
      data[i] = Wire.read();
    }
  }

  // Convert pressure and temperature data to 19-bits
  long adc_p = (((long)(data[0] & 0xFF) * 65536) + ((long)(data[1] & 0xFF) * 256) + (long)(data[2] & 0xF0)) / 16;
  long adc_t = (((long)(data[3] & 0xFF) * 65536) + ((long)(data[4] & 0xFF) * 256) + (long)(data[5] & 0xF0)) / 16;

  // Temperature offset calculations
  double var1 = (((double)adc_t) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
  double var2 = ((((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0) *
                 (((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0)) * ((double)dig_T3);
  double t_fine = (long)(var1 + var2);
  double cTemp = (var1 + var2) / 5120.0;
  double fTemp = cTemp * 1.8 + 32;

  // Pressure offset calculations
  var1 = ((double)t_fine / 2.0) - 64000.0;
  var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
  var2 = var2 + var1 * ((double)dig_P5) * 2.0;
  var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
  var1 = (((double) dig_P3) * var1 * var1 / 524288.0 + ((double) dig_P2) * var1) / 524288.0;
  var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
  double p = 1048576.0 - (double)adc_p;
  p = (p - (var2 / 4096.0)) * 6250.0 / var1;
  var1 = ((double) dig_P9) * p * p / 2147483648.0;
  var2 = p * ((double) dig_P8) / 32768.0;
  double pressure = (p + (var1 + var2 + ((double)dig_P7)) / 16.0) / 100;

  // Output data to serial monitor
  Serial.print("Pressure : ");
  Serial.print(pressure);
  Serial.println(" hPa");
  Serial.print("Temperature in Celsius : ");
  Serial.print(cTemp);
  Serial.println(" C");
  Serial.print("Temperature in Fahrenheit : ");
  Serial.print(fTemp);
  Serial.println(" F");






    // Temperature in Celsius
    //temperature = bme.readTemperature();   
    temperature = cTemp;
    // Uncomment the next line to set temperature in Fahrenheit 
    // (and comment the previous temperature line)
    //temperature = 1.8 * bme.readTemperature() + 32; // Temperature in Fahrenheit
    
    // Convert the value to a char array

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
//    client.publish(tempTopic, payloadku);


 
  
    str = prefixKU+ "/"+CLIENTIDKU+"/mqtt/pressure"; 
    str_len = str.length() + 1; 
    str.toCharArray(tempTopic, str_len);
    memset(tempString, 0, sizeof(tempString));
    memset(payloadku, 0, sizeof(payloadku));
    tempString[0]= '\0';
    payloadku[0]= '\0';
    strcat(payloadku, "N:");
    dtostrf(pressure, 1, 2, tempString);
    strcat(payloadku, tempString);    
    Serial.print("humidity: ");
    Serial.println(payloadku);
    client.publish(tempTopic, payloadku,true);
//    client.publish(tempTopic, payloadku);

    


  
}

void setup() {

  // Initialise I2C communication as MASTER
  Wire.begin(21,22);
  
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
