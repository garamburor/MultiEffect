#pragma once
#include <math.h>

class ME_DSP
{
public:

  void update(int pos, int value);
	void process(float* in, float* out);
	float param1 = 0.8f;
  float param2 = 0.5f;
  float param3 = 0.0f;
  
private:

	// Create effect variables
  int numFx = 4;
  int effect = 0;

	// General DSP
	const float twoPI = 2 * M_PI;
	const float Fs = 44100.0f;
	const float invFs = 1 / Fs;
 
  /* -- EFFECTS -- */
  void tremolo(float* in, float* out);
  void gain(float* in, float* out);
  void clipping(float* in, float* out);
  
	/* -- HELPER EFFECTS -- */
	void LFO(float* out, float &phi, int wave);

	// TREMOLO MEMORY
	float mod = 0.0f;
	float phi = 0.0f;
};
