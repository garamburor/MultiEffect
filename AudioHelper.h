#pragma once
#include <math.h>

// General DSP
#define SEC 1
#define NUM_CHNLS 2
#define twoPI 2 * M_PI
#define PI_2 M_PI / 2
#define Fs 44100.0f
#define invFs 1 / Fs
#define fxMax 8
#define BUFF_SIZE static_cast<int>(120)  //(Fs * SEC)