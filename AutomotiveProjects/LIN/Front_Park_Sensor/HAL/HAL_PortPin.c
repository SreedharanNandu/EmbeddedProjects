/******************************************************************************
** Project Name         : 
** File Name            : HAL_PortPin.C
** Description          : This module is used to configure the IO ports
** Date                 : 
** Hardware Platform    : 
******************************************************************************/

/* Define to include local variables in the header file */
#define _HAL_PortPin_C


/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
/*<list all system include files>*/

#include <CG_macrodriver.h>

#include "system_def.h"
#include "HAL_PortPin_API.h"

/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
/*<list all local macro declarations> */

/******************************************************************************
**               D A T A
******************************************************************************/
/*<list all local variables>  */


/******************************************************************************
**                  F U N C T I O N     D E F I N I T I O N S
******************************************************************************/
/*<list all function definitins here>  */

/******************************************************************************
* Function Name     : HAL_PORTPIN_Init
* Input Params      : None
* Output Params     : None
* Description       : Brief description of the function ; list applicable requirement IDs from SRS>
******************************************************************************/
void HAL_Port_Init(void)
{  
   ADPC = 1;/*an1 to an23 as digital io , an0 is switch*/
   
  /*################ PORT MODE ###########
   0 Output mode (output buffer on)
   1 Input mode (output buffer off)   
   ################ PORT MODE ###########*/

   /*Port 0 registers*/
   PM0.0 = 0;//nc

   /*Port 1 registers*/
   PM1.0 = 0;//ilin tx
   PM1.1 = 1;//ilin rx
   PM1.2 = 0;//wakeup FIDM LS op - alt mode 0 for trcv
   PM1.3 = 0;//tlin tx
   PM1.4 = 1;//tlin rx
   PM1.5 = 1;//nc
   PM1.6 = 0;//nc
   PM1.7 = 1;//swp irq sense
   /*Port 2 registers*/


   /*Port 3 registers*/
   PM3.0 = 0;//output test
   PM3.1 = 0;//wakeup request made output
   PM3.2 = 0;//output wakeup fidm - alt mode1 for trcv

   /*Port 4 registers*/
   PM4.1 = 0;//slp_n op for LIN

   /*Port 5 registers*/

   /*Port 6 registers*/
   PM6.0 = 0;//scl
   PM6.1 = 0;//sda
   PM6.2 = 0;//nc
   PM6.3 = 0;//nc

   /*Port 7 registers*/
   PM7.0 = 0;//nfc_rst
   PM7.3 = 0;//nc
   PM7.2 = 0;//nc
   PM7.1 = 0;//irq
   
 
   
   /*Port 8 registers*/
   PM8.6 = 0;//mode select
   PM8.5 = 0;//nc
   PM8.4 = 0;//nc
   PM8.2 = 0;//nc
   PM8.1 = 0;//nc
   #if 0   
   PM8.0 = 1;//nc
   #endif   
   /*Port 9 registers*/
   PM9.0 = 0;//nc
   PM9.1 = 0;//nc
   PM9.2 = 0;//nc
 

   /*Port 12 registers*/
   PM12.0 = 0;//
   PM12.5 = 0;//nfc_program make it output,connected to mode 0 pin
  
   /*Port 14 registers*/
   PM14.0 = 0;//nfc rst

 
   /*################ PORT REGISTER ###########
     0 Output 0 Input low level
     1 Output 1 Input high level
    ################ PORT REGISTER ###########*/

   P0.0 = 0; 

   P1.0 = 0; 
   P1.2 = 1;//fidm wake up LS
   P1.1 = 1; 
   P1.3 = 0; 
   P1.4 = 0; 
   P1.5 = 0; 
   P1.6 = 0; 
   P1.7 = 1; //slp_n 

   P3.0 = 0;//
   P3.1 = 0;//
   P3.2 = 0;// mode 1 of swcan trcv 

   P4.1 = 1;//slp_n is 1 for Normal operation

   P6.0 = 0; 
   P6.1 = 0; 

   P7.0 = 1;//nfc ic in ven state
   P7.3 = 0;
   P7.2 = 0;
   P7.1 = 0;

   P8.2 = 0; 
   P8.1 = 0; 
   P8.4 = 0; 
   P8.5 = 0; 
   P8.6 = 0; 
   #if 0
   P8.0 = 0; 
   #endif 
   P9.0 = 0;
   P9.1 = 0;
   P9.2 = 0;

   P12.0 = 1;//clock stretching
 
   P12.5 = 0;
  
   P14.0 = 0;


   /*########### Pull-up Resistor Option Register ##############
   0 On-chip pull-up resistor not connected
   1 On-chip pull-up resistor connected*/
   
   PU0 = 0u;
   PU1 = 0xffu; //pull up enabled for the LIN to work
   PU3 = 0u; 
   PU4 = 0u; 
   PU6 = 0u; 
   PU7 = 0u; 
   PU12 = 0xffu;
   PU14 = 0u;
   
   /*########### Port input mode Register ##############
   0 Normal input buffer
   1 TTL input buffer*/

   PIM6 = 0u;
   PIM7 = 0u;
   
   /*########### Port output mode Register ##############
   0 Normal output mode
   1 N-ch open-drain output (VDD tolerance) mode*/
   POM7 = 0u;
   
   /*######### Port output slew rate select register ######
   0 Normal mode (5 ns/5 V (TYP.))
   1 Slow mode (25 ns/5 V (TYP.))*/
   PSRSEL = 0u;

}
/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
** Revision No            :
** Date(dd/mm/yy)         :          
** Author                 : 
** Description of change  :
*******************************************************************************/

