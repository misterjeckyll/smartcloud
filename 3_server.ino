ESP8266WebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;

void FSinit(){
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
  }else{
    Serial.println("FileSystem mounted");
  }
}
void serverInit() {
  server.on("/", handleRoot);                           // Which routine to handle at root location. This is display page
  server.on("/getSSID", getSSID);                       // Asking for actual network SSID
  server.on("/cloudReboot", cloudReboot);                 // Reboot the ball
  server.on("/power", power);
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

void getSSID() {
  String answer = String(wset.ssid);
  server.send(200, "text/plain", answer);
}

void scanNetwork() {
  String options;
  
  int n = WiFi.scanNetworks();
  String detected[n];
  if (n == 0)
    options += "<option disabled selected>No network found...</option>";
  else {
    for (int i = 0; i < n; ++i){
      String found = WiFi.SSID(i);
      bool contain = false;
      for(int j = 0; j<n; ++j){
        if(found == detected[j]){
          contain = true;
          break;
        }
      }
      if(contain)continue;
      detected[i] = found;
      options += "<option value='"+ found +"'>" + found + "</option>";
    }
      
  }
  server.send(200, "text/plain", options);
}
void power(){
  int power = server.arg("state").toInt();
  state = (STATE) power;
  server.send(200);
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

void OTAUpdate() {
  ArduinoOTA.handle();
}

bool handleFileRead(String path) {
  
  if (path.endsWith("/")) path += "index.html";
  
  String contentType = mime::getContentType(path);
  String pathWithGz = path + ".gz";
  Serial.print(path);
  Serial.print("        ");
  Serial.print(contentType);
  if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) {
    if(LittleFS.exists(pathWithGz)) path+=".gz";
    File file = LittleFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    Serial.println();
    return true;
  }else{
    Serial.print("        ");
    Serial.println("Not found");
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
