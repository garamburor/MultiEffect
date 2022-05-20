#include "ME_DSP.h"
#include <cmath> 

/* -- PROCESSOR -- */

void ME_DSP::update(int value)
{
  effect = value;
}

void ME_DSP::process(float* sample)
{
  switch (effect)
  {
    case 1:
        tremolo(sample);
        break;
    case 2:
        bitcrush(sample);
        break;
    default:
        gain(sample);
        break;
  }
}

/* -- DSP EFFECTS -- */

void ME_DSP::gain(float* sample)
{
    *sample = *sample * param1;
}

void ME_DSP::clipping(float* sample)
{
    float a;
    float b;
    // soft-clipping
    a = sinf(param1 * PI_2);
    b = 2*a / (1 - a);
    mVar1 = (1 + b) * *sample / ( 1 + b * std::abs( *sample ) );
    // dry / wet control
    *sample= *sample * (1.0f - param1 + param1 * mVar1);
}

void ME_DSP::tremolo(float* sample)
{
        // phase acts as f*n/Fs
        phi += param2 * 15 * invFs;
        // cal LFO for modulation
        LFO(&mVar1, phi, (int)param3*3);
        // dry / wet control
        *sample = *sample * (1.0f - param1 + param1 * mVar1);
        // reset phi
        if (phi >= 1.0f)
        {
            phi = 0.0f;
        }
}

void ME_DSP::delay(float* sample)
{
  
}

void ME_DSP::bitcrush(float* sample)
{
    mVar1 = pow(2,map(param1, 16, 2)-1);
    *sample = 0.7 * ceil(mVar1 * *sample) / mVar1;
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
                *out = 50.0f * (phi -0.98f);
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
void ME_DSP::setParam(int numParam, const float _parameter)
{
    switch(numParam)
    {
        case 0:
            param1 += _parameter;
            bound(&param1,0,1);
            break;
        case 1:
            param2 += _parameter;
            bound(&param2,0,1);
            break;
        case 2:
            param3 += _parameter;
            bound(&param3,0,1);
            break;
    }
}

/* -- PARAM HELPERS -- */

void ME_DSP::bound(float* x, int low, int high)
{
  if(*x < low)
  {
    *x = low;
  }
  else if(*x > high)
  {
    *x = high;
  }
}

int ME_DSP::map(float& x, int low, int high)
{
  int out;
  out = (int) low + (high - low) * x;
  return out;
}

/* PROCESSOR HELPER */
void ME_DSP::reset()
{
  
}
