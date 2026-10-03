/*
 * @brief FreeRTOS Blinky example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
xQueueHandle xQueue;
xSemaphoreHandle xBinarySemaphore;


/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void)
{
   SystemCoreClockUpdate();
   Board_Init();
}


void TIMER32_0_IRQHandler(void)//every 5sec timeout kept in pre compiled library
{
   #if SEMAPHORE_EXAMPLE
   signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
   #endif

   Board_TIMER32_Clear_Interrupt();

   #if SEMAPHORE_EXAMPLE
   /* Give the semaphore from ISR context -- never call xSemaphoreGive() directly in an ISR */
   xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
   portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
   #endif
}

/* LED1 toggle thread */
static void vTask1(void *pvParameters) 
{
   #if MESSAGE_Q_EXAMPLE
   int32_t lValueToSend1 = 0xAABBCCDDu;
   int32_t lValueToSend2 = 0x11223344u;
   volatile signed portBASE_TYPE xStatus;
   #endif
   static portTickType prevWakeupTime;
   static bool LedState;
   
   prevWakeupTime = xTaskGetTickCount();
   
   while (1) 
   {
      #if MESSAGE_Q_EXAMPLE
      xStatus = xQueueSendToBack( xQueue, &lValueToSend1, 0 );
      xStatus = xQueueSendToBack( xQueue, &lValueToSend2, 0 );
      #endif

      LedState = (bool) !LedState;
      Board_LED_Set(0, LedState);

      #if TASK_DELAY_EXAMPLE
      vTaskDelay(configTICK_RATE_HZ*2);//2sec
      #else
      vTaskDelayUntil(&prevWakeupTime,configTICK_RATE_HZ);//1sec
      #endif
   }
}

/* LED2 toggle thread */
static void vTask2(void *pvParameters) 
{
   #if MESSAGE_Q_EXAMPLE
   int32_t lReceivedValue;
   volatile signed portBASE_TYPE xStatus;
   const portTickType xTicksToWait = configTICK_RATE_HZ/10;//100ms
   #endif
   #if SEMAPHORE_EXAMPLE
   static bool LedState;
   #endif
   while (1) 
   {
      #if MESSAGE_Q_EXAMPLE
      uxQueueMessagesWaiting( xQueue );
      /*
       when we specify xTicksToWait,  it pulls the data out of Q until Q is empty and then waits every ticks you specify and then it wakes up and rechecks buffer 
       when we specify portMAX_DELAY, then xQueueReceive will block this Q in TaskWaitingToReceive list, 
       when xQueueSendToBack gets called it will check the TaskWaitingToReceive and immediately put in ReadList to run 
       */
      xStatus = xQueueReceive( xQueue, &lReceivedValue, portMAX_DELAY /*OR xTicksToWait*/ );
      #endif

      #if SEMAPHORE_EXAMPLE
      if(xSemaphoreTake( xBinarySemaphore, 500u/*portMAX_DELAY*/)== pdPASS)
      {
         LedState = (bool) !LedState;
         Board_LED_Set(0, LedState);
      }
      #endif
   }
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief   main routine for FreeRTOS blinky example
 * @return   Nothing, function should not exit
 */
int main(void)
{
   prvSetupHardware();

   #if MESSAGE_Q_EXAMPLE
   xQueue = xQueueCreate( 2, sizeof( int32_t ) );//inside creats malloc for the QUEUE structure 76bytes and 21bytes (1 * int32_t + 1byte)
   #endif   

   #if SEMAPHORE_EXAMPLE
   /* Before a semaphore is used it must be explicitly created. In this example a binary semaphore is created. */
   vSemaphoreCreateBinary(xBinarySemaphore);
   xSemaphoreTake(xBinarySemaphore, 0); 
   #endif

   xTaskCreate(vTask1, (signed char *) "vTask1", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),(xTaskHandle *) NULL);
   xTaskCreate(vTask2, (signed char *) "vTask2",configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),(xTaskHandle *) NULL);

   Board_TIMER32_Init();
   Board_TIMER32_Enable_Interrupt();

   /* Start the scheduler */
   vTaskStartScheduler();

   /* Should never arrive here */
   return 1;
}
