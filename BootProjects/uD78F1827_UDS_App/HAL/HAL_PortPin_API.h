/******************************************************************************
** Project Name         :
** File Name            : HAL_PortPin_API.h
** Description          : This module is used to configure the Port Pins 
** Date                 : 
** Hardware Platform    :  
******************************************************************************/
#ifndef _HAL_PortPin_API_H
#define _HAL_PortPin_API_H

/******************************************************************************
**   P R O J E C T        I N C L U D E S
******************************************************************************/
/*list all include files*/
#include "CG_port.h"


/******************************************************************************
**   S T A R T      G L O B A L      D E F I N I T I O N 
******************************************************************************/

/*****************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
/*list all global macros*/
#define WRITE_PORT_BIT(PORT, BIT, VALUE) \
    do { \
        if(VALUE == 1U)            \
        {                          \
           PORT |=  (1U << (BIT)); \
        }                          \
        else                       \
        {                          \
           PORT &= ~(1U << (BIT)); \
        }                          \
    } while (0)

// Set a specific bit (make it HIGH)
#define SET_PORT_BIT(PORT, BIT)    ((PORT) |= (1U << (BIT)))
// Clear a specific bit (make it LOW)
#define CLEAR_PORT_BIT(PORT, BIT)  ((PORT) &= ~(1U << (BIT)))
// Toggle a bit
#define TOGGLE_PORT_BIT(PORT, BIT) ((PORT) ^= (1U << (BIT)))
// Read a bit (returns 0 or non-zero)
#define READ_PORT_BIT(PORT, BIT)   (((PORT) >> (BIT)) & 0x1U)

/******************************************************************************
**               D A T A
******************************************************************************/
/*list all global variables*/

/******************************************************************************
**                 G L O B A L   F U N C T I O N S
******************************************************************************/
extern void HAL_Port_Init(void);

#endif /* for #define _MODULE_NAME_H */


/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
** Revision No            :
** Date(dd/mm/yy)         :          
** Author                 : 
** Description of change  :
**
*******************************************************************************/


