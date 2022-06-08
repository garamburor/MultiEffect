#ifndef audio_processor_h_
#define audio_processor_h_

#include "core_pins.h"
#include "AudioStream.h"
#include "ME_DSP.h"
#include <stdlib.h>

#define invBlockSamples 1 / AUDIO_BLOCK_SAMPLES

class AudioProcessor : public AudioStream
{
public:

	AudioProcessor(void) : AudioStream(NUM_CHNLS, inputQueueArray) 
	{  
    for(k = 0; k < fxMax; k++)
    {
      DSPL[k].initialize(&queue[0][0], &head, &test);
      DSPR[k].initialize(&queue[1][0], &head, &test);
    }
  }
	virtual void update(void);
	const int resolutionDAC = 16;
	const int resolutionADC = 16;
	const float conversionConstADC = 1.0f/((1<<resolutionADC)-1);
	const float conversionConstDAC = (1<<resolutionDAC)-1;
  
	// Set a parameter
	void changeParam(int fxPos, int numParam, const float _parameter);
	void changeEffect(int fxPos, int value);
  
private:

  inline void denormalize(float* sample);
  
  audio_block_t *inputQueueArray[NUM_CHNLS];

  // circular buffer
  float queue[NUM_CHNLS][BUFF_SIZE] = {};
  int head = 0;
  int tail = 0;
  int test = 0;
  
	// DSP Objects
	uint8_t fxNum = fxMax;
  ME_DSP DSPL[fxMax]; 
  ME_DSP DSPR[fxMax];
  
	uint8_t channel = 0;
	uint8_t j = 0;
	uint8_t k = 0;

  
};

#endif
