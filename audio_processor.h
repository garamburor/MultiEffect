#ifndef audio_processor_h_
#define audio_processor_h_

#include "AudioStream.h"
#include "ME_DSP.h"

class AudioProcessor : public AudioStream
{
public:
  AudioProcessor(void) : AudioStream(2, inputQueueArray) {  }
  virtual void update(void);
  const int resolutionDAC = 16;
  const int resolutionADC = 16;
  const float conversionConstADC = 1.0f/((1<<resolutionADC)-1);
  const float conversionConstDAC = (1<<resolutionDAC)-1;
  const float invFs = 1 / AUDIO_SAMPLE_RATE_EXACT;

  // Set a parameter
  void setParam(int numParam, const float _parameter);
  void changeEffect(int value);
  
  ME_DSP DSPL;
  ME_DSP DSPR;
private:
  // params
  float depth = 0.9f;
  float freqParameter = 0.5f;
  int type = 0;
  int type2 = 3;
  // DSP - Tremolo

  //ME_DSP DSPR(type, depth, freqParameter);
  //float mod;
  //float phi= 0.0f;
  

  audio_block_t *inputQueueArray[2];
};

#endif
