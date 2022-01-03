


/*for(int i=0; i<count;i++){
    float y = float(i);
    strip.SetPixelColor(i, RgbColor(0,0,lum));
  }*/
  
  /*if(currentMillis - previousMillis >= 1000){
    previousMillis = currentM'rct689illis;

    if (stateblue == true) {
      stateblue = false;
      strip.ClearTo(RgbColor(0,0,lum));
      
    } else {
      stateblue = true;
      strip.ClearTo(RgbColor(lum, 0, 0));
    }
  }*/

  //strip.ClearTo(RgbColor(lum, 0, 0));
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void espReboot() {
  WiFi.softAPdisconnect(true);
  ESP.restart();
}
