#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;
int flexpin1 = A2;
void setup() {
  Serial.begin(115200);
  Serial.println("DRV test");
  drv.begin();
    
  // Set Real-Time Playback mode
  drv.setMode(DRV2605_MODE_REALTIME);
  //Serial.println("done");
}

void loop() {
   int flexVal1;

  flexVal1 = analogRead(flexpin1);

  String input;
  input = Serial.readStringUntil('\n');
  if(input == "3")
  {
    drv.setRealtimeValue(0x5F);
    delay(200);
    drv.setRealtimeValue(0x00);
  }
  else if(input == "4")
  {
    drv.setRealtimeValue(0x6F);
    delay(200);
    drv.setRealtimeValue(0x00);
  }
  else if(input == "5")
  {
    drv.setRealtimeValue(0x7F);
    delay(200);
    drv.setRealtimeValue(0x00);
  }

  if(flexVal1 < 180) Serial.print(0);
  else Serial.print(1);
  
}
