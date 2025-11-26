
#include "system_def.h"

#include "App_Task.h"
#include "App_Can_Tp.h"
#include "App_Uds.h"

void Init_APP(void)
{
   CANTP_Init();
   UDS_Init();
}



