#include "ME_DSP.h"
#include <cmath> 
#include <stdlib.h>

/* PROCESSOR HELPER */

void ME_DSP::setParam(int numParam, const float _parameter)
{
    param[numParam] = _parameter;
}

void ME_DSP::startBuffer(float* bL, int* ind)
{
  buff[0] = bL;
  buff[1] = (bL + BUFF_SIZE);
  index = ind;
}

/* DSP HELPERS */

void ME_DSP::LFO(float* out, float& phi, int wave)
{
    switch (wave)
    {
        case 0: // Sine
            *out = 0.5f + 0.5f * sinf(twoPI * phi);
            break;

        case 1: // Smooth Square
            if (phi < 0.48f)
                *out = 1.0f;
            else if (phi < 0.5f)
                *out = 1.0f - 50.0f * (phi - 0.48f);
            else if (phi < 0.98f)
                *out = 0.0f;
            else
                *out = 50.0f * (phi - 0.98f);
            break;

        case 2: // Triangle
            if (phi < 0.25f)
                *out = 0.5f + 2.0f * phi;
            else if (phi < 0.75f)
                *out = 1.0f - 2.0f * (phi - 0.25f);
            else
                *out = 2.0f * (phi - 0.75f);
            break;

        case 3: // Saw
            if (phi < 0.5f)
                *out = 0.5f - phi;
            else
               *out = 1.5f - phi;
            break;
    }
}

inline float ME_DSP::fmap(float x, float out_min, float out_max) 
{
  return x * (out_max - out_min) + out_min;
}

void ME_DSP::IIRfilter(float* sample, float gain, float* tap)
{
  *sample = *sample * gain + *tap * (1.0f - gain);
  *tap = *sample;
}

void ME_DSP::allpass1(float* sample, float gain, float* tap, int delay, int index)
{
  *sample = *sample - gain * tap[(index + 1) % delay];
  tap[index % delay] = *sample;
  *sample = gain * *sample + tap[(index + 1) % delay];
}

void ME_DSP::allpass2(float* sample, float gain, float* tap, int delay, int index)
{
  *sample = *sample + gain * tap[(index + 1) % delay];
  tap[index % delay] = *sample;
  *sample = - gain * *sample + tap[(index + 1) % delay];
}

void ME_DSP::clipping(float* sample, float thresh)
{
    if (*sample > thresh)
    {
        *sample = 1.0f - expf(-*sample);
    }
    else if(*sample < -thresh)
    {
        *sample = -1.0f + expf(*sample);
    }
}

inline int ME_DSP::positive_modulo(int i, int n)
{
  return (i % n + n) % n;
}

inline float ME_DSP::interpolation(float sampleDelay, int chnl)
{
  eta = sampleDelay - floor(sampleDelay);
  x1 = buff[0][positive_modulo((int) (*index - (int) sampleDelay), BUFF_SIZE)];
  x2 = buff[0][positive_modulo((int) (*index - (int) sampleDelay - 1), BUFF_SIZE)];
  return (1.f - eta) * x1 + eta * x2;
}

/* AUDIO EFFECTS */

void tremolo::process(float* sample)
{
      for(i = 0; i < NUM_CHNLS; i ++)
      {
        // call LFO for modulation
        ME_DSP::LFO(&mod, freqStep, (int) ceil(param[2] * 3));
        // dry / wet control
        sample[i] = sample[i] * (1.0f - ME_DSP::param[0] + ME_DSP::param[0] * mod);
      }
      // phase acts as f*n/Fs
      freqStep += ME_DSP::param[1] * 15.f * invFs;
      // reset phi
      if (freqStep >= 1.0f)
      {
          freqStep = 0.0f;
      }
}

void bitcrush::paramUpdate(int numParam)
{
  switch(numParam)
  {
    case 1:
      bitRes = pow(2,ME_DSP::fmap(ME_DSP::param[1], 16, 2)-1);
      break;
    default:
      break;
  } 
}

void bitcrush::process(float* sample)
{
  for(i = 0; i < NUM_CHNLS; i++)
  {
    wet = ceil(bitRes * sample[i]) / bitRes;
    sample[i] = sample[i] * (1.0f - ME_DSP::param[0]) + wet * ME_DSP::param[0];
  }  
}

reverb::reverb()
{
  tempSample = 0.f;
  tempSample2 = 0.f;
  tempSample3 = 0.f;
  accumulator = 0.f;

  reverbIndex = 0;
  EXCURSION = 24;
  decayDiffusion1 = 0.700f;
  decayDiffusion2 = 0.500f;
  inputDiffusion1 = 0.750f;
  inputDiffusion2 = 0.625f;
  bandwidth = 1.f - exp(- twoPI * 10000.f * invFs);
  damping = exp(- twoPI * 10000.f * invFs); 
  
  node13_14 = (float*) calloc(size[0],sizeof(float));
  node19_20 = (float*) calloc(size[1],sizeof(float));
  node15_16 = (float*) calloc(size[2],sizeof(float));
  node21_22 = (float*) calloc(size[3],sizeof(float));
  node23_24 = (float*) calloc(size[4],sizeof(float));
  node24_30 = (float*) calloc(size[5],sizeof(float));
  node31_33 = (float*) calloc(size[6],sizeof(float));
  node33_39 = (float*) calloc(size[7],sizeof(float));
  node46_48 = (float*) calloc(size[8],sizeof(float));
  node48_54 = (float*) calloc(size[9],sizeof(float));
  node55_59 = (float*) calloc(size[10],sizeof(float));
  node59_63 = (float*) calloc(size[11],sizeof(float));

  paramUpdate(1);
}

reverb::~reverb()
{
  delete node13_14;
  delete node19_20;
  delete node15_16;
  delete node21_22;
  delete node23_24;
  delete node24_30;
  delete node31_33;
  delete node33_39;
  delete node46_48;
  delete node48_54;
  delete node55_59;
  delete node59_63;
}

void reverb::paramUpdate(int numParam)
{
  switch(numParam)
  {
    case 1:
      for(i = 4; i < 12; i++)
      {
        test[i] = (int) (ME_DSP::fmap(ME_DSP::param[1], 2, size[i]));
      }
      break;
    default:
      break;
  } 
}

void reverb::process(float* sample)
{
  // input
  tempSample = (sample[0] + sample[1]) * 0.5f;
  // IIR Filter
  tempSample = tempSample * bandwidth + mPrev[0] * (1.0f - bandwidth);
  ME_DSP::allpass1(&tempSample, inputDiffusion1, &node13_14[0], size[0], reverbIndex);
  ME_DSP::allpass1(&tempSample, inputDiffusion1, &node19_20[0], size[1], reverbIndex);
  ME_DSP::allpass1(&tempSample, inputDiffusion2, &node15_16[0], size[2], reverbIndex);
  ME_DSP::allpass1(&tempSample, inputDiffusion2, &node21_22[0], size[3], reverbIndex);

  // first tank
  tempSample2 = tempSample + node59_63[(reverbIndex + 1) % test[11]];
  ME_DSP::allpass2(&tempSample2, decayDiffusion1, &node23_24[0], test[4], reverbIndex);
  node24_30[reverbIndex % test[5]] = tempSample2;
  // IIR Filter
  tempSample2 = node24_30[(reverbIndex + 1) % test[5]] * (1.0f - damping) + mPrev[1] * damping;
  ME_DSP::allpass1(&tempSample2, decayDiffusion2, &node31_33[0], test[6], reverbIndex);
  tempSample2 = ME_DSP::param[2] * tempSample2;
  node33_39[reverbIndex % test[7]] = tempSample2;

  // second tank
  tempSample3 = tempSample + node33_39[(reverbIndex + 1) % test[7]];
  ME_DSP::allpass2(&tempSample3, decayDiffusion1, &node46_48[0], test[8], reverbIndex);
  node48_54[reverbIndex % test[9]] = tempSample3;
  // IIR Filter
  tempSample3 = node48_54[(reverbIndex + 1) % test[9]] * (1.0f - damping) + mPrev[2] * damping;
  ME_DSP::allpass1(&tempSample3, decayDiffusion2, &node55_59[0], test[10], reverbIndex);
  tempSample3 = ME_DSP::param[2] * tempSample3;
  node59_63[reverbIndex % test[11]] = tempSample3;

  // left output
  accumulator = 0.6 * node48_54[(reverbIndex + 394) % test[9]];
  accumulator += 0.6 * node48_54[(reverbIndex + 4407) % test[9]];
  accumulator -= 0.6 * node55_59[(reverbIndex + 2835) % test[10]];
  accumulator += 0.6 * node59_63[(reverbIndex + 2958) % test[11]];
  accumulator -= 0.6 * node24_30[(reverbIndex + 2949) % test[5]];
  accumulator -= 0.6 * node31_33[(reverbIndex + 277) % test[6]];
  accumulator -= 0.6 * node33_39[(reverbIndex + 1580) % test[7]];

  sample[0] = sample[0] * (1.0f - ME_DSP::param[0]) + ME_DSP::param[0] * accumulator;
  
  // right output
  accumulator = 0.6 * node24_30[(reverbIndex + 523) % test[5]];
  accumulator += 0.6 * node24_30[(reverbIndex + 5374) % test[5]];
  accumulator -= 0.6 * node31_33[(reverbIndex + 1820) % test[6]];
  accumulator += 0.6 * node33_39[(reverbIndex + 3961) % test[7]];
  accumulator -= 0.6 * node48_54[(reverbIndex + 3128) % test[9]];
  accumulator -= 0.6 * node55_59[(reverbIndex + 496) % test[10]];
  accumulator -= 0.6 * node59_63[(reverbIndex + 179) % test[11]];

  sample[1] = sample[1] * (1.0f - ME_DSP::param[0]) + ME_DSP::param[0] * accumulator;

  reverbIndex++;
}


void clip::process(float* sample)
{
  for(i = 0; i < NUM_CHNLS; i++)
  {
    ME_DSP::clipping(&sample[i], (1.0f - ME_DSP::param[0]));
  }
  
}

/* DELAY */

echo::echo()
{
  alpha = 1.f - exp(- twoPI * 4000.f * invFs);
  temp = 0.f;
  paramUpdate(1);
}

void echo::paramUpdate(int numParam)
{
  switch(numParam)
  {
    case 1:
      time = (int) ME_DSP::fmap(ME_DSP::param[1], 1, BUFF_SIZE - 1);
      break;
    default:
      break;
  }
}

void echo::process(float* sample)
{
  for(i = 0; i < NUM_CHNLS; i++)
  {
    sample[i] += ME_DSP::param[2] * ME_DSP::interpolation(time, i);
    ME_DSP::clipping(&sample[i], 0.8f);
    temp = sample[i] * alpha + temp * (1.0f - alpha);
    ME_DSP::buff[i][*ME_DSP::index] = temp;
  }
}

/* VIBRATO */

void vibrato::process(float* sample)
{
  for(i = 0; i < NUM_CHNLS; i++)
  {
    ME_DSP::LFO(&mod, rate,(int) ceil(param[2] * 3));
    mod =  mod * 100.f * ME_DSP::param[0];
    e = mod - floor(mod);
    r1 = ME_DSP::buff[i][ME_DSP::positive_modulo((int) (*ME_DSP::index - (int) mod), BUFF_SIZE)];
    r2 = ME_DSP::buff[i][ME_DSP::positive_modulo((int) (*ME_DSP::index - (int) mod - 1), BUFF_SIZE)];
    sample[i] = (1.f - e) * r1 + e * r2;
  }
  *ME_DSP::index = *ME_DSP::index + (int) mod;
  // phase acts as f*n/Fs
  rate += ME_DSP::param[1] * 6.f * invFs;
  // reset phi
  if (rate >= 1.0f)
  {
      rate = 0.0f;
  }
}

/* PING PONG DELAY */

pingpong::pingpong()
{
  alpha = 1.f - exp(- twoPI * 4000.f * invFs);
  temp = 0.f;
  paramUpdate(1);
}

void pingpong::paramUpdate(int numParam)
{
  switch(numParam)
  {
    case 1:
      time = (int) (ME_DSP::fmap(ME_DSP::param[1], 1, BUFF_SIZE - 1));
      break;
    default:
      break;
  }
}

void pingpong::process(float* sample)
{
  for(i = 0; i < NUM_CHNLS; i++)
  {
    sample[i] += ME_DSP::param[2] * ME_DSP::interpolation(time, (i + 1) % BUFF_SIZE);
    ME_DSP::clipping(&sample[i], 0.8f);
    temp = sample[i] * alpha + temp * (1.0f - alpha);
    ME_DSP::buff[i][*ME_DSP::index] = temp;
  }
}
