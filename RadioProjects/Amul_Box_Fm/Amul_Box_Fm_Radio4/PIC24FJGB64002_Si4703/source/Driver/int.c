
#include "hardware.h"
#include "int.h"

/*****************************************************************************
* Function   : void Init_Externel_Int1(void)
* Created By : Nandu
* Date       : 
* Overview   : Init the Externel Interrupt 1
* Argument   : None
* Output     : None
* Note       : 
*****************************************************************************/
void Init_Ext_INT0(void)      // Init the remappable Port
{
   _INT0EP=0u;         // INT0 Edge is High , This is test code  
   _INT0IP=3u;            // Interrupt Priority is 3 - Test 
   _INT0IF=0u;            // Clearing the interrupt 
   
}
/*****************************************************************************
* Function   : void Enable_Externel_Int1(void)
* Created By : Nandu
* Date       : 
* Overview   : Enable Externel Interrupt
* Argument   : None
* Output     : None
* Note       : 
*****************************************************************************/
void Enable_Ext_INT0(void)   // Enable Externel Interrupt
{
   _INT0IF=0u;            // Clearing the flag    
   _INT0IE=1u;            // Enbaled the Externel INterrupt
}
/*****************************************************************************
* Function   : void Disable_Externel_Int1(void)
* Created By : Nandu
* Date       : 
* Overview   : Disable Externel Interrupt
* Argument   : None
* Output     : None
* Note       : 
*****************************************************************************/
void Disable_Ext_INT0(void)   // Disable Externel Interrupt
{
   _INT0IF=0u;               
   _INT0EP=0u;         
   _INT0IP=0u;            
   _INT0IE=0u;            
}
//EOF
