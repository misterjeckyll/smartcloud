


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

void espReboot() {
  ESP.restart();
}
