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

	AudioProcessor(void) : AudioStream(NUM_CHNLS, inputQueueArray) {}
	virtual void update(void);
	const int resolutionDAC = 16;
	const int resolutionADC = 16;
	const float conversionConstADC = 1.0f/((1<<resolutionADC)-1);
	const float conversionConstDAC = (1<<resolutionDAC)-1;
  
	// Set a parameter
	void changeParam(int fxPos, int numParam, const float _parameter);
	void changeEffect(int fxPos, int value);

  // circular buffer
  float queue[NUM_CHNLS][BUFF_SIZE] = {};
  uint32_t head[NUM_CHNLS] = {};
  int tail[NUM_CHNLS] = {};
  
private:
  inline int positive_modulo(int i, int n);

	// Objects
	unsigned int fxNum = fxMax;
	ME_DSP DSP[NUM_CHNLS][fxMax];
  
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int k = 0;
 
	audio_block_t *inputQueueArray[NUM_CHNLS];
};

#endif
