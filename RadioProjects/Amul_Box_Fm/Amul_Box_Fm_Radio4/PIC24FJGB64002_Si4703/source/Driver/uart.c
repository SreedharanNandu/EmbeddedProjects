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
      _ADDEN  = 0;
      _URXISEL = 1;
      _UTXBRK =0;   
      _UTXISEL0 = 1;  
      _UTXISEL1 = 1;
      _UTXINV = 0;     
      _STSEL = 0;
      _PDSEL = 0;
      _BRGH  = 1;
      _RXINV = 0;
      _ABAUD = 0;
      _LPBACK = 0;
      _WAKE = 0;  
      _UEN = 0;    
      _RTSMD = 0; 
      _IREN = 0;  
      _USIDL = 0;
      _OERR = 0; 
      _UARTEN  = 1;
      _U1RXIF = 0;
      _UTXEN = 1;
      _U1RXIE = 1;
   #endif
       U2BRG = 103u;//for 9615 baud i.e 4/4*(BRG+1) with BRGH=1
       U2STAbits.ADDEN  = 0;
       U2STAbits.URXISEL = 0;
       U2STAbits.UTXBRK =0;   
       U2STAbits.UTXISEL0 = 1;  
       U2STAbits.UTXISEL1 = 1;
       U2STAbits.UTXINV = 0;     
       U2MODEbits.STSEL = 0;
       U2MODEbits.PDSEL = 0;
       U2MODEbits.BRGH  = 1;
       U2MODEbits.RXINV = 0;
       U2MODEbits.ABAUD = 0;
       U2MODEbits.LPBACK = 0;
       U2MODEbits.WAKE = 0;  
       U2MODEbits.UEN = 0;    
       U2MODEbits.RTSMD = 0; 
       U2MODEbits.IREN = 0;  
       U2MODEbits.USIDL = 0;
       U2STAbits.OERR = 0; 
       U2MODEbits.UARTEN  = 1;
      _U2RXIF = 0;
       U2STAbits.UTXEN = 1;
      _U2RXIE = 1;
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
   uint16_t tx_cnt=0;

   for(tx_cnt=0;tx_cnt<str_cnt;tx_cnt++)
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

