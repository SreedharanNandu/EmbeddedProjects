
/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
#include "LPC11xx.h"
#include "App_Si_Intf.h"
#include "i2c.h"
#include "App_Si.h"
#include "type.h"
#include <string.h>
#include "timer16.h"
#include "APP_Si_Am_Table.h"
#include "APP_Si_Fm_Table.h"

/*****************************************************************************
*                                 Macro Definitions                          *
******************************************************************************/


/*****************************************************************************
*                                 Type Declarations                          *
******************************************************************************/

const Si4735_AM_Table_T Si4735_AM_Table[] = 
{
   {CMD_SIZE_AM_POWER_UP      ,RESP_SIZE_AM_POWER_UP      ,  },
   {CMD_SIZE_AM_GET_REV       ,RESP_SIZE_AM_GET_REV       ,  },
   {CMD_SIZE_AM_POWER_DOWN    ,RESP_SIZE_AM_POWER_DOWN    ,  },
   {CMD_SIZE_AM_SET_PROPERTY  ,RESP_SIZE_AM_SET_PROPERTY  ,  },
   {CMD_SIZE_AM_GET_PROPERTY  ,RESP_SIZE_AM_GET_PROPERTY  ,  },
   {CMD_SIZE_AM_GET_INT_STATUS,RESP_SIZE_AM_GET_INT_STATUS,  },
   {CMD_SIZE_AM_TUNE_FREQ     ,RESP_SIZE_AM_TUNE_FREQ     ,  },
   {CMD_SIZE_AM_SEEK_START    ,RESP_SIZE_AM_SEEK_START    ,  },
   {CMD_SIZE_AM_TUNE_STATUS   ,RESP_SIZE_AM_TUNE_STATUS   ,  },
   {CMD_SIZE_AM_RSQ_STATUS    ,RESP_SIZE_AM_RSQ_STATUS    ,  },
   {CMD_SIZE_AM_AGC_STATUS    ,RESP_SIZE_AM_AGC_STATUS    ,  },
   {CMD_SIZE_AM_AGC_OVERRIDE  ,RESP_SIZE_AM_AGC_OVERRIDE  ,  },
   {CMD_SIZE_AM_GPIO_CTL      ,RESP_SIZE_AM_GPIO_CTL      ,  },
   {CMD_SIZE_AM_GPIO_SET      ,RESP_SIZE_AM_GPIO_SET         },

};
   



/*****************************************************************************
*                    Globally  accessed Variable Declarations    
******************************************************************************/










