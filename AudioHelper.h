#pragma once
#include <math.h>
// USER IO CONSTANTS
#define NUM_ENC 3
#define ENC_RESET 0.5f
#define MIDI_TO_FLOAT 0.00787401574 // 1 / 127

// General DSP
#define SEC 1
#define NUM_CHNLS 2
#define twoPI 2 * M_PI
#define PI_2 M_PI / 2
#define Fs 44100.0f
#define invFs 1 / Fs
#define fxMax 8
#define BUFF_SIZE 44100
