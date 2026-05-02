/*filename:-Cal_Const.c*/
#include <stdio.h>
#include "Cal_Const.h"

const unsigned long K_Break_Timeout =  500u; /*( X 10ms)sec sample*/
const BlinkRequest_T blink_ModeChange = {1u,   100u/SAMPLE_PER,  250u/SAMPLE_PER}; // blinks, on and off
const BlinkRequest_T blink_Store =      {1u,  2000u/SAMPLE_PER,  100u/SAMPLE_PER}; // blinks, on and off
const BlinkRequest_T blink_PwnOnOff =   {5u,   250u/SAMPLE_PER,  250u/SAMPLE_PER}; // blinks, on and off
const unsigned char K_Tda_Bass_Reg[15] = {0x00u,0x01u,0x02u,0x03u,0x04u,0x05u,0x06u,0x07u,0x0Eu,0x0Du,0x0Cu,0x0Bu,0x0Au,0x09u,0x08u};
const unsigned char K_Tda_Treble_Reg[15] = {0x00u,0x01u,0x02u,0x03u,0x04u,0x05u,0x06u,0x07u,0x0Eu,0x0Du,0x0Cu,0x0Bu,0x0Au,0x09u,0x08u};
