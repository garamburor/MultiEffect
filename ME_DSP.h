#pragma once
#include "AudioHelper.h"

class ME_DSP
{
public:
  virtual ~ME_DSP() {}
	virtual void process(float* sample) {}
	void setParam(int numParam, const float _parameter);
  virtual void paramUpdate(int numParam) {}
  void startBuffer(float* bL, int* ind);

  /* DSP HELPERS */
	void LFO(float* out, float& phi, int wave);
  inline float fmap(float x, float out_min, float out_max);
  void IIRfilter(float* sample, float gain, float* tap);
  void allpass1(float* sample, float gain, float* tap, int delay, int index);
  void allpass2(float* sample, float gain, float* tap, int delay, int index);
  void clipping(float* sample, float thresh);
  inline int positive_modulo(int i, int n);
  inline float interpolation(float eta, int chnl);

	float param[NUM_ENC] = {0.5f, 0.5f, 0.5f};
  float* buff[2];
  int* index;

private:
  float eta;
  float x1;
  float x2;
};

/* AUDIO EFFECTS */

/* TREMOLO */

class tremolo : public ME_DSP
{
public:
	tremolo()
	{
    freqStep = 0.f;
    mod = 0.f;
	}
 
  void process(float* sample);
  
private:
  unsigned int i;
  float freqStep;
  float mod;
};

/* BITCRUSH */

class bitcrush : public ME_DSP
{
public:
  bitcrush()
  {
    paramUpdate(1);
    wet = 0.f;
  }
  
  void process(float* sample);
  
   
private:
  void paramUpdate(int numParam);

  unsigned int i;
  float bitRes;
  float wet;
};

/* REVERB */

class reverb : public ME_DSP
{
public:
  reverb();
  ~reverb();
 
  void process(float* sample);
  
private:
  void paramUpdate(int numParam);
  unsigned int i;
  unsigned int reverbIndex;
  float tempSample;
  float tempSample2;
  float tempSample3;
  float mPrev[3] = {};
  float accumulator;
  
  int EXCURSION;
  float decay;
  float decayDiffusion1;
  float decayDiffusion2;
  float inputDiffusion1;
  float inputDiffusion2;
  float bandwidth;
  float damping;
  
  int size[12] = {210, 159, 562, 410, 998, 6598, 2667, 5512, 1345, 6249, 3936, 4687};
  int test[12];
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

/* CLIPPING */

class clip : public ME_DSP
{
public:
  
  void process(float* sample);
private:
  unsigned int i;
};

/* DELAY */

class echo : public ME_DSP
{
public:
  echo();
  ~echo(){}
  
  void process(float* sample);
  
   
private:
  void paramUpdate(int numParam);
  int i;
  int time;
  float temp;
  float filter;
  float alpha;
};

/* VIBRATO */

class vibrato : public ME_DSP
{
public:
  vibrato()
  {
    rate = 0.f;
    mod = 0.f;
  }
  ~vibrato(){}
  
  void process(float* sample);
  
private:
  float rate;
  float mod;
  float r1;
  float r2;
  float e;
  int i;
};

/* PING PONG DELAY */

class pingpong : public ME_DSP
{
public:
  pingpong();
  ~pingpong(){}
  
  void process(float* sample);
  
   
private:
  void paramUpdate(int numParam);
  int i;
  int time;
  float temp;
  float alpha;
};
