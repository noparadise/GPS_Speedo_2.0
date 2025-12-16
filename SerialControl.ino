bool processSerial(){
  
  // return result = true only if changes made which require saving to EEPROM
  
  bool result = false;
  
  String inData = "";
  while (Serial.available() > 0) {
    char inchar = Serial.read();
      if (inchar == '\n'){
        if (inData.indexOf("#") == 0){
          // process wifi credentials
          int sep = inData.indexOf("/");
          ssid = inData.substring(1, sep);
          password = inData.substring(sep+1);
          result = connectWifi();
        } else 
        
        {
          int len = inData.length();
          for (int i=0;i<len;i++){
            char inchar = inData.charAt(i);
            if (inchar == 'h')
            {
              printHelp();
            } else 
            if (inchar == 'i')
            {
              printStatus();
            } else 
            if (inchar == 'd')
            {
              dumpVars();
            } else 
            
            if (inchar == '^')
            {
              adjustBrightness(brightness+1); 
              result = true;
            } else 
            if (inchar == '%')
            {
              adjustBrightness(brightness-1);
              result = true;
            } else 
            if (inchar == 'R')
            {
              readSettings();
            } else 
            if (inchar == 'm')
            {
              mqtt_active = !mqtt_active;
              if (is_connected && mqtt_active){
                setup_mqtt();
              } else {
                disconnect_mqtt();
              }
            } else
            if (inchar == 'X')
            {
               clearEprom();  
            } else
            if (inchar == '!')
            {
               Serial.println("Reset device");
               ESP.restart(); //ESP.reset();
            }
          }
        }
      } else {
        inData += inchar; 
      }
      delay(20); // wait for serial to catch up
  }
  return result;
}

void printStatus(){
  Serial.println("\nTime: " + hrTime());
  Serial.print("Speed: ");
  Serial.println(ground_speed);
  Serial.print("Lat/Long: ");
  Serial.print(gps.location.lat());
  Serial.print("/");
  Serial.println(gps.location.lng());
  Serial.print("HDOP: ");
  Serial.println(gps.hdop.value() / 100.0);
  int satcount = gps.satellites.value();
  Serial.print("Satellites: ");
  Serial.println(satcount);
  Serial.print("Brightness: ");
  Serial.println(brightness);
}

void printHelp(){
  Serial.println("\nSerial controls:");
  Serial.println("h : show this list");
  Serial.println("i : status info");
  Serial.println("^ : brighter");
  Serial.println("% : dimmer");
  Serial.println("m : toggle MQTT active");
  Serial.println("! : reset");
  Serial.println("X : delete saved settings");
  Serial.println("#ssid/password : set wifi credentials");
  Serial.println("\n");
}

void dumpVars(){
  Serial.println();
  Serial.println("- ssid \"" + ssid + "\"");
  Serial.println("- password \"" + password + "\"");
  Serial.print("- brightness ");
  Serial.println(brightness);
  if(is_connected){
    Serial.print("- IP address ");
    Serial.println(local_ip);
  } else {
    Serial.print("- IP address ");
    Serial.println(local_ip);
  }
  Serial.println();
}
