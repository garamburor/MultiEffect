#pragma once
#include "AudioHelper.h"

class ME_DSP
{
public:
  void initialize(float* buff, int* write, int* read);
  void update(int value);
  void process(float* sample);
  void setParam(int numParam, const float _parameter);
  
private:
  /* PROCESSOR HELPER */
  void reset();
  void paramUpdate();
  
  /* -- EFFECTS -- */
  void tremolo(float* sample);
  void bitcrush(float* sample);
  void vibrato(float* sample);
  void delay(float* sample);
  void reverb(float* sample);
  void clipping(float* sample, float thresh);
  
  
  /* -- DSP HELPERS -- */
  void LFO(float* out, float &phi, int wave);
  void startVerb();
  void endVerb();
  void IIRfilter(float* sample, float gain, float* tap);
  void allpass1(float* sample, float gain, float* tap, int delay);
  void allpass2(float* sample, float gain, float* tap, int delay);
  inline int positive_modulo(int i, int n);

 /* -- PARAM HELPERS -- */
  inline int map(float& x, int low, int high);
  
  /* -- MEMORY -- */
  int effect = 0;
  float param[NUM_ENC] = { ENC_RESET };
  float* buffer;
  int* writeIndex;
  int* readIndex;

  float temp = 0.0f;
  float mVar1 = 0.0f;
  float mVar2 = 0.0f;
  int mVar3 = 0;
  int mVar4 = 0;

  unsigned int i = 0;

  unsigned long reverbIndex = 0;
  // REVERB
  float tempSample = 0;
  float tempSample2 = 0;
  float tempSample3 = 0;
  float mPrev[3] = {};
  float accumulator = 0;
  int EXCURSION = 24;
  float decay = 0.5f;
  float decayDiffusion1 = 0.700f;
  float decayDiffusion2 = 0.500f;
  float inputDiffusion1 = 0.750f;
  float inputDiffusion2 = 0.625f;
  float bandwidth = 0.9995f;
  float damping = 0.001f;

  float* node13_14;
  float* node19_20;
  float* node15_16;
  float* node21_22;
  float* node23_24;
  float* node24_30;
  float* node31_33;
  float* node33_39;
  float* node46_48;
  float* node48_54;
  float* node55_59;
  float* node59_63;
};
