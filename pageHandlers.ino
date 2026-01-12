
#define FUNCTION "function"
#define SETTINGS "settings"

void indexPage  () {
  Serial.println("serve /");
  String mpage = pageHeader(WiFi.hostname(), ssid);
  String paths = "";
  for (byte idx = 0; idx < sizeof(localPaths) / sizeof(localPaths[0]); idx++) {
    paths += listItem(webLink("/" + localPaths[idx],  localPaths[idx]));
  }
  mpage += wrapUList(paths);

  if(gps.location.isValid()){
    String latlon = String(gps.location.lat(),6) + "," + String(gps.location.lng(),6); //comma seperated lat lon
    mpage += mapEmbed(latlon, 'k', 20);
  }
  server.send(200, mime_type, mpage);
}

void handleGps() {
  Serial.println("serve /gps");
  // build page
  
  String mpage = pageHeader(WiFi.hostname(), ssid);
  String formTemplate = "";

  mpage += htmHead(1, "GPS Speedometer");
  if(gps.location.isValid()){
    String latlon = String(gps.location.lat(),6) + "," + String(gps.location.lng(),6); //comma seperated lat lon
    mpage += "<p>speed: " + String(ground_speed) + "mph";
    mpage += "<p>time: " + hrTime();
    mpage += "<p>coords: " + latlon;
  
    mpage += "<p>sats: " + String(gps.satellites.value());
    mpage += "<p>hdop: " + String(gps.hdop.value() / 100.0);
    mpage += "<p>1/hdop: " + String(1/gps.hdop.value());

    mpage += mapEmbed(latlon, 'm', 15);
  } else {
  
    mpage += "<p>GPS location is invalid";
    mpage += "<p>sats: " + String(gps.satellites.value());
    mpage += "<p>hdop: " + String(gps.hdop.value() / 100.0);
  }
  
  server.send(200, mime_type, mpage);
}

void handleSettings() {
  Serial.println("serve /settings");
  if (server.arg(FUNCTION) == "matrix") {
    if (server.hasArg("bri")){
      adjustBrightness(server.arg("bri").toInt());
    }
  }
  if (server.arg(FUNCTION) == "inv") {
    orientation = (server.arg("orient") == "on");
  }
  if (server.arg(FUNCTION) == "mirr") {
    headup = (server.arg("head") == "on");
  }
  if (server.arg(FUNCTION) == "disp") {
    saveSettings();
  }
  if (server.arg(FUNCTION) == "rst") {
    noticePage("device reset");
    delay(100);
    ESP.restart();
  }
  // MQTT setting
  if (server.arg(FUNCTION) == "nwk") {
    if ((server.hasArg("mqtt") && server.arg("mqtt") == "on") != mqtt_active){
      mqtt_active = !mqtt_active;
      saveSettings();
      if (is_connected && mqtt_active){
        setup_mqtt();
      } else {
        disconnect_mqtt();
      }
    }
  }
  if (server.arg(FUNCTION) == "mqt") {
    mqtt_server = server.arg("ms");
    mqtt_password = server.arg("mp");
    mqtt_username = server.arg("mu");
    saveSettings();
  }
  
  // build page
  String mpage = settingsPage();
  server.send(200, mime_type, mpage);
}

void noticePage(String message){
    String mpage = pageHeader(WiFi.hostname(), ssid);
    mpage += "<p>" + message;
    server.send(200, mime_type, mpage);
}

String mapEmbed(String latlon, char map_type, int map_zoom){
  /*
   * from:
   * https://moz.com/blog/everything-you-never-wanted-to-know-about-google-maps-parameters
map_type t= 
  m – normal  map
  k – satellite
  h – hybrid
  p – terrain
   */
   
  String embedmap = "<div style='width: 100%'><iframe width='100%' height='400' frameborder='0' scrolling='no' marginheight='0' marginwidth='0' ";
  embedmap += "src='https://maps.google.com/maps?width=100%25&amp;height=400&amp;hl=en&amp;q=";
  embedmap += latlon;
  embedmap += "+(GPS Speedo)&amp;t=" + String(map_type) + "&amp;z=" + String(map_zoom) + "&amp;output=embed'></iframe></div>";
  return embedmap;
}

String settingsPage(){
  String mpage = pageHeader(WiFi.hostname(), ssid);
  String formTemplate = "";  
  
  // display settings
  formTemplate += htmHead(3, "Display settings");
  formTemplate += formStart(SETTINGS, FUNCTION, "matrix");
  formTemplate += formRange("Brightness", "bri", brightness, 0, 15, true, false);
  formTemplate += formEnd();  
  // headup
  formTemplate += formStart(SETTINGS, FUNCTION, "mirr");
  formTemplate += formCheckedbox("head", "Headup display (mirrored)", headup, true);
  formTemplate += formEnd();  
  // orientation
  formTemplate += formStart(SETTINGS, FUNCTION, "inv");
  formTemplate += formCheckedbox("orient", "Orientation", orientation, true);
  formTemplate += formEnd();  
  
  formTemplate += formStart(SETTINGS, FUNCTION, "disp");
  formTemplate += formSubmitEnd("Save current orientation &amp; brightness");
  
  formTemplate += formStart(SETTINGS, FUNCTION, "rst");
  formTemplate += formSubmitEnd("Reset device");

  // mqtt settings
  formTemplate += htmHead(3, "MQTT settings");
  formTemplate += formStart(SETTINGS, FUNCTION, "nwk");
  formTemplate += formCheckedbox("mqtt", "MQTT active", mqtt_active, true);
  formTemplate += formEnd();  

  // mqtt network settings
  formTemplate += htmHead(3, "MQTT Server Credentials");
  formTemplate += formStart(SETTINGS, FUNCTION, "mqt");
  formTemplate += "<p>" + formTextInput("mqtt server", "ms", mqtt_server);
  formTemplate += "<p>" + formTextInput("mqtt user", "mu", mqtt_username);
  formTemplate += "<p>" + formTextInput("mqtt password", "mp", "");
  formTemplate += "<p>" + formSubmitEnd("save");
  mpage += formTemplate;
  
  return mpage;
}
