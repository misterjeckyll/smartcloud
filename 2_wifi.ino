
IPAddress apIP(172, 217, 28, 1);

struct _wifiSettings {
  char ssid[SSID_LEN];
  char password[PWD_LEN];
  char outputIp[IP_LEN];
};
_wifiSettings wset;

void accessPointInit(){
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("NUAGE");
}


bool connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.hostname("NUAGE"); 
  WiFi.begin(wset.ssid, wset.password);
  int i = 0;
  bool state = true;
  while (WiFi.status() != WL_CONNECTED) {
    blinkRGB(RgbColor(50,30,0), 125, 125);
    if (i > 50 || WiFi.status() == WL_CONNECT_FAILED) {
      state = false;
      WiFi.disconnect();
      for (byte n = 0; n < (4 * 4); n++) blinkRGB(RgbColor(30,0,0), 125, 125);
      break;
    }
    i++;
  }
  return state;
}

void connectOTA() {
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
  });
  ArduinoOTA.onEnd([]() {});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});  
  ArduinoOTA.onError([](ota_error_t error) {});
  ArduinoOTA.begin(); 
}

void initEEPROM() {
  EEPROM.begin(512);
  EEPROM.get(WS_ADDR, wset);
  EEPROM.end();
}

//-----------------------------------------------------------------------------------
// MULTICAST DNS
//-----------------------------------------------------------------------------------


void multicastDnsInit(void) {
  // Init the (currently empty) host domain string with 'esp8266'
  if (!MDNS.begin("nuage")) {
    Serial.println(" Error setting up MDNS responder!");
  }else{
    Serial.println("MDNS responder started at http://esp8266-4b35f1.local/");
  }
}

void multicastDnsUpdate(void) {
  MDNS.update();
}
