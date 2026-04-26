/*****************************************************************************
* File Name   : uart.c
* Created By  : Nandu
* Date        :    
* Overview    : Configuring the UART - PIC 24FJ256GB110
* Target      : Microchip PIC 24xx Family  
* Note        : UART Configuration for Developement board
*****************************************************************************/
#include "hardware.h"
#include "uart.h"
#include <string.h>
#include "timer.h"


volatile unsigned char UARTData[MAX_RX_SIZE];
volatile unsigned char rx_flag ,rx_cnt;
   
/*****************************************************************************
* Function    : void Init_UART(void)
* Created By  : Nandu
* Date        :    
* Overview    : Configuring the UART - PIC 24FJ256GB110
* Argument    : None 
* Output      : None
* Note        :
*****************************************************************************/
void Init_UART(void)
{   
   #if 0
       U1BRG = 103u;//for 9615 baud i.e 4/4*(BRG+1) with BRGH=1
      _ADDEN  = 0u;
      _URXISEL = 1u;
      _UTXBRK =0u;   
      _UTXISEL0 = 1u;  
      _UTXISEL1 = 1u;
      _UTXINV = 0u;     
      _STSEL = 0u;
      _PDSEL = 0u;
      _BRGH  = 1u;
      _RXINV = 0u;
      _ABAUD = 0u;
      _LPBACK = 0u;
      _WAKE = 0u;  
      _UEN = 0u;    
      _RTSMD = 0u; 
      _IREN = 0u;  
      _USIDL = 0u;
      _OERR = 0u; 
      _UARTEN  = 1u;
      _U1RXIF = 0u;
      _UTXEN = 1u;
   #endif
       U2BRG = 103u;//for 9615 baud i.e 4/4*(BRG+1) with BRGH=1
       U2STAbits.ADDEN  = 0u;
       U2STAbits.URXISEL = 0u;
       U2STAbits.UTXBRK =0u;   
       U2STAbits.UTXISEL0 = 1u;  
       U2STAbits.UTXISEL1 = 1u;
       U2STAbits.UTXINV = 0u;     
       U2MODEbits.STSEL = 0u;
       U2MODEbits.PDSEL = 0u;
       U2MODEbits.BRGH  = 1u;
       U2MODEbits.RXINV = 0u;
       U2MODEbits.ABAUD = 0u;
       U2MODEbits.LPBACK = 0u;
       U2MODEbits.WAKE = 0u;  
       U2MODEbits.UEN = 0u;    
       U2MODEbits.RTSMD = 0u; 
       U2MODEbits.IREN = 0u;  
       U2MODEbits.USIDL = 0u;
       U2STAbits.OERR = 0u; 
       U2MODEbits.UARTEN  = 1u;
      _U2RXIF = 0u;
       U2STAbits.UTXEN = 1u;
}     

/*****************************************************************************
* Function    : void Enable_UART_Interrupt(void)
* Created By  : Nandu
* Date        :    
* Overview    : Configuring the UART - PIC 24FJ256GB110
* Argument    : None 
* Output      : None
* Note        :
*****************************************************************************/
void Enable_UART_Interrupt(void)
{   
   #if 0
      _U1RXIF = 0u;
      _U1RXIE = 1u;
   #endif
      _U2RXIF = 0u;
      _U2RXIE = 1u;
}     
/*****************************************************************************
* Function   : void Send_UART_String(char *str)
* Created By : Nandu
* Date       :    
* Overview   : String Send Function UART - PIC 24FJ256GB110
* Argument   : String Addres
* Output     : None
* Note       : Modification required Test Function
*****************************************************************************/
UART_Error Send_UART_String(const char *str,uint8_t str_cnt)
{
   UART_Error error=UART_SUCCESS;
   uint16_t tx_cnt;

   for(tx_cnt=0u;tx_cnt<str_cnt;tx_cnt++)
   {         
      #if 0
      while(!U1STAbits.TRMT);
      U1TXREG=str[tx_cnt];            
      #endif 
      while(!U2STAbits.TRMT);
      U2TXREG=str[tx_cnt];            
   }

   return error; ;

} 

//EOF

