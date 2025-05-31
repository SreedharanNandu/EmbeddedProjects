/*****************************************************************************
** File Name    : hwio.c
** Initial Date :      
** Created by   : Nandu
** Description  : Interaface for Interrupt
**              : 
** Notes        : 
*****************************************************************************/
#include "int.h"
#include "io.h"
#include "pwm.h"
#include "reset.h"
#include "i2c.h"
#include "timer.h"
#include "uart.h"
#include "int.h"
#include "isr.h"
#include "io_map.h"
#include "wake_reset.h"
#include "led.h"
#include "eeprom.h"
#include "dee.h"


/*****************************************************************************
* Function   : void Init_System(void)
* Created By : Nandu
* Date       : 
* Overview   : Initialising the Peripherals 
* Argument   : None
* Output     :  
* Note       : Init System required lots of clean up   
*****************************************************************************/
void Init_HWIO(void)
{
   Init_Ports();
   Clear_All_Interrupt();
   Disable_Ext_INT0();
   Init_UART();
   Get_Reset_Type();
   Init_Timer1();
   Init_Timer2();
   Turn_Off_Timer2();
   Init_EEPROM();
   Init_PWM1();



}






