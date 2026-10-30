
#include "system_def.h"

#include "App_Task.h"
#include "App_Can_Tp.h"
#include "App_Uds.h"
#include "App_Boot.h"

void Init_APP(void)
{
   Init_Boot_App();
   CANTP_Init();
   UDS_Init();
}



