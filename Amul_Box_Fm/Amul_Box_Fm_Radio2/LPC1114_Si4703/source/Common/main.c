/**
*
* file:- main.c
*/

/*-----------------------------------------------------------------------------
                                     INCLUDES
  -----------------------------------------------------------------------------*/
#include "LPC11xx.h"                        
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "App_Si.h"
#include "type_sys.h"
#include "App_Rotary.h"
#include "App_Led.h"
#include "App_Buttons.h"
#include "gpio.h"
#include "timer16.h"
#include "App_Eeprom.h"
#include "i2c.h"
#include "uart.h"
#include "App_Eeprom.h"


/*-----------------------------------------------------------------------------
                                     MACROS
  -----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
                                     VARIABLES
  -----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                                     PROTOTYPE
  -----------------------------------------------------------------------------*/
void Sys_Init(void);
bool Check_Timer_Expired(void);
void Reset_Timer();
void Port_Init(void);
void I2c_Init(void);



/*-----------------------------------------------------------------------------
                                  FUNC DEFINITION
  -----------------------------------------------------------------------------*/

   

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Init_HWIO(void)
{
   GPIOInit();
   Init_I2C();
   Init_Timer16(0, TIME_INTERVAL);
   Init_Timer16(1, TIME_INTERVAL);
   UARTInit(9600/*19200*/);
}


/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Init_App(void)
{
   Change_Mode_EE(INIT_EE);
   Process_Eeprom();//copy NVM--->shadowRAM
   Init_Encoder();
   Led_Init();
   Init_Radio();

}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
int main(void)
{
   SystemInit();
   Init_HWIO();
   Init_App();
   Enable_Timer16(0);
   Enable_Timer16(1);

   while (1) 
   {
      if(timer16_1ms_flag)
      {
         timer16_1ms_flag = 0;
         Fast_Periodic_Encoder();
      }

      if(timer16_10ms_flag)
      {
         timer16_10ms_flag = 0;
         Scan_Buttons();
         Led_Task();
         Process_App_Si();
      }

      if(timer16_100ms_flag)
      {
         timer16_100ms_flag = 0;
         Slow_Periodic_Encoder();
         Process_Eeprom();
         Process_Commands();
      }
      if(timer16_1s_flag)
      {
         timer16_1s_flag = 0;
      }
   }

   return 0;
}

