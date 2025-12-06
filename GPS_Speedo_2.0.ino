

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <TimeLib.h>
#include "LedControl.h"

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

/*
 * always apt remove brltty unless u r blind
 */

// local config
#include "config.h"

const String ROOT_ID = "GPS_speedo";

/****** WiFi Connection Details *******/
String ssid;
String password;

String local_ip;
String host_name;

/******* MQTT Broker Connection Details *******/
const char* mqtt_server = "653c384af3e04c82bb411d8545dc7fc4.s1.eu.hivemq.cloud";
const char* mqtt_username = "GPS_speedo";
const char* mqtt_password = "G0nz4l3z";
const int mqtt_port = 8883;
/**** Secure WiFi Connectivity Initialisation *****/
WiFiClientSecure espClient;

/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);
bool mqtt_active = false;

/** turn on/off whole network business  **/
bool is_connected = false;

#define panels 2
#define orientation false
#define headup false

#define DIN 16 // D0 GPIO 16
#define CS 5 // D1 GPIO 5
#define CLK 4 // D2 GPIO 4

#define RX 14 // D6 GPIO 14
#define TX 12 // D7 GPIO 12
#define SERIAL_BAUD 9600

#define COMPASS_LEDS 16
#define potPin A0

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RX, TX);
LedControl lc=LedControl(DIN,CLK,CS,panels);

int potVal = 0; // 0-1024
int brightness = 15;

double LAST_LAT = 51.508131, LAST_LON = -0.128002;
int cardinal_led = 0;
unsigned long m_time = millis();
unsigned long pub_time = millis();
unsigned long ping_time = millis();

int ground_speed = 0;
double currentCourse;
time_t myTimeStamp;


void setup() {
  Serial.begin(SERIAL_BAUD);
  while(!Serial){
    delay(100);
  }
  Serial.println("\n\nSerial free");

/*
  //  for proto reset   
  clearEprom();  
/**/

  getSettings();
  
  initPanels();
  for (int i = 0;i<200;i++){
    showSpeed(i);
    delay(20);
  }
  delay(1000);
  clearPanels();
  
  gpsSerial.begin(SERIAL_BAUD);
  while(!gpsSerial){
    delay(100);
  }
  delay(250);
  Serial.println("\nSoftware Serial started");

  /// connectivity
  is_connected = manageConnection();
  if (is_connected && mqtt_active){
    setup_mqtt();
  }
  startServer();
}

void loop() {
  if(processSerial()) saveSettings();
  
  handleHttp();
  
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (millis() - m_time > 1000) {
    gps_update();
    /*
    */
    clearPanels();
    showSpeed(ground_speed);
    m_time = millis();

    // mqtt stuff loop
    if (mqtt_active){
      if (is_connected && (millis() - ping_time > 2000)) {
        mqUpdate();
      }
    }
  }
}

void checkPot(){
  int potRead = analogRead(potPin); 
  if (abs(potVal - potRead) > 4){ 
    potVal = potRead;
    adjustBrightness(potVal/64);
    Serial.print("potVal ");
    Serial.println(potVal);
  }
}

int getBSTOffset(unsigned long t_then){
  setTime(t_then);
  int m_then = month();
  int w_then = weekday();
  int h_then = hour();
  if(m_then>10 || m_then < 3){
    return 0;
  }
  if(m_then > 3 && m_then<10){
    return 1;
  }
  if (m_then==3){
    if(w_then == 1 && h_then < 1){
      return 0;
    }
    if (month(nextSunday(t_then)) > 3){
      return 1;
    } 
  }
  if (m_then==10){
    if(w_then == 1 && h_then < 1){
      return 1;
    }
    if (month(nextSunday(t_then)) > 10){
      return 0;
    } 
    return 1;
  }
  return 0;
}
