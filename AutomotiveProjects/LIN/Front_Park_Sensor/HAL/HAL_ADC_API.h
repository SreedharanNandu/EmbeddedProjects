/******************************************************************************
*******************************************************************************
** Project Name      :
** File Name         : HAL_ADC_API.h
** Description       : This module is used to configure the registers for ADC 
** Date              : 
** Hardware Platform :  
******************************************************************************/
#ifndef HAL_ADC_API_H
#define HAL_ADC_API_H

/******************************************************************************
**               P R O J E C T        I N C L U D E S
******************************************************************************/


#include "system_def.h"

/*
*******************************************************************************
**  Register bit define
*******************************************************************************
*/
/*
   Peripheral enable register 0 (PER0)
*/
/* Control of AD converter input clock (ADCEN) */
#define _00_AD_CLOCK_STOP      0x00U   /* stops supply of input clock */
#define _80_AD_CLOCK_SUPPLY      0x80U   /* supplies input clock */

/*
   AD converter mode register 0 (ADM0)
*/
#define _00_AD_ADM0_INITIALVALUE   0x00U
/* AD conversion operation control (ADCS) */
#define _80_AD_CONVERSION_ENABLE   0x80U   /* enable AD conversion operation control */
#define _00_AD_CONVERSION_DISABLE   0x00U   /* disable AD conversion operation control */
/* Specification of AD conversion operation mode (ADMD2 - ADMD0) */
#define   _00_AD_MODE_CONSELECT      0x00U   /* continuous select mode */
#define   _20_AD_MODE_CONSCAN1      0x20U   /* continuous scan mode (interrupt generated for each channel) */
#define   _30_AD_MODE_CONSCAN2      0x30U   /* continuous scan mode (interrupt generated after conversion of channel, specified by ADS register) */
#define   _40_AD_MODE_ONESELECT      0x40U   /* one-shot select mode */
#define   _60_AD_MODE_ONESCAN1      0x60U   /* one-shot scan mode (interrupt generated for each channel) */
#define   _70_AD_MODE_ONESCAN2      0x70U   /* one-shot scan mode (interrupt generated after conversion of channel, specified by ADS register) */
#define   _20_AD_MODE_SCAN      0x20U   /* scan mode */
#define   _00_AD_MODE_SELECT      0x00U   /* select mode */
/* Discharge function control (ADDC) */
#define _00_AD_DISCHARGE_DISABLE   0x00U   /* disable discharge function */
#define _08_AD_DISCHARGE_ENABLE      0x08U   /* enable discharge function */
/* Specification of AD conversion time mode (ADSH1, ADHS0) */
#define   _00_AD_TIME_MODE_HIGHSPEED1   0x00U   /* high-speed conversion mode 1 */
#define   _02_AD_TIME_MODE_HIGHSPEED2   0x02U   /* high-speed conversion mode 2 */
#define   _04_AD_TIME_MODE_NORMAL      0x04U   /* normal conversion mode */
/* AD comparator operation control (ADCE) */
#define _01_AD_COMPARATOR_ENABLE   0x01U   /* enable comparator operation control */
#define _00_AD_COMPARATOR_DISABLE   0x00U   /* disable comparator operation control */

/*
   AD converter mode register 1 (ADM1)
*/
/* AD trigger mode selection(ADTMD) */
#define _00_AD_TRIGGER_SOFTWARE      0x00U   /* software trigger mode */
#define _80_AD_TRIGGER_HARDWARE      0x80U   /* hardware trigger mode(time trigger mode and external pin trigger signal) */
/* Timer trigger signal selection (ADTMS1, ADTMS0) */
#define _00_AD_TRIGGER_TTRG0      0x00U   /* timer trigger signal 0 */
#define _04_AD_TRIGGER_TTRG1      0x04U   /* timer trigger signal 1 */
#define _08_AD_TRIGGER_TTRG2      0x08U   /* timer trigger signal 2 */
#define _0C_AD_TRIGGER_EXTPIN      0x0CU   /* external pin trigger signal(ADTRG) */
/* Specification of external trigger (ADTRG pin) input valid edge (ADETS1, ADETS0) */
#define   _01_AD_TRG_EDGE_FALLING      0x01U   /* falling edge detection */
#define   _02_AD_TRG_EDGE_RISING      0x02U   /* rising edge detection */
#define   _03_AD_TRG_EDGE_BOTH      0x03U   /* detection of both rising and falling edges */

/*
   AD port configuration register (ADPC)
*/
/* Analog input /digital input switching (ADPC4 - ADPC0) */
#define _00_AD_ADPC_0ANALOG      0x00U   /* ANI00 - ANI23 (all digital) */
#define _01_AD_ADPC_1ANALOG      0x01U   /* ANI00 */
#define _02_AD_ADPC_2ANALOG      0x02U   /* ANI00 - ANI01 */
#define _03_AD_ADPC_3ANALOG      0x03U   /* ANI00 - ANI02 */
#define _04_AD_ADPC_4ANALOG      0x04U   /* ANI00 - ANI03 */
#define _05_AD_ADPC_5ANALOG      0x05U   /* ANI00 - ANI04 */
#define _06_AD_ADPC_6ANALOG      0x06U   /* ANI00 - ANI05 */
#define _07_AD_ADPC_7ANALOG      0x07U   /* ANI00 - ANI06 */
#define _08_AD_ADPC_8ANALOG      0x08U   /* ANI00 - ANI07 */
#define _09_AD_ADPC_9ANALOG      0x09U   /* ANI00 - ANI08 */
#define _0A_AD_ADPC_10ANALOG      0x0AU   /* ANI00 - ANI09 */
#define _0B_AD_ADPC_11ANALOG      0x0BU   /* ANI00 - ANI10 */
#define _0C_AD_ADPC_12ANALOG      0x0CU   /* ANI00 - ANI11 */
#define _0D_AD_ADPC_13ANALOG      0x0DU   /* ANI00 - ANI12 */
#define _0E_AD_ADPC_14ANALOG      0x0EU   /* ANI00 - ANI13 */
#define _0F_AD_ADPC_15ANALOG      0x0FU   /* ANI00 - ANI14 */
#define _10_AD_ADPC_16ANALOG      0x10U   /* ANI00 - ANI15 */
#define _11_AD_ADPC_17ANALOG      0x11U   /* ANI00 - ANI16 */
#define _12_AD_ADPC_18ANALOG      0x12U   /* ANI00 - ANI17 */
#define _13_AD_ADPC_19ANALOG      0x13U   /* ANI00 - ANI18 */
#define _14_AD_ADPC_20ANALOG      0x14U   /* ANI00 - ANI19 */
#define _15_AD_ADPC_21ANALOG      0x15U   /* ANI00 - ANI20 */
#define _16_AD_ADPC_22ANALOG      0x16U   /* ANI00 - ANI21 */
#define _17_AD_ADPC_23ANALOG      0x17U   /* ANI00 - ANI22 */
#define _18_AD_ADPC_24ANALOG      0x18U   /* ANI00 - ANI23 */

/*
   Analog input channel specification register (ADS)
*/
/* Specification of Analog input channel (ADS4 - ADS0) */
#define   _00_AD_ANI_INPUT00      0x00U   /* select mode: ANI00; scan mode: ANI00 */
#define   _01_AD_ANI_INPUT01      0x01U   /* select mode: ANI01; scan mode: ANI00, ANI01 */
#define   _02_AD_ANI_INPUT02      0x02U   /* select mode: ANI02; scan mode: ANI00 to ANI02 */
#define   _03_AD_ANI_INPUT03      0x03U   /* select mode: ANI03; scan mode: ANI00 to ANI03 */
#define   _04_AD_ANI_INPUT04      0x04U   /* select mode: ANI04; scan mode: ANI00 to ANI04 */
#define   _05_AD_ANI_INPUT05      0x05U   /* select mode: ANI05; scan mode: ANI00 to ANI05 */
#define   _06_AD_ANI_INPUT06      0x06U   /* select mode: ANI06; scan mode: ANI00 to ANI06 */
#define   _07_AD_ANI_INPUT07      0x07U   /* select mode: ANI07; scan mode: ANI00 to ANI07 */
#define   _08_AD_ANI_INPUT08      0x08U   /* select mode: ANI08; scan mode: ANI00 to ANI08 */
#define   _09_AD_ANI_INPUT09      0x09U   /* select mode: ANI09; scan mode: ANI00 to ANI09 */
#define   _0A_AD_ANI_INPUT10      0x0AU   /* select mode: ANI10; scan mode: ANI00 to ANI10 */
#define   _0B_AD_ANI_INPUT11      0x0BU   /* select mode: ANI11; scan mode: ANI00 to ANI11 */
#define   _0C_AD_ANI_INPUT12      0x0CU   /* select mode: ANI12; scan mode: ANI00 to ANI12 */
#define   _0D_AD_ANI_INPUT13      0x0DU   /* select mode: ANI13; scan mode: ANI00 to ANI13 */
#define   _0E_AD_ANI_INPUT14      0x0EU   /* select mode: ANI14; scan mode: ANI00 to ANI14 */
#define   _0F_AD_ANI_INPUT15      0x0FU   /* select mode: ANI15; scan mode: ANI00 to ANI15 */
#define   _10_AD_ANI_INPUT16      0x10U   /* select mode: ANI16; scan mode: ANI00 to ANI16 */
#define   _11_AD_ANI_INPUT17      0x11U   /* select mode: ANI17; scan mode: ANI00 to ANI17 */
#define   _12_AD_ANI_INPUT18      0x12U   /* select mode: ANI18; scan mode: ANI00 to ANI18 */
#define   _13_AD_ANI_INPUT19      0x13U   /* select mode: ANI19; scan mode: ANI00 to ANI19 */
#define   _14_AD_ANI_INPUT20      0x14U   /* select mode: ANI20; scan mode: ANI00 to ANI20 */
#define   _15_AD_ANI_INPUT21      0x15U   /* select mode: ANI21; scan mode: ANI00 to ANI21 */
#define   _16_AD_ANI_INPUT22      0x16U   /* select mode: ANI22; scan mode: ANI00 to ANI22 */
#define   _17_AD_ANI_INPUT23      0x17U   /* select mode: ANI23; scan mode: ANI00 to ANI23 */

/*
   AD conversion time setting register (ADSMP)
*/
#define _00_AD_ADSMP_INITIALVALUE   0x00U   /* ADSMP initialvalue */
/* Specification of the number of conversion clocks of one channel (ADSMP3 - ADSMP0) */
/* No Discharge */
#define _00_AD_NUM_CLOCK0_NODIS      0x00U   /* high-speed mode 1: 16; high-speed mode 2: 17; normal mode: 22 */
#define _01_AD_NUM_CLOCK1_NODIS      0x01U   /* high-speed mode 1: 18; high-speed mode 2: 20; normal mode: 40 */
#define _02_AD_NUM_CLOCK2_NODIS      0x02U   /* high-speed mode 1: 20; high-speed mode 2: 35; normal mode: 58 */
#define _03_AD_NUM_CLOCK3_NODIS      0x03U   /* high-speed mode 1: 33; high-speed mode 2: 37; normal mode: 76 */
#define _04_AD_NUM_CLOCK4_NODIS      0x04U   /* high-speed mode 1: 34; high-speed mode 2: 40; normal mode: 94 */
#define _05_AD_NUM_CLOCK5_NODIS      0x05U   /* high-speed mode 1: 37; high-speed mode 2: 56; normal mode: 114 */
#define _06_AD_NUM_CLOCK6_NODIS      0x06U   /* high-speed mode 1: 50; high-speed mode 2: 59; normal mode: 132 */
#define _07_AD_NUM_CLOCK7_NODIS      0x07U   /* high-speed mode 1: 52; high-speed mode 2: 72; normal mode: 150 */
#define _08_AD_NUM_CLOCK8_NODIS      0x08U   /* high-speed mode 1: 54; high-speed mode 2: 76; normal mode: 168 */
#define _09_AD_NUM_CLOCK9_NODIS      0x09U   /* high-speed mode 1: 55; high-speed mode 2: 78; normal mode: 186 */
#define _0A_AD_NUM_CLOCK10_NODIS   0x0AU   /* high-speed mode 1: 71; high-speed mode 2: 96; normal mode: 208 */
#define _0B_AD_NUM_CLOCK11_NODIS   0x0BU   /* high-speed mode 1: 73; high-speed mode 2: 99; normal mode: 226 */
/* Discharge Performed */
#define _00_AD_NUM_CLOCK0_DIS      0x00U   /* high-speed mode 1: 19; high-speed mode 2: 22; normal mode: 27 */
#define _01_AD_NUM_CLOCK1_DIS      0x01U   /* high-speed mode 1: 22; high-speed mode 2: 25; normal mode: 47 */
#define _02_AD_NUM_CLOCK2_DIS      0x02U   /* high-speed mode 1: 24; high-speed mode 2: 42; normal mode: 68 */
#define _03_AD_NUM_CLOCK3_DIS      0x03U   /* high-speed mode 1: 38; high-speed mode 2: 44; normal mode: 88 */
#define _04_AD_NUM_CLOCK4_DIS      0x04U   /* high-speed mode 1: 39; high-speed mode 2: 48; normal mode: 108 */
#define _05_AD_NUM_CLOCK5_DIS      0x05U   /* high-speed mode 1: 44; high-speed mode 2: 67; normal mode: 132 */
#define _06_AD_NUM_CLOCK6_DIS      0x06U   /* high-speed mode 1: 58; high-speed mode 2: 70; normal mode: 152 */
#define _07_AD_NUM_CLOCK7_DIS      0x07U   /* high-speed mode 1: 60; high-speed mode 2: 84; normal mode: 173 */
#define _08_AD_NUM_CLOCK8_DIS      0x08U   /* high-speed mode 1: 63; high-speed mode 2: 89; normal mode: 193 */
#define _09_AD_NUM_CLOCK9_DIS      0x09U   /* high-speed mode 1: 64; high-speed mode 2: 92; normal mode: 213 */
#define _0A_AD_NUM_CLOCK10_DIS      0x0AU   /* high-speed mode 1: 83; high-speed mode 2: 115; normal mode: 240 */
#define _0B_AD_NUM_CLOCK11_DIS      0x0BU   /* high-speed mode 1: 86; high-speed mode 2: 119; normal mode: 260 */
/*
*******************************************************************************
**  Macro define
*******************************************************************************
*/
enum ADChannel 
{
   ADCHANNEL0, ADCHANNEL1, ADCHANNEL2, ADCHANNEL3, ADCHANNEL4, ADCHANNEL5, 
   ADCHANNEL6, ADCHANNEL7, ADCHANNEL8, ADCHANNEL9, ADCHANNEL10
};
#define   AD_CR_START         0x00280U

/******************************************************************************
**               S T A R T      G L O B A L      D E F I N I T I O N 
******************************************************************************/


/*****************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
/*list all global macros*/


/******************************************************************************
**               D A T A
******************************************************************************/
extern u16 fl_ADC_Counts_u16[4];


/******************************************************************************
**                 G L O B A L   F U N C T I O N S
******************************************************************************/
/*list all global functions*/

extern void HAL_ADC_Init(void);
extern void HAL_ADC_Read_Channel_Counts(u16 * buffer);
extern void HAL_ADC_Start_Operation(void);
extern void HAL_ADC_Select_Channel(enum ADChannel channel);
extern void Task_ADC_Convert(void);

#endif 


/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
** Revision No                     : 
** Date(dd/mm/yy)                  :           
** Author                          : 
** Description of change           : 
**
*******************************************************************************/


