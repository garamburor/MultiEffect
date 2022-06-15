#include "audio_processor.h"
#include <math.h>

void AudioProcessor::update(void)
{
    audio_block_t *blockL, *blockR; // audio blocks in 16 bit integers
    float sample[NUM_CHNLS]; // memory for current samples in floating point
    float drySample[NUM_CHNLS]; // memory for current samples in floating point
    
    // The data[] is an array of 16 bit integers representing the audio (blockL->data[i] is of uint16_t type)
    // (Note: The data[] array is always 32 bit aligned in memory, so you can fetch pairs of samples 
    //        by type casting the address as a pointer to 32 bit data (uint32_t))
    
    // obtain AUDIO_BLOCK_SAMPLES samples (by default 128)
    blockL = receiveWritable(0);
    blockR = receiveWritable(1);
    if (!blockL) return;
    if (!blockR) return;

    // audio processing
    for (j = 0; j < AUDIO_BLOCK_SAMPLES; j++) 
    {
      
        // read the input signal and convert to float
        drySample[0] = blockL->data[j] * conversionConstADC;
        drySample[1] = blockR->data[j] * conversionConstADC;

        sample[0] = drySample[0];
        sample[1] = drySample[1];
        
        // save sample to buffer
        queue[0][head] = sample[0];
        queue[1][head] = sample[1];
        
        readPtr = head;
        // apply fx
        for (k = 0; k < fxMax; k++)
        {
           DSP[k]->process(&sample[0]); 
        }
        
        // go to next buffer index
        head = (head + 1) % BUFF_SIZE;

        // dry / wet
        sample[0] = drySample[0] * (1.f - depth) + sample[0] * depth;
        sample[1] = drySample[1] * (1.f - depth) + sample[1] * depth;

        // set volume
        sample[0] = volume * sample[0];
        sample[1] = volume * sample[1];
        
        // denormalize
        denormalize(&sample[0]);
        denormalize(&sample[1]); 
           
        // write output sample
        blockL->data[j] = sample[0] * conversionConstDAC;
        blockR->data[j] = sample[1] * conversionConstDAC;
    }
    
    // send buffer to output
    transmit(blockL, 0);
    transmit(blockR, 1);
    release(blockL);
    release(blockR);
}

inline void AudioProcessor::denormalize(float* sample)
{
    float absValue = fabs(*sample);
    if (absValue < 1e-15)
    {
        *sample = 0.0f;
    }
}

void AudioProcessor::changeParam(int fxPos, int numParam, const float _parameter)
{
    __disable_irq();
    DSP[fxPos]->setParam(numParam, _parameter);
    DSP[fxPos]->paramUpdate(numParam);
    __enable_irq();
}

void AudioProcessor::changeVolume(float gain)
{
  volume = gain;
}

void AudioProcessor::changeMix(float d)
{
  depth = d;
}

void AudioProcessor::changeEffect(int fxPos, int fx)
{
    __disable_irq();
    
    delete DSP[fxPos];
    switch(fx)
    {
      case 1:
        DSP[fxPos] = new tremolo;
        break;
      case 2:
        DSP[fxPos] = new bitcrush;
        break;
      case 3:
        DSP[fxPos] = new reverb;
        break;
      case 4:
        DSP[fxPos] = new echo;
        DSP[fxPos]->startBuffer(&queue[0][0], &readPtr);
        break;
      case 5:
        DSP[fxPos] = new vibrato;
        DSP[fxPos]->startBuffer(&queue[0][0], &readPtr);
        break;
      case 6:
        DSP[fxPos] = new pingpong;
        DSP[fxPos]->startBuffer(&queue[0][0], &readPtr);
        break;
      default:
        DSP[fxPos] = new ME_DSP;
        break;
    }
    
    __enable_irq();
}
