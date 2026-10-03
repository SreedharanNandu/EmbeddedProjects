/*****************************************************************************
** File Name    : eeprom.c
** Initial Date :  
** Created by   : 
** Description  : 
** Notes        : 
*****************************************************************************/
#include "eeprom.h"
#include "i2c.h"
#include "types.h"
#include "dee.h"
#include "memory.h"

/*****************************************************************************
* Function   : 
* Created By :    
* Date       :    
* Overview   : 
* Argument   : 
* Output     : 
* Note       :    
*****************************************************************************/
void Init_EEPROM(void)
{
   ee_state=INIT_EE;
   DataEEInit();
   dataEEFlags.val = 0u;
}
