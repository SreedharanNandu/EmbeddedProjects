/******************************************************************************
** Project Name         : 
** File Name            : HAL_Init.c
** Description          : This module is used for initialization
** Date                 : 
** Hardware Platform    : 
******************************************************************************/
/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
#include "HAL_Init.h"


/* Set option bytes */
#pragma location = "OPTBYTE"
__root const u8 opbyte0 = 0xEFU;
#pragma location = "OPTBYTE"
__root const u8 opbyte1 = 0x7FU;
#pragma location = "OPTBYTE"
__root const u8 opbyte2 = 0xFEU;
#pragma location = "OPTBYTE"
__root const u8 opbyte3 = 0x04U;

/* Set security ID */
#pragma location = "SECUID"
__root const u8 secuid[10] = 
   {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};

/* Define to include local variables in the header file */
void Init_HW(void)
{

   HAL_SYSCLK_Clock_Init();
   HAL_Port_Init();
   TAU0_Init();
   WDT_Init();

   UARTF1_Init();//not used 
   UARTF0_Init();


   CanInit();

   #if (GMLAN_ENABLED == TRUE)
   App_Trcv_Mode_HVWU_To_Normal();
   #endif

   INTF_VTMR_PowerUp_Init();

   WAKEUPTM_Init();

   RST_NFC(1);     
   NFC_IRQ(1);     
   FIDM_IRQ(1);    

}

