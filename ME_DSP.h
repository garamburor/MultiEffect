#pragma once
#include "AudioHelper.h"

class ME_DSP
{
public:
  void update(int value);
  void process(float* sample, float* buffer, int& writeHead, int* readHead);
  void setParam(int numParam, const float _parameter);
  
private:
  /* PROCESSOR HELPER */
  void reset();
  void paramUpdate();
  
  /* -- EFFECTS -- */
  void tremolo(float* sample);
  void bitcrush(float* sample);
  void vibrato(float* sample, float* buffer);
  void delay(float* sample, float* buffer);
  void clipping(float* sample, float thresh);
  
  
  /* -- DSP HELPERS -- */
  inline void LFO(float* out, float &phi, int wave);
  inline int positive_modulo(int i, int n);

 /* -- PARAM HELPERS -- */
  inline int map(float& x, int low, int high);
  
  /* -- MEMORY -- */
  int effect = 0;
  float param[NUM_ENC] = { ENC_RESET };
  int writeIndex;
  int* readIndex;

  float temp = 0.0f;
  float mVar1 = 0.0f;
  float mVar2 = 0.0f;
  int mVar3 = 0;
  int mVar4 = 0;

  unsigned int i = 0;
};
