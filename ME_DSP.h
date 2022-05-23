#pragma once
#include "AudioHelper.h"

class ME_DSP
{
public:
  void update(int value);
  void process(float* sample, float* buffer, int& writeHead);
  void setParam(int numParam, const float _parameter);
  
private:
  /* PROCESSOR HELPER */
  void reset();
  void paramUpdate();
  
  /* -- EFFECTS -- */
  void tremolo(float* sample);
  void bitcrush(float* sample, float* buffer, int& writeHead);
  void vibrato(float* sample, float* buffer, int& writeHead);
  void delay(float* sample, float* buffer, int& writeHead);
  void clipping(float* sample);
  
  
  /* -- DSP HELPERS -- */
  void LFO(float* out, float &phi, int wave);
  inline int positive_modulo(int i, int n);

 /* -- PARAM HELPERS -- */
  void bound(float* x, int low, int high);
  int map(float& x, int low, int high);
  
  /* -- MEMORY -- */
  int effect = 0;
  float param[NUM_ENC] = { 0.5f };

  float temp = 0.0f;
  float mVar1 = 0.0f;
  float mVar2 = 0.0f;
  int mVar3 = 0;
  int mVar4 = 0;

  unsigned int i = 0;
};
