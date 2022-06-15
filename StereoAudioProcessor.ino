#include <Audio.h>
#include <SPI.h>
#include <Wire.h>
#include <SerialFlash.h>
#include "audio_processor.h" // Include custom audio processor
#include <MIDI.h>

// Audio Objects
AudioControlSGTL5000     sgtl5000_1;
AudioInputI2S         usb1;
AudioOutputI2S           i2s1; 
AudioProcessor           myProcessor; 

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // Create MIDI communication

// UI
uint8_t pos = 0; // Current effect slot
uint8_t fx = 0; // Current effect
float step;

// Connect ADC - > Processor -> DAC
AudioConnection          patchCord1(usb1, 0, myProcessor, 0);
AudioConnection          patchCord2(usb1, 1, myProcessor, 1);
AudioConnection          patchCord3(myProcessor, 0, i2s1, 0);
AudioConnection          patchCord4(myProcessor, 1, i2s1, 1);

void setup() {
  
  // --- audio
  AudioMemory(50);
  
  
  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  //sgtl5000_1.volume(0.6);
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.muteHeadphone();
  sgtl5000_1.unmuteLineout();
  sgtl5000_1.lineInLevel(11);
  sgtl5000_1.lineOutLevel(21);
  // --- serial
  MIDI.setHandleControlChange(processMIDI); // Function to call for midi data
  MIDI.begin(2); // MIDI channel to read data from
  myProcessor.changeEffect(0,1); // Set first effect slot to audio effect 1, which is Tremolo
}

void loop() {
  MIDI.read();
}

void processMIDI(byte channel, byte number, byte value)
{
    step = value * MIDI_TO_FLOAT; // Transform value to float number between 0 and 1
    switch(number)
    {
      case 0: // Change parameter 1
        myProcessor.changeParam(pos,number,step);
        break;
      case 1: // Change parameter 2
        myProcessor.changeParam(pos,number,step);
        break;
      case 2: // Change parameter 3
        myProcessor.changeParam(pos,number,step);
        break;
      case 3: // Change audio effect
        fx = value;
        myProcessor.changeEffect(pos,fx);
        break;
      case 4: // Change effect slot
        pos = value;
        break;
      case 5:
        myProcessor.changeMix(step);
        break;
      case 6:
        myProcessor.changeVolume(step);
        break;
    }
}
