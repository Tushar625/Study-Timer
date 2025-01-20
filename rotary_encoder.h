// this file provides an object for easy handling of rotary encoder

#ifndef ROTARY_ENCODER_HEADER
#define ROTARY_ENCODER_HEADER

#include<RotaryEncoder.h>

#define DT 7
#define CLK 8

RotaryEncoder encoder(CLK, DT, RotaryEncoder::LatchMode::TWO03);

#endif