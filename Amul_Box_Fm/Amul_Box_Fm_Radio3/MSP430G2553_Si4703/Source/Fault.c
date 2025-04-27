#include <stdio.h>
#include <stdint.h>
#include "type.h"
#include <msp430.h>



/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Fault(Fault_Type_T id ,unsigned char status)
{
   if(!status)//at fault
   {
      if(id < FLT_RADIO_NONE)   
      {
         radio_state = RADIO_RESET;
      }
      if((id > FLT_RADIO_NONE) && 
         (id < FLT_EE_NONE))
      {
         ee_state = INIT_EE;
      }
   }

}

