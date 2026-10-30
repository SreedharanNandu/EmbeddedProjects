/******************************************************************************
*******************************************************************************
** Project Name         :
** File Name            : HAL_Timer_API
** Description          : This module is used to configure the Timer
** Date                 : 
** Hardware Platform    :  
******************************************************************************/
#ifndef HAL_Timer_API
#define HAL_Timer_API



/******************************************************************************
**   P R O J E C T        I N C L U D E S
******************************************************************************/




/******************************************************************************
**   S T A R T      G L O B A L      D E F I N I T I O N 
******************************************************************************/


/*****************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/

#define Enable_TAU0_Interrupt()  (TMMK00 = 0u)
#define Disable_TAU0_Interrupt() (TMMK00 = 1u)

/*
   Timer Clock Select Register m (TPSm)
*/
/* Operating mode and clear mode selection (PRSm33 - PRSm30) */
#define _0040_TAU_CKM1_FCLK_4            0x0040U   /* CKm1-fCLK/2^4 */
#define _0800_TAU_CKM2_FCLK_8            0x0800U   /* CKm2-fCLK/2^8 */
#define _F000_TAU_CKM3_FCLK_15            0xF000U   /* CKm3-fCLK/2^15 */
/* Operating mode and clear mode selection (PRSm03 - PRSm00) */
#define _0000_TAU_CKM0_FCLK_0            0x0000U   /* CKm0-fCLK */
/*
   Timer Mode Register mn (TMRmn)
*/

/* Selection of macro clock (MCK) of channel n (CKSmn1,CKSmn0) */
#define _0000_TAU_CLOCK_SELECT_CKM0         0x0000U   /* operation clock CKm0 set by TPSm register */
/* Selection of count clock (TCLK) of channel n (CCSmn) */
#define _0000_TAU_CLOCK_MODE_CKS         0x0000U   /* macro clock MCK specified by CKSmn bit */
/* Selection of slave/master of channel n (MASTERmn) */
#define _0000_TAU_COMBINATION_SLAVE         0x0000U   /* operate as slave channel with combination operation function */
#define _0800_TAU_COMBINATION_MASTER         0x0800U   /* operates as master channel with combination operation function */
/* Setting of start trigger or capture trigger of channel n (STSmn2 - STSmn0) */
#define _0000_TAU_TRIGGER_SOFTWARE         0x0000U   /* only software trigger start is valid */
#define _0400_TAU_TRIGGER_MASTER_INT         0x0400U   /* interrupt signal of the master channel is used */
/* Operation mode of channel n (MDmn3, MDmn0) */
#define _0001_TAU_MODE_PWM_MASTER         0x0001U   /* PWM Function (Master Channel) mode */
#define _0009_TAU_MODE_PWM_SLAVE         0x0009U   /* PWM Function (Slave Channel) mode */

/* Operation stop trigger of channel 7 (TTm7) */
#define _0080_TAU_CH7_STOP_TRG_ON         0x0080U   /* stop operation (generate stop trigger) */
/* Operation stop trigger of channel 6 (TTm6) */
#define _0040_TAU_CH6_STOP_TRG_ON         0x0040U   /* stop operation (generate stop trigger) */
/* Operation stop trigger of channel 5 (TTm5) */
#define _0020_TAU_CH5_STOP_TRG_ON         0x0020U   /* stop operation (generate stop trigger) */
/* Operation stop trigger of channel 4 (TTm4) */
#define _0010_TAU_CH4_STOP_TRG_ON         0x0010U   /* stop operation (generate stop trigger) */
/* Operation stop trigger of channel 3 (TTm3) */
#define _0008_TAU_CH3_STOP_TRG_ON         0x0008U   /* stop operation (generate stop trigger) */
/* Operation stop trigger of channel 2 (TTm2) */
#define _0004_TAU_CH2_STOP_TRG_ON         0x0004U   /* stop operation (generate stop trigger) */
/* Operation stop trigger of channel 1 (TTm1) */
#define _0002_TAU_CH1_STOP_TRG_ON         0x0002U   /* stop operation (generate stop trigger) */
/* Operation stop trigger of channel 0 (TTm0) */
#define _0001_TAU_CH0_STOP_TRG_ON         0x0001U   /* stop operation (generate stop trigger) */
/* Timer output enable/disable of channel 1 (TOEm1) */
#define _0002_TAU_CH1_OUTPUT_ENABLE         0x0002U   /* enable TOm7 operation by count operation (timer channel output bit) */
/* Timer output enable/disable of channel 0 (TOEm0) */
#define _0001_TAU_CH0_OUTPUT_ENABLE         0x0001U   /* enable TOm7 operation by count operation (timer channel output bit) */
/* Timer output of channel 1 (TOm1) */
#define _0002_TAU_CH1_OUTPUT_VALUE_1         0x0002U   /* timer output value is "1" */
/* Control of timer output level of channel 1 (TOLm1) */
#define _0002_TAU_CH1_OUTPUT_LEVEL_L         0x0002U   /* inverted output (active-low) */
/* Control of timer output level of channel 0 (TOLm0) */
#define _0001_TAU_CH0_OUTPUT_LEVEL_L         0x0001U   /* inverted output (active-low) */
/* Control of timer output mode of channel 1 (TOMm1) */
#define _0002_TAU_CH1_OUTPUT_COMBIN         0x0002U   /* combination operation mode */
/* Control of timer output mode of channel 0 (TOMm0) */
#define _0001_TAU_CH0_OUTPUT_COMBIN         0x0001U   /* combination operation mode */
/* 16-bit timer data register 10 (TDR10) */
#define _C34F_TAU_TDR10_VALUE            0xC34FU
/* 16-bit timer data register 11 (TDR11) */
#define _61A8_TAU_TDR11_VALUE            0x61A8U

#define   _02_WAKEUPTM_INTERNAL_CLOCK2         0x02U   /* fMAIN/2^8 */


#define _003F_WAKEUPTM_WUTMCMP_VALUE      0x3Fu
#define _0137_WAKEUPTM_WUTMCMP_VALUE     0x0137u
#define _0001_TAU_CH0_START_TRG_ON         0x0001U   /* enable operation (generate start software trigger) */ 
#define _0002_TAU_CH1_START_TRG_ON         0x0002U   /* enable operation (generate start software trigger) */
#define _0008_TAU_CH3_START_TRG_ON         0x0008U   /* enable operation (generate start software trigger) */
#define _0010_TAU_CH4_START_TRG_ON         0x0010U   /* enable operation (generate start software trigger) */
#define _0004_TAU_CH2_OUTPUT_ENABLE         0x0004U   /* enable TOm7 operation by count operation (timer channel output bit) */
#define _0008_TAU_CH3_OUTPUT_ENABLE         0x0008U   /* enable TOm7 operation by count operation (timer channel output bit) */
#define _0010_TAU_CH4_OUTPUT_ENABLE         0x0010U   /* enable TOm7 operation by count operation (timer channel output bit) */
#define _0004_TAU_CH2_START_TRG_ON         0x0004U   /* enable operation (generate start software trigger) */


#define Set_T01(x)  (TDR01 = x) 
#define Set_T11(x)  (TDR11 = x)
#define Set_T12(x)  (TDR12 = x)
#define Set_T13(x)  (TDR13 = x)
#define Set_T14(x)  (TDR14 = x)






/*******************************************************************************
**  Function define
*******************************************************************************
*/
void TAU1_Init(void);
void TAU1_Channel0_Start(void);
void TAU1_Channel0_Stop(void);
void TAU0_Init(void);
void TAU0_Channel1_ChangeDuty(u8 ratio);
__interrupt void MD_INTTM11(void);

void WAKEUPTM_Init(void);
void WAKEUPTM_Start(void);
void WAKEUPTM_Stop(void);
void TAU0_Channel0_Start(void);
void TAU0_Channel0_Stop(void);
void TAU1_Channel1_ChangeDuty(u8 ratio);
void TAU1_Channel2_ChangeDuty(u8 ratio);
void TAU1_Channel3_ChangeDuty(u8 ratio);
void TAU1_Channel4_ChangeDuty(u8 ratio);

/******************************************************************************
**                 G L O B A L   F U N C T I O N S
******************************************************************************/

#endif
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