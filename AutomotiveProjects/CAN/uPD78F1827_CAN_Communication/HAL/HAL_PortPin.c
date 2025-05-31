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
#include <io78f1827_48.h>
#include <io78f1827_48_ext.h>
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
   ADPC =0;/*an0 to an23 as digital io*/
   
  /*################ PORT MODE ###########
   0 Output mode (output buffer on)
   1 Input mode (output buffer off)   
   ################ PORT MODE ###########*/

   /*Port 0 registers*/
   PM0_bit.no0 = 0;//nc

   /*Port 1 registers*/
   PM1_bit.no0 = 0;//ilin tx
   PM1_bit.no1 = 1;//ilin rx
   PM1_bit.no2 = 0;//wakeup FIDM LS op - alt mode 0 for trcv
   PM1_bit.no3 = 0;//tlin tx
   PM1_bit.no4 = 0;//tlin rx
   PM1_bit.no5 = 1;//nc
   PM1_bit.no6 = 0;//nc
   PM1_bit.no7 = 1;//swp irq sense
   /*Port 2 registers*/


   /*Port 3 registers*/
   PM3_bit.no0 = 0;//output test
   PM3_bit.no1 = 1;//wakeup request LS ip
   PM3_bit.no2 = 0;//output wakeup fidm - alt mode1 for trcv

   /*Port 4 registers*/
   PM4_bit.no1 = 0;//slp_n op for LIN

   /*Port 5 registers*/

   /*Port 6 registers*/
   PM6_bit.no0 = 0;//scl
   PM6_bit.no1 = 0;//sda
   PM6_bit.no2 = 0;//nc
   PM6_bit.no3 = 0;//nc

   /*Port 7 registers*/
   PM7_bit.no0 = 0;//nfc_rst
   PM7_bit.no3 = 0;//nc
   PM7_bit.no2 = 0;//nc
   PM7_bit.no1 = 0;//irq
   
 
   
   /*Port 8 registers*/
   PM8_bit.no6 = 0;//mode select
   PM8_bit.no5 = 0;//nc
   PM8_bit.no4 = 0;//nc
   PM8_bit.no2 = 0;//nc
   PM8_bit.no1 = 0;//nc
   PM8_bit.no0 = 0;//nc
   
   /*Port 9 registers*/
   PM9_bit.no0 = 0;//nc
   PM9_bit.no1 = 0;//nc
   PM9_bit.no2 = 0;//nc
 

   /*Port 12 registers*/
   PM12_bit.no0 = 1;//clock streching input pin
   PM12_bit.no3 = 1;//chg pin (alternate)
   PM12_bit.no4 = 0;//nfc irq
   PM12_bit.no5 = 0;//nfc_program make it output,connected to mode 0 pin
  
   /*Port 14 registers*/
   PM14_bit.no0 = 0;//nfc rst

 
   /*################ PORT REGISTER ###########
     0 Output 0 Input low level
     1 Output 1 Input high level
    ################ PORT REGISTER ###########*/

   P0_bit.no0 = 0; 

   P1_bit.no0 = 0; 
   P1_bit.no2 = 1;//fidm wake up LS - mode 0 of GMLAN TRCV
   P1_bit.no1 = 1; 
   P1_bit.no3 = 0; 
   P1_bit.no4 = 0; 
   P1_bit.no5 = 0; 
   P1_bit.no6 = 0; 
   P1_bit.no7 = 1; //slp_n 

   P3_bit.no0 = 0;//
   P3_bit.no1 = 0;//
   P3_bit.no2 = 0;// mode 1 of swcan trcv 

   P4_bit.no1 = 1;//slp_n is 1 for Normal operation

   P6_bit.no0 = 0; 
   P6_bit.no1 = 0; 

   P7_bit.no0 = 1;//nfc ic in ven state
   P7_bit.no3 = 0;
   P7_bit.no2 = 0;
   P7_bit.no1 = 0;

   P8_bit.no2 = 0; 
   P8_bit.no1 = 0; 
   P8_bit.no4 = 0; 
   P8_bit.no5 = 0; 
   P8_bit.no6 = 0; 
   P8_bit.no0 = 0; 

   P9_bit.no0 = 0;
   P9_bit.no1 = 0;
   P9_bit.no2 = 0;

   P12_bit.no0 = 1;//clock stretching
   P12_bit.no3 = 0;   
   P12_bit.no4 = 0;
   P12_bit.no5 = 0;
  
   P14_bit.no0 = 0;


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

