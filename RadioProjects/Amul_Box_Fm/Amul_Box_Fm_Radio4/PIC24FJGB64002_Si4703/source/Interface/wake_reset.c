/*****************************************************************************
** File Name    : wake_reset.c
** Initial Date :      
** Created by   : Nandu
** Description  : RESET Monitoring+ Battery Monitoring API S
**              :   
** Notes        :
*****************************************************************************/
#include "uart.h"
#include "reset.h"
#include "types.h"
#include "uart.h"
#include "radio.h"


/*****************************************************************************
* Function   : void Get_Reset_Type(void)
* Created By : Nandu
* Date       : 
* Overview   : Monitoring the device reset
* Argument   : None 
* Output     : Printing reset type
* Note       : Right now code is not handling the reset status just printing
*            : to UART, Later we have to take care the reset situvation
*****************************************************************************/
void Get_Reset_Type(void)
{
    uint8_t reset_ret;
    reset_ret=Read_MCU_RCON_Reg();    
    
#if (DBG_MSG == 1)

     
    switch(reset_ret)
    {
       case POR:
       UART_Printf(SEND_POR_RST);
       break;
       
       case MCLR:
       UART_Printf(SEND_MCLR_RST);
       break;
       
       case SWR:
       UART_Printf(SEND_SOFT_RST);
       break;
       
       case WDT:
       UART_Printf(SEND_WDT_RST);
       break;
       
       case BOR:
       UART_Printf(SEND_BOR_RST);
       break;
       
       case CM:
       UART_Printf(SEND_CM_RST);
       break;
       
       case TRAPR:
       UART_Printf(SEND_TRAP_RST);
       break;
       
       case IOPUWR:
       UART_Printf(SEND_ILLOP_RST);
       break;
       
       case INVALID:
       UART_Printf(SEND_INV_RST);
       break;
       
       default:
       break;    
       
    }
#endif    
   
}
/*****************************************************************************
* Function   : void Get_Wakeup_Method(void)
* Created By : Nandu
* Date       : 
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*            : 
*****************************************************************************/
void Get_Wakeup_Method(void)
{
   
}

//EOF
