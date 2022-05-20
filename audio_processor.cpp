#include "audio_processor.h"
#include <math.h>

void AudioProcessor::update(void)
{
    audio_block_t* block;
    //float sample
    float sample;
    
    // The data[] is an array of 16 bit integers representing the audio (blockL->data[i] is of uint16_t type)
    // (Note: The data[] array is always 32 bit aligned in memory, so you can fetch pairs of samples 
    //        by type casting the address as a pointer to 32 bit data (uint32_t))
    for (i = 0; i < NUM_CHNLS; i++)
    {
        // obtain AUDIO_BLOCK_SAMPLES samples (by default 128)
        block = receiveWritable(i);
        if (!block) return;

        // audio processing
        for (j = 0; j < AUDIO_BLOCK_SAMPLES; j++) {
            // read the input signal
            sample = block->data[j] * conversionConstADC;
            for (k = 0; k < fxNum; k++)
            {
               DSP[i][k].process(&sample, &queue[i][0], head[i]); 
            }
            // save sample to buffer
            queue[i][head[i]] = sample;
            //tail[i] = positive_modulo(head[i] - 35000, BUFF_SIZE);
            head[i] = (head[i] + 1) % BUFF_SIZE;
            //sample = queue[i][tail[i]];
            
            // write output sample
            block->data[j] = sample * conversionConstDAC;
        }
        // send buffer to output
        transmit(block, i);
        release(block);
    }
}

void AudioProcessor::changeParam(int fxPos, int numParam, const float _parameter)
{
    __disable_irq();
    DSP[0][fxPos].setParam(numParam, _parameter);
    DSP[1][fxPos].setParam(numParam, _parameter);
    __enable_irq();
}

void AudioProcessor::changeEffect(int fxPos, int fx)
{
    __disable_irq();
    DSP[0][fxPos].update(fx);
    DSP[1][fxPos].update(fx);
    __enable_irq();
}
