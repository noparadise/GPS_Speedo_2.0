
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// server
ESP8266WebServer server(80);
const String mime_type = "text/html";
const String applying = "applying changes";
int found_SSID;

// endpoints to index for /
const String localPaths[] = {"wifi", "gps", "settings"};

bool manageConnection(){
  if (connectWifi()){
    return true;
  }
  setupAP(ROOT_ID, "8888");
  return false;
}

bool connectWifi(){
  scanForAPs();
  WiFi.mode(WIFI_STA);
  host_name = WiFi.hostname();
  host_name.replace("ESP", ROOT_ID);
  WiFi.hostname(host_name);
  WiFi.begin(ssid, password);

  // Wait for connection
  Serial.println("\n\nConnecting to " + ssid);
//  int count = 0;
  m_time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    if (millis() > m_time + 10000) {
      Serial.println("\nConnect fail after WifiBusiness.timer mS");
      return false;
    }
  }
  local_ip = WiFi.localIP().toString();
  Serial.println("\nConnected to " + ssid + " as " + WiFi.hostname() + " at " + local_ip);
  return true;
}

void startServer(){
  Serial.println("start web interface");
  
  server.on("/", indexPage);
  server.on("/wifi", handleSetup);
  server.on("/settings", handleSettings);

  // custom handlers in pageHandlers ...
  server.on("/gps", handleGps);
  //
  server.begin();
}

void handleHttp(){
  server.handleClient();
}

void handleSetup() {
  Serial.println("serve /wifi");
  String mpage = pageHeader(WiFi.hostname(), ssid) + htmHead(3, String(found_SSID) + " networks found");

  if (server.args() > 0) {
    Serial.print("setting ");
    if (server.arg("func") == "setwf") {
      Serial.println("wifi");
      storeWifi();
    }
  }
  if (found_SSID > 0) {
    // have discovered >0 APs
    String formTemplate = formStart("wifi", "func", "setwf");
    String apOptions = "";
    String apList = "";
    for (int i = 0; i < found_SSID; ++i) {
      // get password for preferred network
      String ssid_i = String(WiFi.SSID(i));
      apOptions += singleOption(ssid_i, ssid_i, (ssid == ssid_i));
      apList += listItem(ssid_i + " (" + String(WiFi.RSSI(i)) + "dB)");
    }
    formTemplate += "networks found:" + wrapUList(apList);
    formTemplate += wrapOptions("network: ", "ap", "ap", false, apOptions);
    formTemplate += "<p>" + formTextInput("password", "pw", "");
    formTemplate += formSubmitEnd("save");
    mpage += formTemplate;
  }
  server.send(200, mime_type, mpage);
}

/*
 * custom page handlers ...
 */

int scanForAPs() {
  // scan the air for wifi
  WiFi.disconnect();
  delay(100);
  found_SSID = WiFi.scanNetworks();
  Serial.print("scan found ");
  Serial.print(found_SSID);
  Serial.println(" APs");
  for (int i = 0; i < found_SSID; ++i) {
    Serial.print("  " + WiFi.SSID(i) + ":");
    Serial.print(WiFi.RSSI(i));
    Serial.println("dBm");
  }
  return found_SSID;
}

void storeWifi() {
  // store wifi credentials on successful connect
  ssid = server.arg("ap");
  password = server.arg("pw");
  if (connectWifi()) {
    saveSettings();
  } else {
    Serial.println("couldn't connect - reverting");
    delay(20);
    setup();
  }
}

void setupAP(String l_ssid, String l_pass) {
  
  WiFi.disconnect();
  // set up as AP
  WiFi.softAP(l_ssid, l_pass);
  IPAddress l_ip = WiFi.softAPIP();
  Serial.print("set up as ");
  Serial.print(l_ssid);
  Serial.print(" serving on ");
  Serial.println(l_ip);
  local_ip = l_ip.toString();
}
