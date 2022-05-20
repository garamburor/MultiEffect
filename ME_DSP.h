#pragma once
#include "AudioHelper.h"

class ME_DSP
{
public:
	void update(int value);
	void process(float* sample);
	void setParam(int numParam, const float _parameter);
  
private:
  /* PROCESSOR HELPER */
  void reset();
  
	/* -- EFFECTS -- */
	void tremolo(float* sample);
  void delay(float* sample);
	void gain(float* sample);
	void clipping(float* sample);
	void bitcrush(float* sample);
  
	/* -- DSP HELPERS -- */
	void LFO(float* out, float &phi, int wave);

 /* -- PARAM HELPERS -- */
	void bound(float* x, int low, int high);
  int map(float& x, int low, int high);
  
	/* -- MEMORY -- */
  int effect = 0;
  float param1 = 0.5f;
  float param2 = 0.5;
  float param3 = 0.5f;
  
	unsigned int i = 0;

	// Tremolo
	float mVar1 = 0.0f;
	float phi = 0.0f;

};
