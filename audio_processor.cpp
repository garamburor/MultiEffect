#include "audio_processor.h"
#include <math.h>

void AudioProcessor::update(void)
{
    audio_block_t *blockL, *blockR;
    //float sample
    float sampleL, sampleR;
    
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
      
        // read the input signal
        sampleL = blockL->data[j] * conversionConstADC;
        sampleR = blockR->data[j] * conversionConstADC;
        
        // save sample to buffer
        queue[0][head] = sampleL;
        queue[1][head] = sampleR;
        
        test = head;
        // apply fx
        for (k = 0; k < fxNum; k++)
        {
           DSPL[k].process(&sampleL); 
           DSPR[k].process(&sampleR); 
        }
        
        // go to next buffer index
        head = (head + 1) % BUFF_SIZE;
        
        // denormalize
        denormalize(&sampleL);
        denormalize(&sampleR); 
           
        // write output sample
        blockL->data[j] = sampleL * conversionConstDAC;
        blockR->data[j] = sampleR * conversionConstDAC;
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
    DSPL[fxPos].setParam(numParam, _parameter);
    DSPR[fxPos].setParam(numParam, _parameter);
    __enable_irq();
}

void AudioProcessor::changeEffect(int fxPos, int fx)
{
    __disable_irq();
    DSPL[fxPos].update(fx);
    DSPR[fxPos].update(fx);
    __enable_irq();
}
