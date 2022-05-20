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
  void paramUpdate(int numParam);
  
	/* -- EFFECTS -- */
	void tremolo(float* sample);
  void delay(float* sample, float* buffer, int& writeHead);
	void gain(float* sample);
	void clipping(float* sample);
	void bitcrush(float* sample);
  
	/* -- DSP HELPERS -- */
	void LFO(float* out, float &phi, int wave);
  inline int positive_modulo(int i, int n);

 /* -- PARAM HELPERS -- */
	void bound(float* x, int low, int high);
  int map(float& x, int low, int high);
  
	/* -- MEMORY -- */
  int effect = 10;
  float param1 = 0.5f;
  float param2 = 0.1f;
  float param3 = 0.3f;

	float mVar1 = 0.0f;
	float mVar2 = 0.0f;
  int mVar3 = 0;
  int mVar4 = 0;

};
