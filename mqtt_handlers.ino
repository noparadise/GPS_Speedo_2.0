



/************* Set up MQTT ***********/
void setup_mqtt(){
  
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

/************* Connect to MQTT Broker ***********/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";   // Create a random client ID
    clientId += host_name; //String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.publish("connection", host_name.c_str());
      String m_topic = host_name + "_control";
      client.subscribe(m_topic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");   // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqUpdate(){
  
      if (!client.connected()) reconnect(); // check if client is connected
      
      client.loop();
      
      if (millis() - pub_time > 30000 ) {
        DynamicJsonDocument doc(1024);
        
        doc["deviceId"] = WiFi.hostname();
//        char mbuffer[6];
//        sprintf(mbuffer, "%02d:%02d:%02d", gps.time.hour() + getBSTOffset(myTimeStamp)%24, gps.time.minute(), gps.time.second());
        doc["hr_time"] = hrTime();
        doc["mph"] = ground_speed;
        doc["lat"] = gps.location.lat();
        doc["lon"] = gps.location.lng();
        doc["alt"] = gps.altitude.meters();
        doc["epoch"] = myTimeStamp;
        doc["hdg"] = TinyGPSPlus::cardinal(currentCourse);
        doc["sat"] = gps.satellites.value();
        doc["hdop"] = gps.hdop.value() / 100.0;
        
        doc["hdinv"] = 1/gps.hdop.value();
        doc["bri"] = brightness;

        
        //128 is one line - may need to extend
        char mqtt_message[256];
        serializeJson(doc, mqtt_message);
        String m_topic = host_name + "_data";
        publishMessage(m_topic.c_str(), mqtt_message, true);
        pub_time = millis();
      }
      ping_time = millis();
}

/**** Method for Publishing MQTT Messages **********/
void publishMessage(const char* topic, String payload , boolean retained){
  if (client.publish(topic, payload.c_str(), true))
//      Serial.println("Message published to /"+String(topic)+": "+payload);
      Serial.print("*");
}

/***** Call back Method for Receiving MQTT messages // ****/

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char str[length+1];
  int i=0;
  for (i = 0; i < length; i++) {
    str[i]=(char)payload[i];
  }
  str[i] = 0; // Null termination
  Serial.println(str);
 
  if (strcmp(str, "reset") == 0){
    ESP.restart();
  }
  //otherwise assume json payload:
  
  StaticJsonDocument <1024> doc;
  deserializeJson(doc,payload);
  //adjustBrightness
  if (doc.containsKey("bri")) {
    adjustBrightness(doc["bri"]);
  }
  //reset
  if (doc.containsKey("reset") && doc["reset"]) {
    ESP.restart();
  }
}
