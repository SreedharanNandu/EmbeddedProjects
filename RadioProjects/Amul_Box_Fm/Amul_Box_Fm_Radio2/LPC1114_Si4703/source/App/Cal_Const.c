#include <stdio.h>
#include "Cal_Const.h"

const unsigned long K_Break_Timeout =  500u; /*( X 10ms)sec sample*/
const BlinkRequest_T blink_ModeChange = {1u,   100u/SAMPLE_PER,  250u/SAMPLE_PER}; // blinks, on and off
const BlinkRequest_T blink_Store =      {1u,  2000u/SAMPLE_PER,  100u/SAMPLE_PER}; // blinks, on and off
const BlinkRequest_T blink_PwnOnOff =   {5u,   250u/SAMPLE_PER,  250u/SAMPLE_PER}; // blinks, on and off

