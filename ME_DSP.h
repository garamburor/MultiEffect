#pragma once
#include "AudioHelper.h"

class ME_DSP
{
public:
	void update(int value);
	void process(float* sample);
	void setParam(int numParam, const float _parameter);
  
private:
	// Create effect variables
	int effect = 0;
	float param1 = 1.0f;
	float param2 = 0.8;
	float param3 = 0.0f;
  
	/* -- EFFECTS -- */
	void tremolo(float* sample);
  void delay(float* sample);
	void gain(float* sample);
	void clipping(float* sample);
	void bitcrush(float* sample);
  
	/* -- HELPER EFFECTS -- */
	void LFO(float* out, float &phi, int wave);
	void bound(float* x, int low, int high);
  int map(float& x, int low, int high);
  
	/* -- MEMORY -- */
	unsigned int i = 0;

	// Tremolo
	float mVar1 = 0.0f;
	float phi = 0.0f;

};
