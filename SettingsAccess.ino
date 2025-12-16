
#include <EEPROM.h>

// EEPROM storage version
#define eeVersion 1
//reserve the first bytes of EEPROM byte 1 version, 2 bytes for conf length, 1 spare
#define eestore 4
// EEPROM size is variable: from 4 to 4096 on huzzah but 2048 ok
#define esize 2048

bool getSettings(){
  JsonDocument doc;
  Serial.println("read settings");
  if(readEeprom(doc)){
    jsonToVars(doc);
//    String jsonVars;
//    serializeJsonPretty(doc, jsonVars);
//    Serial.println(jsonVars);
    return true;
  } else {
    Serial.println("EEPROM read err: vars from config");
    ssid = m_ssid;
    password = m_password;
    mqtt_server = m_mqtt_server;
    mqtt_username = m_mqtt_username;
    mqtt_password = m_mqtt_password;
  }
  return false;
}

void readSettings(){
  String outputString;
  JsonDocument doc;
  readEeprom(doc);
  serializeJsonPretty(doc, outputString);
  Serial.println("readSettings() from EEPROM:");
  Serial.println(outputString);
}


void saveSettings() {
  JsonDocument doc = varsToJson();
  writeEeprom(doc);
//  Serial.println("check read:");
//  readSettings();
}


JsonDocument varsToJson() {
  JsonDocument doc;
  doc["ap"] = ssid;
  doc["pw"] = password;
  doc["br"] = brightness;
  doc["or"] = orientation;
  doc["he"] = headup;
  doc["mq"] = mqtt_active;
  doc["mqsrv"] = mqtt_server;
  doc["mqusr"] = mqtt_username;
  doc["mqpwd"] = mqtt_password;
  doc["tv"] = "thingon2025_gps";
  return doc;
}

void jsonToVars(JsonDocument doc) {
  if (doc.containsKey("ap")) {
    ssid = doc["ap"].as<String>();
  }
  if (doc.containsKey("pw")) {  
    password = doc["pw"].as<String>();
  }
  if (doc.containsKey("br")) {
    adjustBrightness(doc["br"]);
  }
  if (doc.containsKey("he")) {
    headup = doc["he"].as<bool>();
  }
  if (doc.containsKey("or")) {
    orientation = doc["or"].as<bool>();
  }
  if (doc.containsKey("mq")) {
    mqtt_active = doc["mq"].as<bool>();
  }
  if (doc.containsKey("mqsrv")) {
    mqtt_server = doc["mqsrv"].as<String>();
  }
  if (doc.containsKey("mqusr")) {
    mqtt_username = doc["mqusr"].as<String>();
  }
  if (doc.containsKey("mqpwd")) {
    mqtt_password = doc["mqpwd"].as<String>();
  }
//    updateLedVars(doc);
}

bool readEeprom(JsonDocument& doc) {
  // set the vars from the json stored in eeprom
  // ignore first byte as version is only 1
  // if eestore changes, that would become v2 otherwise we can put whatever in json
  String result = "";
  EEPROM.begin(esize);
  int lobits = EEPROM.read(1);
  int hibits = EEPROM.read(2);
  int count = (hibits << 8) | lobits;
  for (int addr = 0; addr < count; addr++) {
    int value = EEPROM.read(addr + eestore);
    result += (char) (value);
  }
  EEPROM.end();

  DeserializationError error = deserializeJson(doc, result);  
  if (error.code() == DeserializationError::Ok) {
    return true;
  } else {
    Serial.print("DeserializationError: ");
    Serial.println(error.c_str());
    return false;
  }
}

void writeEeprom(JsonDocument doc) {
  Serial.print("writing to EEPROM: ");
  String jsonVars;
  serializeJson(doc, jsonVars);
//  Serial.println(jsonVars);

  const char * m_chars = jsonVars.c_str();
  EEPROM.begin(eestore + jsonVars.length());
  EEPROM.write(0, eeVersion); // version
  EEPROM.write(1, jsonVars.length());
  EEPROM.write(2, jsonVars.length() >> 8);
  for (unsigned int addr = eestore; addr < eestore + jsonVars.length(); addr++) {
    EEPROM.write(addr, (byte) m_chars[addr - eestore]);
  }
  EEPROM.end();
  Serial.println("done");
}


void clearEprom() {
  Serial.println("***** erasing EEPROM memory *****");
  EEPROM.begin(esize);
  for (int i = 0; i < esize; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}
