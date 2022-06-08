#include <Audio.h>
#include <SPI.h>
#include <Wire.h>
#include <SerialFlash.h>
#include "audio_processor.h"
#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// --- connections
AudioControlSGTL5000     sgtl5000_1;
AudioInputI2S         usb1;
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
  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  myProcessor.changeEffect(0,6);
  myProcessor.changeParam(0,1,0.9f);
  myProcessor.changeParam(0,2,0.5f);
  // --- serial
  MIDI.setHandleControlChange(processMIDI);
  MIDI.begin(2);
}


void loop() {
  MIDI.read();
}

void processMIDI(byte channel, byte number, byte value)
{
    step = value * MIDI_TO_FLOAT;
    switch(number)
    {
      case 0:
        myProcessor.changeParam(pos,number,step);
        break;
      case 1:
        myProcessor.changeParam(pos,number,step);
        break;
      case 2:
        myProcessor.changeParam(pos,number,step);
        break;
      case 3:
        fx = value;
        myProcessor.changeEffect(pos,fx);
        break;
      case 4:
        pos = value;
        break;
    }
}
