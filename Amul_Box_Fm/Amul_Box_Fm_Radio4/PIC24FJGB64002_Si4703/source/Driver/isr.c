/*****************************************************************************
* File Name   : isr.c
* Created By  : Nandu
* Date        : 
* Overview    : IHL ISR Handling file 
* Target      : Microchip PIC 24xx Family  
* Note        :  
*****************************************************************************/
#include "hardware.h"
#include "io_map.h"
#include "isr.h"
#include "int.h"
#include "timer.h"
#include "uart.h"
#include "types.h"
#include "process.h"


/*****************************************************************************
* ISR        : U2RXInterrupt
* Created By : Nandu
* Overview   : Reception Interrupt
* Note       : Modification required Test Function
*****************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void) 
{
   unsigned char temp=0u;

   temp = U2RXREG;
   _U2RXIF=0u;

   if((rx_cnt == 0u) && (temp == 0x55u))
   {    
      UARTData[0] = temp;
      rx_cnt = 1u;
   }   
   else if(rx_cnt == 1u)
   {
     if(UARTData[0] == 0x55u)
     {
        UARTData[1] = temp;
        rx_cnt = 2u;
     }
     else
     {
        UARTData[0] = UARTData[1] = 0;
        rx_cnt = 0u;
     }
   }
   else if(rx_cnt == 2u)
   {
      if(temp == 0xAAu)      
      {
         if(UARTData[0] == 0x55u)
         {
            UARTData[2] = temp;
            rx_cnt = 0u;
            rx_flag = 1u;
         }
         else
         {
            rx_flag = 0u;
            UARTData[0]=UARTData[1]=UARTData[2]=0u;
            rx_cnt = 0;
         }
      }
      else
      {
         rx_flag = 0u;
         UARTData[0]=UARTData[1]=UARTData[2]=0u;
         rx_cnt = 0;
      }
   }
   else
   {
   }
}
/*****************************************************************************
* ISR        : SPI TRansmition Interrupt
* Created By : Nandu
* Overview   : Reception Interrupt
* Note       : Modification required Test Function
*****************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _SPI1Interrupt(void) 
{
    _SPI1IF=0;
}
/*****************************************************************************
* ISR        : Ext0 Interrupt
* Created By : Nandu
* Overview   : Externel Interrupt 1 Prority-3 
* Note       : Modification required Test Function
*****************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _INT0Interrupt(void) 
{
   _INT0IF=0;     
}
/*****************************************************************************
* ISR        : Timer 1 Interrupt
* Created By : Nandu
* Overview   : Timer Interrupt with Priority 1-1
* Note       : Modification required Test Function
*****************************************************************************/
void  __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
   _T1IF = 0; // Clear Timer falg 
  _10msCounter++;
  _100msCounter++;
  _1sCounter++;

  //safety
  if(_100msCounter > 400u)
  {
     //means overflow and could be stuck at while loop 
     asm( "RESET ");
  }

}

//EOF
