#include <Audio.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <SerialFlash.h>
#include "audio_processor.h"
#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// --- connections
AudioInputUSB            usb1;
AudioOutputI2S           i2s1; 

AudioProcessor           myProcessor; 

AudioConnection          patchCord1(usb1, 0, myProcessor, 0);
AudioConnection          patchCord2(usb1, 1, myProcessor, 1);
AudioConnection          patchCord3(myProcessor, 0, i2s1, 0);
AudioConnection          patchCord4(myProcessor, 1, i2s1, 1);

// UI
uint8_t pos = 0;
uint8_t fx = 0;
float step;

void setup() {
  
  // --- audio
  AudioMemory(50);
  myProcessor.changeEffect(0,1);
  // --- serial
  MIDI.setHandleControlChange(processMIDI);
  MIDI.begin(2);
}


void loop() {
  MIDI.read();
}

void processMIDI(byte channel, byte number, byte value)
{
    step = (value - 40)*0.02;
    switch(number)
    {
      case 0:
        myProcessor.changeParam(pos,0,step);
        break;
      case 1:
        myProcessor.changeParam(pos,1,step);
        break;
      case 2:
        myProcessor.changeParam(pos,2,step);
        break;
      case 3:
        fx = value;
        myProcessor.changeEffect(pos,fx);
        break;
      case 4:
        pos = value;
        Serial.println(pos);
        break;
    }
}
