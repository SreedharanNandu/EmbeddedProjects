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
#include "type_sys.h"
#include "App_Led.h"
#include "App_Buttons.h"
#include "gpio.h"
#include "timer16.h"
#include "i2c.h"
#include "uart.h"
#include "adc.h"
#include "system_LPC11xx.h"
#include "App_Audio.h"

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
   ADC_Init();
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
   Init_Buttons();
   Led_Init();
   AudioCtrl_Init();
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
int main(void)
{
   unsigned char infinite = 0u; 

   SystemInit();
   Init_HWIO();
   PowerUpInit_LED();
   Init_App();
   Enable_Timer16(0);
   Enable_Timer16(1);

   while ((infinite == infinite)) 
   {
      if(timer16_1ms_flag)
      {
         timer16_1ms_flag = 0;
      }

      if(timer16_10ms_flag)
      {
         timer16_10ms_flag = 0;
         Scan_Buttons();
         Led_Task();
      }

      if(timer16_100ms_flag)
      {
         timer16_100ms_flag = 0;
      }
      if(timer16_1s_flag)
      {
         timer16_1s_flag = 0;
         LED_Toggle();
         AudioCtrl_Task();
      }
   }
   return 0;
}


