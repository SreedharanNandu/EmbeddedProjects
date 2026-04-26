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
#include "buttons.h"

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
   Init_Buttons();
   PreInit_Radio();
   Update_EE_To_RAM();
   Init_Encoder();
   Init_Radio();
   LED_Off(LED_B);
}





