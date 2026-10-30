
#include "system_def.h"

#include "App_Task.h"
#include "App_Can_Tp.h"
#include "App_Uds.h"
#include "App_Flash.h"
#include "HAL_WDT_API.h"

void Init_APP(void)
{
   CANTP_Init();
   UDS_Init();
   WDT_Restart();
 }



