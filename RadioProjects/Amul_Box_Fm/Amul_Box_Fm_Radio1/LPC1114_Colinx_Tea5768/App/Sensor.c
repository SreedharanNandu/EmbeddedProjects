/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"         /* LPC11xx Peripheral Registers */
#include "uart.h"
#include "type.h"
#include "i2c.h"
#include "system.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LM75_ADDR   0x90

#define TEMPERATURE_REG_ADDR   0x00000000 /* Temperature Register, Read-only */
#define CONFIGURATION_REG_ADDR   0x00000001 /* Configuration Register,Read-write,set operating modes */
#define TOS_SET_POINT         0x00000011 /* Tos Set Point Register,Read-write */
#define THYST_SET_POINT         0x00000010 /* Thyst Set Point Register,Read-write */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/ 

state_t sensor_state;
unsigned char Temp_Bytes[7];

static void I2CReadTemSensor(unsigned char *ptr);

/**
  * @brief   I2CReadTemSensor program
  * @param  DeviceAddr  - Hardware address
  * @param  RegAddr     - Register address
  * @param  ReadSize    - Read Size (Byte)
  * @retval None
  */
static void I2CReadTemSensor(unsigned char *ptr)
{
   unsigned short int data = 0xFFFF;
   unsigned short int length = 7;
   
   //set the pointed to 0 to read temp register first
   I2CStart();
   //-- Set ADDRESS
   Send_I2C(LM75_ADDR);
   while(LPC_I2C->STAT != 0x18);   
   Send_I2C(0);
   while(LPC_I2C->STAT != 0x28);
   I2CStop();
   
   //start reading entire reg set
   I2CStart();
   Send_I2C(LM75_ADDR | 0x01);
   if(LPC_I2C->STAT != 0x40)
   {
      I2CStop();
      return ;
   }
   LPC_I2C->CONCLR = I2C_FLAG_SI;
   LPC_I2C->CONSET |= I2C_FLAG_AA;
   for(;;)
   {
      while(!(LPC_I2C->CONSET & I2C_FLAG_SI));  //-- End Data from slave;
      *ptr++ = (unsigned char)LPC_I2C->DAT;
      length--;
      if(!length)
      {
         break;
      }
      else if(length == 1)
      {
       
         LPC_I2C->CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
      }
      else
      {
         LPC_I2C->CONCLR = I2C_FLAG_SI;
         LPC_I2C->CONSET |= I2C_FLAG_AA;
      }
   }
   I2CStop();
   
}


/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
void Process_Sensor(void)
{
   switch(sensor_state)
   {
    case INIT:
             sensor_state = UPDATE;
             break;
    case UPDATE:
             if(!disp_freq_once)//block if radio data is being processed
             {
                I2CReadTemSensor((unsigned char*)&Temp_Bytes[0]);//9 bit ADC sensor with 7 bytes (Read and Write combined)
                sensor_state = IDLE;
                Start_Timer(TMR1,20000u);
                //Temp is ( MSB with only 7bits | LSB ) bcoz MSBbit is sign, then shift 8 bits to right for 9 bit ADC converted result
                UART_Sprintf("\n\rPCB Temp = %d°C",((((Temp_Bytes[0]&0x7Fu)<<8)|Temp_Bytes[1])>>8));
             }
             break;
    case IDLE:
             if(Get_Timer_Status(TMR1) == TMR_EXPIRED)
             {
                sensor_state = UPDATE;
             }
             break;
    default:
             break;

   }
}

