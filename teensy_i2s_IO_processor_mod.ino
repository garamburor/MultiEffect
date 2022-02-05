#include <Audio.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <SerialFlash.h>
#include "audio_processor.h"

// delete later
const int btnPin = 2;
bool btnState = 0;
int i = 0;
float value = 0.0f;
// --- connections
AudioInputUSB            usb1;
AudioOutputI2S           i2s1; 

AudioProcessor           myProcessor; 

AudioConnection          patchCord1(usb1, 0, myProcessor, 0);
AudioConnection          patchCord2(usb1, 1, myProcessor, 1);
AudioConnection          patchCord3(myProcessor, 0, i2s1, 0);
AudioConnection          patchCord4(myProcessor, 1, i2s1, 1);

void setup() {
  
  AudioMemory(20);
  Serial.begin(9600);
  pinMode(btnPin, INPUT);
  myProcessor.DSPL.update(1);
  myProcessor.DSPR.update(1);
}


void loop() {
  // delete later
  if(Serial.available() && btnState == 0)
  {
    i = (int) Serial.read();
    i = i - 48;
    if(i >= 0)
    {
      myProcessor.changeEffect(i);
      Serial.println(i);
    }
  }
  value = mapi(analogRead(A0), 0.0f, 1023.0f, 0.0f, 1.0f);
  myProcessor.setParam(1,value);
 
}
float mapi(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
