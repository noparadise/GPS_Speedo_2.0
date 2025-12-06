

void handleGps() {
  Serial.println("serve /gps");
  // perform functions according to post vars or whatever

//  mqtt
//  brightness
  
  // build page
  String mpage = gpsPage();
  server.send(200, mime_type, mpage);
}

String gpsPage(){
    // build form page
  String mpage = pageHeader(WiFi.hostname(), ssid);
  String formTemplate = "";

  String latlon = String(gps.location.lat()) + "," + String(gps.location.lng()); //comma seperated lat lon
  mpage += htmHead(1, "GPS Speedometer");

  mpage += "<p>speed: " + String(ground_speed) + "mph";
  mpage += "<p>time: " + hrTime();

  mpage += "<p>sats: " + String(gps.satellites.value());
  mpage += "<p>hdop: " + String(gps.hdop.value() / 100.0);
  mpage += "<p>1/hdop: " + String(1/gps.hdop.value());
  mpage += "<p>brightness: " + String(brightness);

  mpage += "<div style='width: 100%'><iframe width='720' height='400' frameborder='0' scrolling='no' marginheight='0' marginwidth='0' ";
  mpage += "src='https://maps.google.com/maps?width=720&amp;height=400&amp;hl=en&amp;q=";
  mpage += latlon;
  mpage += "+(GPS Speedo)&amp;t=k&amp;z=14&amp;ie=UTF8&amp;iwloc=B&amp;output=embed'></iframe></div>";
  
  
  return mpage;
}
