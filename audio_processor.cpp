#include "audio_processor.h"
#include <math.h>

void AudioProcessor::update(void)
{
  audio_block_t *blockL, *blockR;
  float inL, inR, outL, outR;
  unsigned int i;


  // obtain AUDIO_BLOCK_SAMPLES samples (by default 128)
  blockL = receiveWritable(0);
  if (!blockL) return;
  blockR = receiveWritable(1);
  if (!blockR) return;

  // The data[] is an array of 16 bit integers representing the audio (blockL->data[i] is of uint16_t type)
  // (Note: The data[] array is always 32 bit aligned in memory, so you can fetch pairs of samples 
  //        by type casting the address as a pointer to 32 bit data (uint32_t))

  for (i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
    // read the input signal
    inL = blockL->data[i] * conversionConstADC;
    inR = blockR->data[i] * conversionConstADC;
    
    DSPL.process(&inL, &outL);
    DSPR.process(&inR, &outR);
    //DSPR.process(&inR, &outR);
    // write the output signal
    blockL->data[i] = outL * conversionConstDAC;
    blockR->data[i] = outR * conversionConstDAC;    
  }

  transmit(blockL,0);
  transmit(blockR,1);
  release(blockL);
  release(blockR);
}

void AudioProcessor::setParam(int numParam, const float _parameter)
{
  __disable_irq();
  switch(numParam)
  {
    case 1:
      DSPL.param1 = _parameter;
      DSPR.param1 = _parameter;
      break;
    case 2:
      DSPL.param2 = _parameter;
      DSPR.param1 = _parameter;
      break;
    case 3:
      DSPL.param3 = _parameter;
      DSPR.param1 = _parameter;
      break;
    default:
      break;
  }
  __enable_irq();
}
void AudioProcessor::changeEffect(int value)
{
  __disable_irq();
  DSPL.update(value);
  DSPR.update(value);
  __enable_irq();
}
