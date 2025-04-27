/******************** (C) COPYRIGHT 2010 Embest Info&Tech Co.,LTD. ************
* File Name          : UART_main.c
* Author             : Wuhan R&D Center, Embest
* Date First Issued  : 01/18/2010
* Description        : UART_main program body.
*******************************************************************************
*******************************************************************************
* History:
* 01/18/2010       : V1.0         initial version
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"
#include "uart.h"
#include "system.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile char  UARTData[UART_BUFSIZE];
volatile unsigned char UARTFlag;
char printBuffer[PRINTBUFSIZE];


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Send_Radio_Data(void);

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
state_t serial_state;

void Process_Uart(void) 
{

   switch(serial_state)
   {
    case INIT:
             serial_state = UPDATE;
             break;
    case UPDATE:
             serial_state = IDLE;
             Start_Timer(TMR5,1000u);
           //  Send_Radio_Data();
             break;
    case IDLE:
             if(Get_Timer_Status(TMR5) == TMR_EXPIRED)
             {
                serial_state = UPDATE;
             }
             break;
    default:
             break;

   }
}

void Send_Radio_Data(void)
{
   static unsigned char flip=0;
   unsigned int freq;
   
   freq = Convert_PLL_To_Freq();
   flip = !flip;
   if(flip)
   {
      UART_Sprintf("\n\rFreq : %dMhz", freq);
   }
   else
   {
      UART_Sprintf(" blf value:0x%X", Radio_Read.data_byte1.bits.blf);
      UART_Sprintf(" pll value:0x%X", Radio_Read.data_byte1.bits.pll);
      UART_Sprintf(" rf value:0x%X", Radio_Read.data_byte1.bits.rf);
      UART_Sprintf(" pll value:0x%X", Radio_Read.data_byte2.bits.pll);
      UART_Sprintf(" if value:0x%X", Radio_Read.data_byte3.bits.pll_if);
      UART_Sprintf(" stereo value:0x%X", Radio_Read.data_byte3.bits.stereo);
      UART_Sprintf(" ci value:0x%X", Radio_Read.data_byte4.bits.ci);
      UART_Sprintf(" level value:0x%X \n\r", Radio_Read.data_byte4.bits.lev);
   }
}

void UART_Printf(char *ptr)
{
   UARTSend(ptr, strlen(ptr));
}

