/*****************************************************************************
** File Name    : Process.c
** Initial Date :  
** Created by   : 
** Description  : 
** Notes        : 
*****************************************************************************/
#include "types.h"
#include "Process.h"
#include "led.h"
#include "hwio.h"
#include <string.h>
#include "rotary.h"
#include "radio.h"
#include "memory.h"

uint16_t _10msCounter;
uint16_t _100msCounter;
uint16_t _1sCounter;


/*****************************************************************************
* Function   : 
* Created By : 
* Date       : 
* Overview   : 
* Argument   : 
* Output     : 
* Note       :    
*****************************************************************************/
void Init_Process(void)
{
   UART_Printf(SEND_WC); //welcome
   
   Update_EE_To_RAM();
   Init_Encoder();
   Init_Radio();

   //3 times LED+BUZZER indication
   Start_LED(LED_A,BLINK_ON,3,(unsigned long)100,(unsigned long)100,TURN_ON,1);
   Start_LED(LED_B,BLINK_ON,3,(unsigned long)100,(unsigned long)100,TURN_ON,1);

   LED_Off(LED_A);
   LED_Off(LED_B);
}





