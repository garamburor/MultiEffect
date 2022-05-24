#include "ME_DSP.h"
#include <cmath> 

/* -- PROCESSOR -- */

void ME_DSP::update(int value)
{
  effect = value;
  reset();
}

void ME_DSP::process(float* sample, float* buffer, int& writeHead, int* readHead)
{
  writeIndex = writeHead;
  readIndex = readHead;
  switch (effect)
  {
    case 0:
        break;
    case 1:
        tremolo(sample);
        break;
    case 2:
        bitcrush(sample);
        break;
    case 3:
        vibrato(sample, buffer);
        break;
    case 4:
        delay(sample, buffer);
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

void ME_DSP::delay(float* sample, float* buffer)
{
  mVar3 = param[1] * (BUFF_SIZE - 1); // calculate delay
  mVar4 = positive_modulo(*readIndex - mVar3, BUFF_SIZE); // set read index
  temp = *sample + param[2] * *(buffer + mVar4); // delay effect
  *sample = *sample * (1.0f - param[0]) + temp * param[0];
  clipping(sample, 0.8f);
  *(buffer + *readIndex) = *sample; // feedback
}

void ME_DSP::vibrato(float* sample, float* buffer)
{
  mVar1 += param[1] * 15 * invFs;
  LFO(&mVar2, mVar1, (int)param[2]*3);
  mVar3 = positive_modulo((int) writeIndex - mVar2 * 150 * param[0], BUFF_SIZE);
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

inline void ME_DSP::LFO(float* out, float& phi, int wave)
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
