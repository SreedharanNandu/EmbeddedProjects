/******************** (C) COPYRIGHT 2010 Embest Info&Tech Co.,LTD. ************
* File Name          : LED_main.c
* Author             : Wuhan R&D Center, Embest
* Date First Issued  : 01/18/2010
* Description        : LED_main program body.
*******************************************************************************
*******************************************************************************
* History:
* 01/18/2010       : V1.0         initial version
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"                        /* LPC11xx definitions */
#include "timer16.h"
#include "clkconfig.h"
#include "gpio.h"
#include "uart.h"
#include "system.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void Init_Led(void) 
{
   Power_Led(TRUE);
   Mode_Led(FALSE);
   GPIOSetValue(2,3,1);
   Stereo_Led(FALSE);
}
/**
  * @}
  */ 

/**
  * @}
  */ 

/************* (C) COPYRIGHT 2010 Wuhan R&D Center, Embest *****END OF FILE****/
