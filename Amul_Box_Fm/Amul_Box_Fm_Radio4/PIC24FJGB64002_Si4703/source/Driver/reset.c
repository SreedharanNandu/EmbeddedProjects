
#include "hardware.h"
#include "reset.h"


/*****************************************************************************
* Function   : RESET_TYPE Read_MCU_RCON_Reg(void)
* Created B y: Nandu
* Date       : 
* Overview   : Monitoring the device reset
* Argument   : None 
* Output     : RESET_TYPE
* Note       : Setting and Clearing the rcon bits will not cause any probelm in MCU
*****************************************************************************/
RESET_t Read_MCU_RCON_Reg(void)
{   
   if(WDTO_RESET)
   {
      WDTO_RESET=0;
      return WDT;
   }
   else if(TRAPR_RESET)
   {
      TRAPR_RESET=0;
      return TRAPR;
   }      
   else if(IOPUWR_RESET)
   {
      IOPUWR_RESET=0;
      return IOPUWR;
   }   
   else if(CM_RESET)
   {
      CM_RESET=0;
      return CM;
   }      
   else if(SWR_RESET)
   {
      SWR_RESET=0;
      return SWR;
   }      

   else if(BOR_RESET)
   {
      BOR_RESET=0;
      return BOR;
   }         
    else if(POR_RESET)
    {
       POR_RESET=0;
       return POR;
    }
    else if(EXTR_RESET)
   {
      EXTR_RESET=0;
      return MCLR;
   }    
    return INVALID ;          
   
}
/*****************************************************************************
* Function   : WAKEUP_FROM Read_Wakeup_Status(void)
* Created By : Nandu
* Date       : 
* Overview   : getting the wake up status
* Argument   : None 
* Output     : WAKEUP_FROM which status
* Note       : 
*****************************************************************************/
WAKEUP_t Read_Wakeup_Status(void)
{
   return IDLE;
}


// EOF
