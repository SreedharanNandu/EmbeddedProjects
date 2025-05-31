/******************************************************************************
* File Name          : retarget.c

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"
#include "uart.h" 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/  

/**
  * @brief  Redefine fputc function.
  * @param  ...
  * @retval ...
  */
int fputc(int ch, FILE *f)
{
  /* Loop until the end of transmission */
  while (!(LPC_UART->LSR & LSR_THRE))
  {     
  }
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */  
  LPC_UART->THR = ch;

  return ch;
}


/**
  * @}
  */ 

/**
  * @}
  */ 


