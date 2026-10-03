/*
 * @brief NXP LPCXpresso 11C24 board file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "retarget.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define TICK_PERIOD_S (5)   /* seconds between ticks */
/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

static ADC_CLOCK_SETUP_T ADCSetup;

/*****************************************************************************
 * Private functions
 ****************************************************************************/
 static void Board_ADC_Init(void);
/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_UART)
   Chip_UART_SendBlocking(DEBUG_UART, &ch, 1);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_UART)
   uint8_t data;

   if (Chip_UART_Read(DEBUG_UART, &data, 1) == 1) {
      return (int) data;
   }
#endif
   return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(char *str)
{
#if defined(DEBUG_UART)
   while (*str != '\0') {
      Board_UARTPutChar(*str++);
   }
#endif
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_UART)
   Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* RXD */
   Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* TXD */

   /* Setup UART for 115.2K8N1 */
   Chip_UART_Init(LPC_USART);
   Chip_UART_SetBaud(LPC_USART, 115200);
   Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
   Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
   Chip_UART_TXEnable(LPC_USART);
#endif
}

/* Initializes board LED(s) */
static void Board_LED_Init(void)
{
   /* Set the PIO_7 as output */
   Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 3);
}

/* Sets the state of a board LED to on or off */
void Board_LED_Set(uint8_t LEDNumber, bool On)
{
   if(LEDNumber == 0) 
   {
      Chip_GPIO_SetPinState(LPC_GPIO, 0, 3, On);
   }
}

/* Returns the current state of a board LED */
bool Board_LED_Test(uint8_t LEDNumber)
{
   return Chip_GPIO_GetPinState(LPC_GPIO, 0, 3);
}

void Board_LED_Toggle(uint8_t LEDNumber)
{
   if (LEDNumber == 0)
   {
      Chip_GPIO_SetPinToggle(LPC_GPIO, 0, 3);
   }
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
   /* Sets up DEBUG UART */
   DEBUGINIT();

   /* Initialize GPIO */
   Chip_GPIO_Init(LPC_GPIO);

   /* Initialize LEDs */
   Board_LED_Init();

   Board_ADC_Init();
}


static void Board_ADC_Init(void)
{
    /* Verify FUNC value against your exact LPC1114 datasheet pin table for
       whichever physical AD channel you've wired -- example uses AD0 on PIO0_11 */
    Chip_IOCON_PinMux(LPC_IOCON, IOCON_PIO0_11, IOCON_MODE_INACT, IOCON_FUNC2);

    Chip_ADC_Init(LPC_ADC, &ADCSetup);
    Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, 1000);   /* slow, easy to observe */
    Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);

}
void Board_ADC_Convert(uint16_t *dataADC)
{
   while (1) 
   {
      /* Start A/D conversion */
      Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

      /* Waiting for A/D conversion complete */
      while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH0, ADC_DR_DONE_STAT) != SET) {}

      /* Read ADC value */
      Chip_ADC_ReadValue(LPC_ADC, ADC_CH0, dataADC);
   }
}


/**
 * @brief   
 * @return   Function should not exit.
 */
void Board_TIMER32_Init(void)
{
   uint32_t timerFreq;

   /* Enable timer 1 clock */
   Chip_TIMER_Init(LPC_TIMER32_0);

   /* Timer rate is system clock rate */
   timerFreq = Chip_Clock_GetSystemClockRate();

   /* Timer setup for match and interrupt at TICKRATE_HZ */
   Chip_TIMER_Reset(LPC_TIMER32_0);
   Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 1);
   Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, (timerFreq * TICK_PERIOD_S) - 1);
   Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_0, 1);
   Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
   Chip_TIMER_Enable(LPC_TIMER32_0);

}

void Board_TIMER32_Enable_Interrupt(void)
{
   /* Enable timer interrupt */
   NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
   NVIC_EnableIRQ(TIMER_32_0_IRQn);
}

void Board_TIMER32_Disable_Interrupt(void)
{
   /* Enable timer interrupt */
   NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
   NVIC_DisableIRQ(TIMER_32_0_IRQn);
}

void Board_TIMER32_Clear_Interrupt(void)
{
   //if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 1)) 
   {
      Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
   }
}






