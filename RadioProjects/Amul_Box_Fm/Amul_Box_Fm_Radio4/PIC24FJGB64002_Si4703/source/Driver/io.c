
#include "hardware.h"
#include "io.h"
#include "timer.h"

void Init_Ports(void)
{
    //all digital I/O's
   _PCFG = 0xFFFFu;

   _RTSECSEL = 3u;
   
   //MCLR (pin1)
   _TRISA0 = 1u;//enc2 A
   _TRISA1 = 1u;//enc2 B
   //GED1 (pin4)
   //GEC1 (pin5)
   _TRISB2 = 0u;//TX RP2 RB2(pin6)
   _TRISB3 = 1u;//enc2 btn RP3 RB3(pin7)
   //VSS  (pin8) 8Mhz
   //XTAL (pin9) 8Mhz
   //XTAL (pin10)
   _TRISB4 = 1u;//GPIO (pin11)RB4 enc1 A
   _TRISA4 = 1u;//GPIO (pin12)RA4 enc1 btn
   //VDD  (pin13)
   _TRISB5 = 1u;//GPIO (pin14)enc1 B
   //VBUS (pin15)
   _TRISB7 = 1u;//RX (pin16)
   _TRISB8 = 0u;//SCL1//SCL1 RB8(pin17)
   _TRISB9 = 0u;//SDA1//SDA1 RB9(pin18)
   //DREG (pin19)
   //VCAP (pin20)
   //GPIO (pin21)D+
   //GPIO (pin22)D-
   //VUSB (pin23)
   _RP13R = 18u;//MUTE RB13(pin24)
   _TRISB13 = 0u;
   _TRISB14 = 0u;//GPIO (pin25) RSEN
   _TRISB15 = 0u;//GPIO (pin26)	RST
   //VSS  (pin27)
   //VDD  (pin28)

   _RP2R = 5u;// UART TXD (pin6)
   InsertDelay(100);
   _U2RXR = 7u;// UART RXD (pin16)


   _RB8 = 1;
   InsertDelay(100u);
   _RB9 = 1; 
   InsertDelay(100u);
   _RB14 = 1;
   InsertDelay(100u);
   _RB15 = 1;
   InsertDelay(100u);
}


//EOF

