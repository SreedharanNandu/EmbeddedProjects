/*****************************************************************************
filename:- i2c.c
******************************************************************************/
#include <msp430.h>
#include<msp430g2553.h>
#include "type.h"



/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void InitI2C(unsigned char eeprom_i2c_address)
{
   UCB0CTL1 |= UCSWRST;                      // Enable SW reset
   UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
   UCB0CTL1 = UCSSEL_2  + UCSWRST;           // Use SMCLK, TX mode, keep SW reset
   UCB0BR0 = 155u;                           // fSCL = SMCLK/160 = ~100kHz
   UCB0BR1 = 0u;
   UCB0I2CSA  = eeprom_i2c_address;          
   UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void I2CWriteInit(void)
{
   UCB0CTL1 |= UCTR;                         // UCTR=1 => Transmit Mode (R/W bit = 0)c:\Users\nandu\workspace_ccstheia\msp430g2xx3_uscib0_i2c_04.c\msp430g2xx3_uscib0_i2c_04.c
   IFG2 &= ~UCB0TXIFG;
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void I2CReadInit(void)
{
   UCB0CTL1 &= ~UCTR;                        // UCTR=0 => Receive Mode (R/W bit = 1)
   IFG2 &= ~UCB0RXIFG;
}
