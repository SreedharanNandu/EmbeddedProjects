/* Includes ------------------------------------------------------------------*/
#include "LPC11xx.h"
#include "uart.h"
#include "system.h"
#include <string.h>

volatile uint32_t UARTStatus;
volatile uint8_t  UARTTxEmpty = 1;
volatile uint8_t  UARTBuffer[UART_BUFSIZE];
volatile uint32_t UARTCount = 0;

/*****************************************************************************
** Function name:      UART_IRQHandler
**
** Descriptions:      UART interrupt handler
**
** parameters:         None
** Returned value:      None
** 
*****************************************************************************/
void UART_IRQHandler(void)
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;

  IIRValue = LPC_UART->IIR;
    
  IIRValue >>= 1;         /* skip pending bit in IIR */
  IIRValue &= 0x07;         /* check bit 1~3, interrupt identification */
  if (IIRValue == IIR_RLS)      /* Receive Line Status */
  {
    LSRValue = LPC_UART->LSR;
    /* Receive Line Status */
    if (LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXFE | LSR_BI))
    {
      /* There are errors or break interrupt */
      /* Read LSR will clear the interrupt */
      UARTStatus = LSRValue;
      Dummy = LPC_UART->RBR;   /* Dummy read on RX to clear 
                        interrupt, then bail out */
      return;
    }
    if (LSRValue & LSR_RDR)   /* Receive Data Ready */         
    {
      /* If no error on RLS, normal ready, save into the data buffer. */
      /* Note: read RBR will clear the interrupt */
      UARTBuffer[UARTCount++] = LPC_UART->RBR;
      if (UARTCount == UART_BUFSIZE)
      {
        UARTCount = 0;      /* buffer overflow */
      }   
    }
  }
  else if (IIRValue == IIR_RDA)   /* Receive Data Available */
  {
    /* Receive Data Available */
    UARTBuffer[UARTCount++] = LPC_UART->RBR;
    if (UARTCount == UART_BUFSIZE)
    {
       if(!UARTFlag)
       { 
          memcpy((void*)&UARTData,(void*)&UARTBuffer,UARTCount);
          UARTFlag = 1;
       }
       UARTCount = 0;      /* buffer overflow */
    }
  }
  else if (IIRValue == IIR_CTI)   /* Character timeout indicator */
  {
    /* Character Time-out indicator */
    UARTStatus |= 0x100;      /* Bit 9 as the CTI error */
  }
  else if (IIRValue == IIR_THRE)   /* THRE, transmit holding register empty */
  {
    /* THRE interrupt */
    LSRValue = LPC_UART->LSR;      /* Check status in the LSR to see if
                        valid data in U0THR or not */
    if (LSRValue & LSR_THRE)
    {
      UARTTxEmpty = 1;
    }
    else
    {
      UARTTxEmpty = 0;
    }
  }
  return;
}


/*****************************************************************************
** Function name:      UARTInit
**
** Descriptions:      Initialize UART0 port, setup pin select,
**            clock, parity, stop bits, FIFO, etc.
**
** parameters:         UART baudrate
** Returned value:      None
** 
*****************************************************************************/
void UARTInit(uint32_t baudrate)
{
  uint32_t Fdiv;
  uint32_t regVal;

  UARTTxEmpty = 1;
  UARTCount = 0;
  
  NVIC_DisableIRQ(UART_IRQn);

  LPC_IOCON->PIO1_6 &= ~0x07;    /*  UART I/O config */
  LPC_IOCON->PIO1_6 |= 0x01;     /* UART RXD */
  LPC_IOCON->PIO1_7 &= ~0x07;   
  LPC_IOCON->PIO1_7 |= 0x01;     /* UART TXD */

  /* Enable UART clock */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
  LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1 */

  #if 1
	LPC_UART->LCR = 0x83;             /* 8 bits, no Parity, 1 Stop bit */
  regVal = LPC_SYSCON->UARTCLKDIV;
  Fdiv = ((SystemAHBFrequency/regVal)/16)/baudrate ;   /*baud rate */

  LPC_UART->DLM = Fdiv / 256;                     
  LPC_UART->DLL = Fdiv % 256;
  LPC_UART->LCR = 0x03;      /* DLAB = 0 */
  LPC_UART->FCR = 0x07;      /* Enable and reset TX and RX FIFO. */
	#else
	
	
	// Set baud rate to 115200 kb/s @ UART_CLK of 48Mhz  (DLM = 0, DLL = 17, DIVADDVAL = 8, and MULVAL = 15)
	LPC_UART->DLM = 0x00;		// Default
	LPC_UART->DLL = 0x11u;		
	LPC_UART->FDR = 0xF8u;		// FDR = (MULVAL << 4 ) | DIVADDVAL
	
	// Turn off DLAB
	LPC_UART->LCR = 0x83u;
	#endif
	

  /* Read to clear the line status. */
  regVal = LPC_UART->LSR;

  /* Ensure a clean start, no data in either TX or RX FIFO. */
  while ( LPC_UART->LSR & (LSR_THRE|LSR_TEMT) != (LSR_THRE|LSR_TEMT) );
  while ( LPC_UART->LSR & LSR_RDR )
  {
   regVal = LPC_UART->RBR;   /* Dump data from RX FIFO */
  }
 
  /* Enable the UART Interrupt */
  NVIC_EnableIRQ(UART_IRQn);

#if TX_INTERRUPT
  LPC_UART->IER = IER_RBR | IER_THRE | IER_RLS;   /* Enable UART interrupt */
#else
  LPC_UART->IER = IER_RBR | IER_RLS;   /* Enable UART interrupt */
#endif
  return;
}

/*****************************************************************************
** Function name:      UARTSend
**
** Descriptions:      Send a block of data to the UART 0 port based
**            on the data length
**
** parameters:      buffer pointer, and data length
** Returned value:   None
** 
*****************************************************************************/
void UARTSend(uint8_t *BufferPtr, uint32_t Length)
{
  
  while ( Length != 0 )
  {
     /* THRE status, contain valid data */
#if !TX_INTERRUPT
     while ( !(LPC_UART->LSR & LSR_THRE) );
     LPC_UART->THR = *BufferPtr;
#else
     /* Below flag is set inside the interrupt handler when THRE occurs. */
      while ( !(UARTTxEmpty & 0x01) );
     LPC_UART->THR = *BufferPtr;
      UARTTxEmpty = 0;   /* not empty in the THR until it shifts out */
#endif
      BufferPtr++;
      Length--;
  }
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
