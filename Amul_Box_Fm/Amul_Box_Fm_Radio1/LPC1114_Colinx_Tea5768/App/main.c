/*
pinout of coocox link board of lpc1114

+3.3v
swdio
swclk
swo
nc
reset
nc
gnd
*/
/*################################ Note ##############################################
    Also Note that the J7 row of the lpc1114 coocox board is reversed so be careful
  ###################################################################################*/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"                        /* LPC11xx definitions */
#include "uart.h"
#include "gpio.h"
#include "i2c.h"
#include "system.h"
#include "timer16.h"
#include <string.h>

#undef TEST_PIN

static void Init_HWIO(void);
static void Init_App(void);
static void ClearClkCtrl(void);

#ifdef TEST_PIN
static void Test_Pin(void);
#endif

  
/* Private function prototypes -----------------------------------------------*/   

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
   #if TEST_EE
      unsigned char var = 0xDD;
   #endif
   #if TEST_RADIO
      volatile unsigned char flip=0;
   #endif
   /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
     initialize the PLL and update the SystemFrequency variable. */
   SystemInit();

   Init_HWIO();
   #ifdef TEST_PIN
   Test_Pin();
   #endif
 
   
   Init_Led();

   Init_App();
   Init_Radio();
   Init_Encoder();

   Reset_All_Timers();
   Start_Timer(TMR2,500u);
   Start_Timer(TMR6,100u);

   UART_Printf("\n\r ------------------------------------\n\r");
   UART_Printf("\n\r -------- Test TEA5768 Radio --------\n\r");
   UART_Printf("\n\r ------------------------------------\n\r");

   Audio_Mute(OFF);

   #if TEST_EE
   while(1)
   {
      memset((void*)&K_Radio_Data_Write,var,21);
      memset((void*)&K_Radio_Data_Read,0,5);
      EE_Write(0x00,(unsigned char*)&K_Radio_Data_Write,21);
      EE_Read(0x00,(unsigned char*)&K_Radio_Data_Read,21);
      Delay_Ms(500);
   }
   #endif

   #if TEST_RADIO
   while(1)
   {   
       flip = !flip;
       Radio_Write.data_byte3.bits.swp1=flip;
       Radio_Write.data_byte4.bits.swp2=!flip;
       Write_Radio((unsigned char*)&Radio_Write,5);
       Delay_Ms(500);
       memset((void*)&Radio_Read,0,5);
       Read_Radio((unsigned char*)&Radio_Read,5);
   }
   #endif
   while (1)
   {
      Process_Timer();
      Process_Buttons();
      Process_Encoder();
      Process_Radio();
      Process_Eeprom();
      Process_Sensor();
      //Process_Uart();
      Process_Commands();
    }
}  


static void Init_HWIO(void)
{
   ClearClkCtrl();
   GPIOInit();
   UARTInit(9600u);  
   Init_Timer16(0, TIME_INTERVAL);
   Init_Timer16(1, TIME_INTERVAL);
   I2CInit();    
   Enable_Timer16(0);
   
}

static void ClearClkCtrl(void)
{
   LPC_SYSCON->SYSAHBCLKCTRL &= ~((1<<5)|(1<<6)|(1<<7)|(1<<8)|(1<<9)|(1<<10)|(1<<11)|(1<<12)|(1<<13)|(1<<14)|(1<<15)|(1<<16)|(1<<17));
}

static void Init_App(void)
{
   ee_state = INIT;
   radio_state = INIT;
   sensor_state = INIT;
   serial_state = INIT;



}

#ifdef TEST_PIN
static void Test_Pin(void)
{
   volatile unsigned char toggle=0u;
   volatile unsigned int delayToggle=0u;
    while(1)
   {
      toggle = !toggle;
      GPIOSetValue(1,7,toggle);
      GPIOSetValue(1,6,toggle);
      GPIOSetValue(1,5,toggle);
      delayToggle = 0xFFFFu;
      while(delayToggle--);
   }
}

#endif