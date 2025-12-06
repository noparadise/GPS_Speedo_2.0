

void gps_update(){
  
  if (gps.location.isUpdated()) {
    ground_speed = gps.speed.mph();
    myTimeStamp = tmConvert_t(gps);      
    
    currentCourse = TinyGPSPlus::courseTo(
      LAST_LAT,
      LAST_LON,
      gps.location.lat(),
      gps.location.lng()
      );
    cardinal_led = ledFromBearing(currentCourse, COMPASS_LEDS);
    LAST_LAT = gps.location.lat();
    LAST_LON = gps.location.lng();
  
  }
}

String hrTime(){
  char mbuffer[6];
  sprintf(mbuffer, "%02d:%02d:%02d", gps.time.hour() + getBSTOffset(myTimeStamp)%24, gps.time.minute(), gps.time.second());
  return mbuffer;
}


time_t tmConvert_t(TinyGPSPlus &gps) {
  tmElements_t tm;
  tm.Year = gps.date.year() - 1970;
  tm.Month = gps.date.month();
  tm.Day = gps.date.day();
  tm.Hour = gps.time.hour();
  tm.Minute = gps.time.minute();
  tm.Second = gps.time.second();
  return makeTime(tm);
}
