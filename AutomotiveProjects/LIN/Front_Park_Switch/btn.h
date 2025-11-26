
#ifndef BTN_H
#define BTN_H

#include "pic12f1822.h"
#include <stdio.h>

/**********************************************************************************
Global definition
***********************************************************************************/

#define PIN_MAKE_THR          3u
#define PIN_MAKE_SHORT_THR    15u
#define PIN_MAKE_LONG_THR     30u
#define PIN_MAKE_STUCK_THR    100u

typedef enum
{
   NO_PRESS_TYPE,
   SHORT_PRESS_TYPE,
   LONG_PRESS_TYPE,
   STUCK_TYPE        
}btn_press_type_t;

/**********************************************************************************
Global variables
***********************************************************************************/

/***********************************************************************************
Function Prototypes
***********************************************************************************/
extern void App_Btn_Process(void);
extern void Init_Btn(void);



#endif

/*-----------------------------------------------------------------------------
 * Version      Description                       Name       Date
 *
 *---------------------------------------------------------------------------*/

