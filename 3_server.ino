ESP8266WebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;


void serverInit() {
  SPIFFS.begin();                                       // Start file system
  server.on("/", handleRoot);                           // Which routine to handle at root location. This is display page
  server.on("/getSSID", getSSID);                       // Asking for actual network SSID
  server.on("/cloudReboot", cloudReboot);                 // Reboot the ball
  server.on("/scanNetwork", scanNetwork);               // Asking for available networks
  server.on("/setNetwork", HTTP_POST, setNetwork);      // Setting WiFi network parameters
  server.on("/randomblink", randomblink);
  server.on("/snake", snakeReceive);
  server.on("/simple", simple);
  
  server.onNotFound([]() {                              // If the client requests any URI
    if (serverMode) {
      if (!handleFileRead(server.uri())) {
        const char *metaRefreshStr = "<head><meta http-equiv=\"refresh\" content=\"0; url=http://index.html\" /></head><body><p>redirecting...</p></body>";
        server.send(200, "text/html", metaRefreshStr);
      }
    } else {
      if (!handleFileRead(server.uri()))                  // send it if it exists
        server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    }
  });
  server.begin();
  
  serverMode = true;
}

void handleRoot() {
  if (!handleFileRead("/"))                             // send it if it exists
    server.send(404, "text/plain", "404: Not Found");   // otherwise, respond with a 404 (Not Found) error
}

String getContentType(String filename) {
  if (filename.endsWith(".html"))      return "text/html";
  else if (filename.endsWith(".htm"))  return "text/html";
  else if (filename.endsWith(".css"))  return "text/css";
  else if (filename.endsWith(".js"))   return "application/javascript";
  else if (filename.endsWith(".png"))  return "image/png";
  else if (filename.endsWith(".svg"))  return "image/svg+xml";
  else if (filename.endsWith(".gif"))  return "image/gif";
  else if (filename.endsWith(".jpg"))  return "image/jpeg";
  else if (filename.endsWith(".ico"))  return "image/x-icon";
  else if (filename.endsWith(".xml"))  return "text/xml";
  else if (filename.endsWith(".pdf"))  return "application/x-pdf";
  else if (filename.endsWith(".zip"))  return "application/x-zip";
  else if (filename.endsWith(".gz"))   return "application/x-gzip";
  return "text/plain";
}

void getSSID() {
  String answer = String(wset.ssid);
  server.send(200, "text/plain", answer);
}

void scanNetwork() {
  String options;
  int n = WiFi.scanNetworks();
  if (n == 0)
    options += "<option disabled selected>No network found...</option>";
  else {
    for (int i = 0; i < n; ++i)
      options += "<option>" + WiFi.SSID(i) + "</option>";
  }
  server.send(200, "text/plain", options);
}

void setNetwork() {
  server.arg("ssid").toCharArray(wset.ssid, SSID_LEN);
  server.arg("pwd").toCharArray(wset.password, PWD_LEN);
  server.send(200);
  saveWifiSettings();
}

void cloudReboot(){
  server.send(200);
  espReboot();
}

void serverUpdate() {
  server.handleClient();
}

void OTAInit() {
  ArduinoOTA.setHostname("NUAGE");
  ArduinoOTA.begin();
}

void OTAUpdate() {
  ArduinoOTA.handle();
}

bool handleFileRead(String path) {
  if (path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void saveWifiSettings() {
  EEPROM.begin(512);
  EEPROM.put(WS_ADDR, wset);
  EEPROM.end();
}

void captiveDnsInit(){
  dnsServer.start(DNS_PORT, "*", apIP);
}

void captiveDnsUpdate() {
  dnsServer.processNextRequest();
}

void serverSend(String s){
  server.send(200, "text/html", s);
}
// ---------------------------------------------------------------------------------------------------
void simple(){
  int r,b,g;
  if(server.hasArg("r")) r = server.arg("r").toInt();
  if(server.hasArg("g")) g = server.arg("g").toInt();
  if(server.hasArg("b")) b = server.arg("b").toInt();

  simple_d.color = RgbColor(r,g,b);
  effect = SIMPLE;
  server.send(200);
}
void randomblink(){
  int lum = server.arg("v").toInt();
  effect = RANDBLINK;
  setup_randblink(lum);
  server.send(200);
}

void snakeReceive(){
  
  snake_d.startcol = RgbColor(server.arg("r0").toInt(),server.arg("g0").toInt(),server.arg("b0").toInt());
  snake_d.endcol = RgbColor(server.arg("r1").toInt(),server.arg("g1").toInt(),server.arg("b1").toInt());
  snake_d.currentcol = snake_d.startcol;
  setup_snake();
  effect = SNAKE;
  server.send(200);
}
