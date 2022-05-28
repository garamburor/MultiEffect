#include "ME_DSP.h"
#include <cmath> 
#include <stdlib.h>

/* -- PROCESSOR -- */

void ME_DSP::update(int value)
{
  effect = value;
  if(effect == 1)
  {
    startVerb();
  }
  reset();
}

void ME_DSP::process(float* sample)
{
  switch (effect)
  {
    case 0:
        break;
    case 1:
        //tremolo(sample);
        reverb(sample);
        break;
    case 2:
        bitcrush(sample);
        break;
    case 3:
        vibrato(sample);
        break;
    case 4:
        delay(sample);
        break;
    case 5:
        clipping(sample, 1.0f - param[0]);
    default:
        break;
  }
}

/* -- DSP EFFECTS -- */

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

void ME_DSP::tremolo(float* sample)
{
        // phase acts as f*n/Fs
        mVar1 += param[1] * 15 * invFs;
        // cal LFO for modulation
        LFO(&mVar2, mVar1, (int)param[2]*3);
        // dry / wet control
        *sample = *sample * (1.0f - param[0] + param[0] * mVar2);
        // reset phi
        if (mVar1 >= 1.0f)
        {
            mVar1 = 0.0f;
        }
}

void ME_DSP::delay(float* sample)
{
  mVar3 = param[1] * (BUFF_SIZE - 1); // calculate delay
  mVar4 = positive_modulo(*readIndex - mVar3, BUFF_SIZE); // set read index
  temp = *sample + param[2] * *(buffer + mVar4); // delay effect
  *sample = *sample * (1.0f - param[0]) + temp * param[0];
  clipping(sample, 0.8f);
  *(buffer + *readIndex) = *sample; // feedback
}

void ME_DSP::reverb(float* sample)
{
  // input
  IIRfilter(sample, bandwidth, &mPrev[0]);
  allpass1(sample, inputDiffusion1, &node13_14[0], 210);
  allpass1(sample, inputDiffusion1, &node19_20[0], 159);
  allpass1(sample, inputDiffusion2, &node15_16[0], 562);
  allpass1(sample, inputDiffusion2, &node21_22[0], 410);
  
  // first tank
  tempSample2 = *sample + node59_63[(reverbIndex + 1) % 4687];
  allpass2(&tempSample2, decayDiffusion1, &node23_24[0], 998);
  node24_30[reverbIndex % 6598] = tempSample2;
  IIRfilter(&node24_30[(reverbIndex + 1) % 6598], damping, &mPrev[1]);
  tempSample2 = param[1] * tempSample2;
  allpass1(&tempSample2, decayDiffusion2, &node31_33[0], 2667);
  node33_39[reverbIndex % 5512] = tempSample2;

  // second tank
  tempSample3 = *sample + node33_39[(reverbIndex + 1) % 5512];
  LFO(&mVar2, mVar1, 0);
  allpass2(&tempSample3, decayDiffusion1, &node46_48[0], 1345);
  node48_54[reverbIndex % 6249] = tempSample3;
  IIRfilter(&node48_54[(reverbIndex + 1) % 6249], damping, &mPrev[2]);
  tempSample3 = param[1] * tempSample3;
  allpass1(&tempSample3, decayDiffusion2, &node55_59[0], 3936);
  node59_63[reverbIndex % 4687] = tempSample3;
  
  // left output
  accumulator = 0.6 * node48_54[(reverbIndex + 394) % 6249];
  accumulator += 0.6 * node48_54[(reverbIndex + 4407) % 6249];
  accumulator -= 0.6 * node55_59[(reverbIndex + 2835) % 3936];
  accumulator += 0.6 * node59_63[(reverbIndex + 2958) % 4687];
  accumulator -= 0.6 * node24_30[(reverbIndex + 2949) % 6598];
  accumulator -= 0.6 * node31_33[(reverbIndex + 277) % 2667];
  accumulator -= 0.6 * node33_39[(reverbIndex + 1580) % 5512];
  
  *sample = *sample * (1.0f - param[0]) + param[0] * accumulator;
  
  reverbIndex++;
}

void ME_DSP::vibrato(float* sample)
{
  mVar1 += param[1] * 15 * invFs;
  LFO(&mVar2, mVar1, (int)param[2]*3);
  mVar3 = positive_modulo((int) *writeIndex - mVar2 * 150 * param[0], BUFF_SIZE);
  *sample = *(buffer + mVar3);
  *readIndex = mVar3;
}

void ME_DSP::bitcrush(float* sample)
{
    mVar1 = pow(2,map(param[1], 16, 2)-1);
    temp = ceil(mVar1 * *sample) / mVar1;
    *sample = *sample * (1.0f - param[0]) + temp * param[0];
}

/* -- DSP HELPERS -- */

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

void ME_DSP::IIRfilter(float* sample, float gain, float* tap)
{
  *sample = *sample * gain + *tap * (1.0f - gain);
  *tap = *sample;
}

void ME_DSP::allpass1(float* sample, float gain, float* tap, int delay)
{
  *sample = *sample - gain * tap[(reverbIndex + 1) % delay];
  tap[reverbIndex % delay] = *sample;
  *sample = gain * *sample + tap[(reverbIndex + 1) % delay];
}

void ME_DSP::allpass2(float* sample, float gain, float* tap, int delay)
{
  *sample = *sample + gain * tap[(reverbIndex + 1) % delay];
  tap[reverbIndex % delay] = *sample;
  *sample = - gain * *sample + tap[(reverbIndex + 1) % delay];
}

inline int ME_DSP::positive_modulo(int i, int n)
{
  return (i % n + n) % n;
}

/* -- PARAM HELPERS -- */

void ME_DSP::setParam(int numParam, const float _parameter)
{
    param[numParam] = _parameter;
    paramUpdate();
}

inline int ME_DSP::map(float& x, int low, int high)
{
  int out;
  out = (int) low + (high - low) * x;
  return out;
}

/* PROCESSOR HELPER */

void ME_DSP::startVerb()
{  
  // REVERB
  tempSample = 0;
  tempSample2 = 0;
  tempSample3 = 0;
  accumulator = 0;
  EXCURSION = 24;
  decay = 0.98f;
  decayDiffusion1 = 0.700f;
  decayDiffusion2 = 0.500f;
  inputDiffusion1 = 0.750f;
  inputDiffusion2 = 0.625f;
  bandwidth = 0.9995f;
  damping = 0.001f;

  node13_14 = (float*) calloc(210,sizeof(float));
  node19_20 = (float*) calloc(159,sizeof(float));
  node15_16 = (float*) calloc(562,sizeof(float));
  node21_22 = (float*) calloc(410,sizeof(float));
  node23_24 = (float*) calloc(998,sizeof(float));
  node24_30 = (float*) calloc(6598,sizeof(float));
  node31_33 = (float*) calloc(2667,sizeof(float));
  node33_39 = (float*) calloc(5512,sizeof(float));
  node46_48 = (float*) calloc(1345,sizeof(float));
  node48_54 = (float*) calloc(6249,sizeof(float));
  node55_59 = (float*) calloc(3936,sizeof(float));
  node59_63 = (float*) calloc(4687,sizeof(float));
}

void ME_DSP::initialize(float* buff, int* write, int* read)
{
  buffer = buff;
  writeIndex = write;
  readIndex = read;
}

void ME_DSP::reset()
{
  for(i = 0; i < NUM_ENC; i++)
  {
    param[i] = ENC_RESET;
  }
  temp = 0.0f;
  mVar1 = 0.0f;
  mVar2 = 0.0f;
  mVar3 = 0;
  mVar4 = 0;
}

void ME_DSP::paramUpdate() // change parameter values only
{
}
