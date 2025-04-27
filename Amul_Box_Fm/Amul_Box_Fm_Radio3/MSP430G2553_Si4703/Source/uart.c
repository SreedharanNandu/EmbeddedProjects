#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "type.h"
#include <msp430.h>

volatile unsigned char  UARTBuffer[UART_BUFSIZE];
volatile unsigned int UARTCount;
unsigned char UARTData[UART_SIZE];
unsigned char UARTFlag;



/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Init_Uart(unsigned int baud) 
{
    // Configure UART (UCA0)
    UCA0CTL1 |= UCSWRST;        // Put UART in reset
    UCA0CTL1 |= UCSSEL_2;       // Use SMCLK as clock source (16 MHz assumed)
    UCA0BR0 = 162;              // Baud rate 9600 with 16 MHz clock (UCBR = 104)
    UCA0BR1 = 6;                // (UCBR1 = 0)
    UCA0MCTL |= UCBRS_1;        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;       // Release UART from reset

    IE2 |= UCA0RXIE;            // Enable UART RX interrupt
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Send_Uart(unsigned char *ptr , unsigned int size)
{
   while(size != 0u)
   {
      // Wait until the UART is ready for transmission (TX buffer empty)
      while(!(IFG2 & UCA0TXIFG))
      {
          if(exitLoop(38))
          {
             break;
          }
      } // Wait for TX buffer to be ready
      // Send the character
      UCA0TXBUF = *ptr;
      *ptr++;
      size--;
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
   // Read the received character and echo it back
   unsigned char receivedChar = UCA0RXBUF; // Read received data
   UARTBuffer[UARTCount++] = receivedChar;
   if (UARTCount == UART_BUFSIZE)
   {
      if(!UARTFlag)
      { 
         memcpy((void*)&UARTData,(void*)&UARTBuffer,UARTCount);
         UARTFlag = 1;
      }
      UARTCount = 0;      /* buffer overflow */
   }   
   // Clear interrupt flag
   IFG2 &= ~UCA0RXIFG;
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
   // TX interrupt is generated when the TX buffer is empty
   // In this case, no specific action is needed since data is sent automatically.
   // Clear the interrupt flag for TX
   IFG2 &= ~UCA0TXIFG;
}


