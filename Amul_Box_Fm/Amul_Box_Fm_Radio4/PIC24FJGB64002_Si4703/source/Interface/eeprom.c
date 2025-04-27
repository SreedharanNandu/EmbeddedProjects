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
   DataEEInit();
   dataEEFlags.val = 0;
}
