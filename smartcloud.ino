#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>           
#include <LittleFS.h>
#include "header/cloud.h"

bool serverMode;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Cloud initialisation...");
  FSinit();
  setup_neopix();
  blinkRGB(RgbColor(50,50,128), 1000, 125);
  initEEPROM();
  
  if (connectWifi()) {
    Serial.print("Connected to wifi ");
    Serial.print(WiFi.SSID());
    Serial.print(" by ");
    Serial.println(WiFi.hostname());
    serverMode = false;
    Serial.print("At ");
    Serial.println(WiFi.localIP());
    blinkRGB(RgbColor(0,100,0), 1000, 125);
    multicastDnsInit();
  }else{
    serverMode = true;
    accessPointInit();
    captiveDnsInit();
  }
  serverInit();
  connectOTA();
}

void loop() {
  serverUpdate();
  OTAUpdate();
  if(serverMode){
    captiveDnsUpdate();
  }else{
    multicastDnsUpdate();
  }
  commands_loop();
}
