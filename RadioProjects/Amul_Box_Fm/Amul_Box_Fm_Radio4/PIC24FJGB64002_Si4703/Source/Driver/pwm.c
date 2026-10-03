/*****************************************************************************
* Function   : pwm.c
* Created By : Nandu
* Date       : 
* Overview   : PWM Driver 
*****************************************************************************/
#include "hardware.h"
#include "pwm.h"

/*****************************************************************************
* Function   : void Init_PWM1(void)
* Created By : Nandu
* Date       : 
* Overview   : PWM Initilalisation
* Argument   : None 
* Output     : None
* Note       : 
*****************************************************************************/
void Init_PWM1(void)
{
   OC1CON1 = 0u;
   OC1CON1bits.OCTSEL = 0x07u; 
   OC1R = 200u; 
   OC1RS = 200u; 
   PR3 = 500u; 
   OC1CON2bits.SYNCSEL = 0b01101;    // Timer 3
   OC1CON1bits.OCM = 5u;          // Continous Mode 
   T3CONbits.TON = 0u;           // Turn off the Timer 
}
/*****************************************************************************
* Function   : void Set_PWM(uint16_t)
* Created By : Nandu
* Date       : 
* Overview   : PWM Duty Cycle chnging function
* Argument   : None 
* Output     : None
* Note       : PWM Module is Inverted so 80% Duty cycle is equal to 20%
*****************************************************************************/
void Set_PWM_Duty(uint16_t xpwm)
{
   OC1R=xpwm;
}


//EOF

