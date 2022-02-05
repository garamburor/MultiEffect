#include "ME_DSP.h"
void ME_DSP::update(int value)
{
  effect = value;
}
void ME_DSP::process(float* in, float* out)
{
  switch (effect)
  {
  case 1:
      tremolo(in, out);
      break;
  case 2:
      clipping(in, out);
      break;
  default:
      gain(in, out);
      break;
  }
}
void ME_DSP::gain(float* in, float* out)
{
    *out = *in * param1;
}
void ME_DSP::clipping(float* in, float* out)
{
    *out = *in * (27 + *in * *in) / (27 + param1 * 350 * *in * *in);
}
void ME_DSP::tremolo(float* in, float* out)
{
    // phase acts as f*n/Fs
    phi += param2 * 15 * invFs;
    // cal LFO for modulation
    LFO(&mod, phi, (int)param3);
    // dry / wet control
    *out = *in * (1.0f - param1 + param1 * mod);
    // reset phi
    if (phi >= 1.0f)
    {
        phi = 0.0f;
    }
}
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
