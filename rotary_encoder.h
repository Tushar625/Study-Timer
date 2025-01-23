
// this file provides an object for easy handling of rotary encoder

#ifndef ROTARY_ENCODER_HEADER
#define ROTARY_ENCODER_HEADER

#include<RotaryEncoder.h> // version 1.5.3

#define DT 7
#define CLK 8

/*
    notice the first 2 args, "CLK, DT", you can swap tham as,
    "DT, CLK", to change input direction
*/

RotaryEncoder encoder(CLK, DT, RotaryEncoder::LatchMode::TWO03);

#endif