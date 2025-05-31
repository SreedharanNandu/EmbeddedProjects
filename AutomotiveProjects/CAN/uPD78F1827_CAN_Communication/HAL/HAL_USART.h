/******************************************************************************
*******************************************************************************
** Project Name         :
** File Name            : HAL_USART.c
** Description          : This module is used to define USART driver
** Date                 : 
** Hardware Platform    : 
******************************************************************************/
#ifndef HAL_USART_H
#define HAL_USART_H

/******************************************************************************
**               P R O J E C T        I N C L U D E S
******************************************************************************/
/*list all include files*/


/******************************************************************************
**               S T A R T      G L O B A L      D E F I N I T I O N 
******************************************************************************/


/*****************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/

typedef enum
{
  WAIT_START=0u,
  WAIT_ID,
  WAIT_DCL,
  WAIT_DATA,
  WAIT_CKSUM
}USART_WAIT_STATE;

extern volatile USART_WAIT_STATE state;

typedef enum
{
  SERIAL_TRANSMIT_STATUS_PENDING,
  SERIAL_TRANSMIT_STATUS_COMPLETE,
} SERIAL_TRANSMIT_STATUS_ID;

typedef enum
{
  SERIAL_RECEIVE_STATUS_PENDING,
  SERIAL_RECEIVE_STATUS_COMPLETE,
} SERIAL_RECEIVE_STATUS_ID;

typedef enum
{
  idle_msg1,
  break_msg1,
  sync_msg1,
  message_id1,
  byte0_msg1,
  byte1_msg1,
  byte2_msg1,
  byte3_msg1,
  byte4_msg1,
  byte5_msg1,
  cksum_msg1,
  endOfState_msg1,
} SERIAL_Frame_Msg1_State;

typedef enum
{
  idle_msg2,
  break_msg2,
  sync_msg2,
  message_id2,
  byte0_msg2,
  byte1_msg2,
  cksum_msg2,
  endOfState_msg2,
  
} SERIAL_Frame_Msg2_State;

typedef enum
{
  idle_msg3,
  sync_msg3,
  message_id3,
  byte0_msg3,
  byte1_msg3,
  byte2_msg3,
  byte3_msg3,
  byte4_msg3,
  byte5_msg3,
  byte6_msg3,
  byte7_msg3,
  cksum_msg3,
  endOfState_msg3,
} SERIAL_Frame_Msg3_State;
typedef void (*SERIAL_CMD_CALLBACK_Type)( u8 Received_Data );
typedef void (*SERIAL_COMPLETE_CALLBACK_Type)( u8 *g_SERIAL_data_Received_Buffer_AU8 );

typedef struct
{
  u8 data_buffer[8];
  u8 read_length;
  u8 message_checksum;
  SERIAL_RECEIVE_STATUS_ID receive_status;
  SERIAL_COMPLETE_CALLBACK_Type complete_callback;
} SERIAL_RECEIVE_MESSAGE_RAM_STRUCT;

typedef struct
{
  u8 data_buffer[2];
  u8 message_length;
  u8 message_checksum;
  SERIAL_TRANSMIT_STATUS_ID transmit_status;
  SERIAL_COMPLETE_CALLBACK_Type complete_callback;
} SERIAL_TRANSMIT_MESSAGE_RAM_STRUCT;

typedef struct
{
  u8 CMD_id;
  SERIAL_CMD_CALLBACK_Type callback;
} SERIAL_ID_LOOKUP_TABLE_STRUCT;

typedef struct
{
  u16 Received_Status;
  u16 Transmit_Status;
  u8 Received_Data;
  u8 Transmit_Data;
  u8 Transmitted_Data;
  u8 state;
  u8 byteswritten;
  u8 bytesread;
  u8 status;
} SERIAL_INT_STRUCT;


#define MAX_TX_BUF_SIZE 50u
#define MAX_RX_BUF_SIZE 50u

#define Clear_All_SERIAL_Status                    ( (u16)0x7F1F )


// IDs 0 to 63 for a total of 64
#define Maximum_SERIAL_ID                  ( (u8)63 )


#define SERIAL_STATE_Waiting_for_Break          1u
#define SERIAL_STATE_Waiting_for_Sync          2u
#define SERIAL_STATE_Waiting_for_ID          3u
#define SERIAL_STATE_Setup_Response          4u
#define SERIAL_STATE_Transmitting_Data                 5u
#define SERIAL_STATE_Receiving_Transmitted       6u
#define SERIAL_STATE_Receiving            7u

#define SERIAL_STATUS_NO_FAULT            0u
#define SERIAL_STATUS_RESET            1u
#define SERIAL_STATUS_RESERVED1            2u
#define SERIAL_STATUS_RESERVED2            3u
#define SERIAL_STATUS_TX_BIT_ERROR            4u
#define SERIAL_STATUS_CHECKSUM_ERROR         5u
#define SERIAL_STATUS_BYTEFIELD_FRAMING_ERROR            6u
#define SERIAL_STATUS_ID_PARITY_ERROR         7u


/* Control of serial array unit 0 input clock (SAU0EN) */
#define _00_SAU0_CLOCK_STOP         0x00U   /* stops supply of input clock */
#define _08_SAU0_CLOCK_SUPPLY         0x08U   /* supplies input clock */
/* Control of serial array unit 1 input clock (SAU1EN) */
#define _00_SAU1_CLOCK_STOP         0x00U   /* stops supply of input clock */
#define _10_SAU1_CLOCK_SUPPLY         0x10U   /* supplies input clock */

/*
   Peripheral Enable Register 0 (PER1)
*/
/* Control of serial array unit 1 input clock (SAU2EN) */
#define _00_SAU2_CLOCK_STOP         0x00U   /* stops supply of input clock */
#define _08_SAU2_CLOCK_SUPPLY         0x08U   /* supplies input clock */

/*
   Serial Clock Select Register m (SPSm)
*/
/* Operating mode and clear mode selection (PRSm03 - PRSm00) */
#define _0000_SAU_CK00_FCLK_0         0x0000U   /* CK00-fCLK */
#define _0001_SAU_CK00_FCLK_1         0x0001U   /* CK00-fCLK/2^1 */
#define _0002_SAU_CK00_FCLK_2         0x0002U   /* CK00-fCLK/2^2 */
#define _0003_SAU_CK00_FCLK_3         0x0003U   /* CK00-fCLK/2^3 */
#define _0004_SAU_CK00_FCLK_4         0x0004U   /* CK00-fCLK/2^4 */
#define _0005_SAU_CK00_FCLK_5         0x0005U   /* CK00-fCLK/2^5 */
#define _0006_SAU_CK00_FCLK_6         0x0006U   /* CK00-fCLK/2^6 */
#define _0007_SAU_CK00_FCLK_7         0x0007U   /* CK00-fCLK/2^7 */
#define _0008_SAU_CK00_FCLK_8         0x0008U   /* CK00-fCLK/2^8 */
#define _0009_SAU_CK00_FCLK_9         0x0009U   /* CK00-fCLK/2^9 */
#define _000A_SAU_CK00_FCLK_10         0x000AU   /* CK00-fCLK/2^10 */
#define _000B_SAU_CK00_FCLK_11         0x000BU   /* CK00-fCLK/2^11 */
#define _000F_SAU_CK00_INTTM23         0x000FU   /* CK00-INTTM23 */
/* Operating mode and clear mode selection (PRSm13 - PRSm10) */
#define _0000_SAU_CK01_FCLK_0         0x0000U   /* CK01-fCLK */
#define _0010_SAU_CK01_FCLK_1         0x0010U   /* CK01-fCLK/2^1 */
#define _0020_SAU_CK01_FCLK_2         0x0020U   /* CK01-fCLK/2^2 */
#define _0030_SAU_CK01_FCLK_3         0x0030U   /* CK01-fCLK/2^3 */
#define _0040_SAU_CK01_FCLK_4         0x0040U   /* CK01-fCLK/2^4 */
#define _0050_SAU_CK01_FCLK_5         0x0050U   /* CK01-fCLK/2^5 */
#define _0060_SAU_CK01_FCLK_6         0x0060U   /* CK01-fCLK/2^6 */
#define _0070_SAU_CK01_FCLK_7         0x0070U   /* CK01-fCLK/2^7 */
#define _0080_SAU_CK01_FCLK_8         0x0080U   /* CK01-fCLK/2^8 */
#define _0090_SAU_CK01_FCLK_9         0x0090U   /* CK01-fCLK/2^9 */
#define _00A0_SAU_CK01_FCLK_10         0x00A0U   /* CK01-fCLK/2^10 */
#define _00B0_SAU_CK01_FCLK_11         0x00B0U   /* CK01-fCLK/2^11 */
#define _00F0_SAU_CK01_INTTM23         0x00F0U   /* CK01-INTTM23 */

/*
   Serial Mode Register mn (SMRmn)
*/

#define   _0020_SAU_SMRMN_INITIALVALUE      0x0020U
/* Selection of macro clock (MCK) of channel n (CKSmn) */
#define _0000_SAU_CLOCK_SELECT_CK00      0x0000U   /* operation clock CK0 set by PRS register */ 
#define _8000_SAU_CLOCK_SELECT_CK01      0x8000U   /* operation clock CK1 set by PRS register */
/* Selection of transfer clock (TCLK) of channel n (CCSmn) */
#define _0000_SAU_CLOCK_MODE_CKS      0x0000U   /* divided operation clock MCK specified by CKSmn bit */  
#define _4000_SAU_CLOCK_MODE_TI0N      0x4000U   /* clock input from SCK pin (slave transfer in CSI mode) */
/* Selection of start trigger source (STSmn) */
#define _0000_SAU_TRIGGER_SOFTWARE      0x0000U   /* only software trigger is valid */
#define _0100_SAU_TRIGGER_RXD         0x0100U   /* valid edge of RXD pin */
/* Controls inversion of level of receive data of channel n in UART mode (SISmn0) */
#define _0000_SAU_EDGE_FALL         0x0000U   /* falling edge is detected as the start bit */
#define _0040_SAU_EDGE_RISING         0x0040U   /* rising edge is detected as the start bit */
/* Setting of operation mode of channel n (MDmn2, MDmn1) */
#define _0000_SAU_CSI            0x0000U   /* CSI mode */
#define _0002_SAU_UART            0x0002U   /* UART mode */
#define _0004_SAU_IIC            0x0004U   /* simplified IIC mode */
/* Selection of interrupt source of channel n (MDmn0) */
#define _0000_SAU_TRANSFER_END         0x0000U   /* transfer end interrupt */
#define _0001_SAU_BUFFER_EMPTY         0x0001U   /* buffer empty interrupt */

/*
   Serial Communication Operation Setting Register mn (SCRmn)
*/
/* Setting of operation mode of channel n (TXEmn, RXEmn) */
#define _0000_SAU_NOT_COMMUNICATION      0x0000U   /* does not start communication */
#define _4000_SAU_RECEPTION         0x4000U   /* reception only */
#define _8000_SAU_TRANSMISSION         0x8000U   /* transmission only */
#define _C000_SAU_RECEPTION_TRANSMISSION   0xC000U   /* reception and transmission */
/* Selection of data and clock phase in CSI mode (DAPmn, CKPmn) */
#define _0000_SAU_TIMING_1         0x0000U   /* type 1 */
#define _1000_SAU_TIMING_2         0x1000U   /* type 2 */
#define _2000_SAU_TIMING_3         0x2000U   /* type 3 */
#define _3000_SAU_TIMING_4         0x3000U   /* type 4 */
/* Setting of parity bit in UART mode (PTCmn1, PTCmn0) */
#define _0000_SAU_PARITY_NONE         0x0000U   /* none parity */
#define _0100_SAU_PARITY_ZERO         0x0100U   /* zero parity */
#define _0200_SAU_PARITY_EVEN         0x0200U   /* even parity */
#define _0300_SAU_PARITY_ODD         0x0300U   /* odd parity */
/* Selection of data transfer sequence in CSI and UART modes (DIRmn) */
#define _0000_SAU_MSB            0x0000U   /* MSB */
#define _0080_SAU_LSB            0x0080U   /* LSB */
/* Setting of stop bit in UART mode (SLCmn1, SLCmn0) */
#define _0000_SAU_STOP_NONE         0x0000U   /* none stop bit */
#define _0010_SAU_STOP_1         0x0010U   /* 1 stop bit */
#define _0020_SAU_STOP_2         0x0020U   /* 2 stop bits */
/* Setting of data length in CSI and UART modes (DLSmn3 - DLSmn0) */
#define _0006_SAU_LENGTH_7         0x0006U   /* 7-bit data length */
#define _0007_SAU_LENGTH_8         0x0007U   /* 8-bit data length */
#define _0008_SAU_LENGTH_9         0x0008U   /* 9-bit data length */
#define _0009_SAU_LENGTH_10         0x0009U   /* 10-bit data length */
#define _000A_SAU_LENGTH_11         0x000AU   /* 11-bit data length */
#define _000B_SAU_LENGTH_12         0x000BU   /* 12-bit data length */
#define _000C_SAU_LENGTH_13         0x000CU   /* 13-bit data length */
#define _000D_SAU_LENGTH_14         0x000DU   /* 14-bit data length */
#define _000E_SAU_LENGTH_15         0x000EU   /* 15-bit data length */
#define _000F_SAU_LENGTH_16         0x000FU   /* 16-bit data length */

/*
   Serial Output Level Register m (SOLm)
*/
/* Selects inversion of the level of the transmit data of channel n in UART mode */
#define _0000_SAU_CHANNEL0_NORMAL      0x0000U   /* normal bit level */
#define _0001_SAU_CHANNEL0_INVERTED      0x0001U   /* inverted bit level */
#define _0000_SAU_CHANNEL1_NORMAL      0x0000U   /* normal bit level */  
#define _0004_SAU_CHANNEL1_INVERTED      0x0004U   /* inverted bit level */

/*
   Noise Filter Enable Register 0 (NFEN0)
*/
/* Use of noise filter of RXD2/P43 pin (UNFEN2) */
#define _00_SAU_RXD2_FILTER_OFF         0x00U   /* noise filter off */
#define _10_SAU_RXD2_FILTER_ON         0x10U   /* noise filter on */
/* Use of noise filter of SSI01/P77 pin (SNFEN01) */
#define _00_SAU_SSI01_FILTER_OFF      0x00U   /* noise filter off */
#define _02_SAU_SSI01_FILTER_ON         0x02U   /* noise filter on */
/* Use of noise filter of SSI00/P30 or SSI00/P63 pin (SNFEN00) */
#define _00_SAU_SSI00_FILTER_OFF      0x00U   /* noise filter off */
#define _01_SAU_SSI00_FILTER_ON         0x01U   /* noise filter on */

/*
   Format of Serial Status Register mn (SSRmn)
*/
/* Communication status indication flag of channel n (TSFmn) */
#define _0040_SAU_UNDER_EXECUTE         0x0040U   /* communication is under execution */
/* Buffer register status indication flag of channel n (BFFmn) */
#define _0020_SAU_VALID_STORED         0x0020U   /* valid data is stored in the SDRmn register */
/* Framing error detection flag of channel n (FEFmn) */
#define _0004_SAU_FRAM_ERROR         0x0004U   /* a framing error occurs during UART reception */
/* Parity error detection flag of channel n (PEFmn) */
#define _0002_SAU_PARITY_ERROR         0x0002U   /* a parity error occurs during UART reception or ACK is not detected during I2C transmission */
/* Overrun error detection flag of channel n (OVFmn) */
#define _0001_SAU_OVERRUN_ERROR         0x0001U   /* an overrun error occurs */

/*
   Serial Channel Start Register m (SSm)
*/
/* Operation start trigger of channel 0 (SSm0) */
#define _0000_SAU_CH0_START_TRG_OFF      0x0000U   /* no trigger operation */
#define _0001_SAU_CH0_START_TRG_ON      0x0001U   /* sets SEm0 to 1 and enters the communication wait status */
/* Operation start trigger of channel 1 (SSm1) */
#define _0000_SAU_CH1_START_TRG_OFF      0x0000U   /* no trigger operation */
#define _0002_SAU_CH1_START_TRG_ON      0x0002U   /* sets SEm1 to 1 and enters the communication wait status */

/*
   Serial Channel Stop Register m (STm)
*/
/* Operation stop trigger of channel 0 (STm0) */
#define _0000_SAU_CH0_STOP_TRG_OFF      0x0000U   /* no trigger operation */
#define _0001_SAU_CH0_STOP_TRG_ON      0x0001U   /* operation is stopped (stop trigger is generated) */
/* Operation stop trigger of channel 1 (STm1) */
#define _0000_SAU_CH1_STOP_TRG_OFF      0x0000U   /* no trigger operation */
#define _0002_SAU_CH1_STOP_TRG_ON      0x0002U   /* operation is stopped (stop trigger is generated) */

/*
   Format of Serial Flag Clear Trigger Register mn (SIRmn)
*/
/* Clear trigger of overrun error flag of channel n (OVCTmn) */
#define   _0001_SAU_SIRMN_OVCTMN         0x0001U
/* Clear trigger of parity error flag of channel n (PECTmn) */
#define   _0002_SAU_SIRMN_PECTMN         0x0002U
/* Clear trigger of framing error of channel n (FECTmn) */
#define   _0004_SAU_SIRMN_FECTMN         0x0004U

/*
   Serial Output Enable Register m (SOEm)
*/
/* Serial output enable/disable of channel 0 (SOEm0) */
#define _0000_SAU_CH0_OUTPUT_DISABLE      0x0000U   /* disable output by serial communication operation */
#define _0001_SAU_CH0_OUTPUT_ENABLE      0x0001U   /* enables output by serial communication operation */
/* Serial output enable/disable of channel 1 (SOEm1) */
#define _0000_SAU_CH1_OUTPUT_DISABLE      0x0000U   /* disable output by serial communication operation */
#define _0002_SAU_CH1_OUTPUT_ENABLE      0x0002U   /* enables output by serial communication operation */

/*
   Serial Output Register m (SOm)
*/
/* Serial data output of channel 0 (SOm0) */
#define _0000_SAU_CH0_DATA_OUTPUT_0      0x0000U   /* serial data output value is "0" */
#define _0001_SAU_CH0_DATA_OUTPUT_1      0x0001U   /* serial data output value is "1" */
/* Serial data output of channel 1 (SOm1) */
#define _0000_SAU_CH1_DATA_OUTPUT_0      0x0000U   /* serial data output value is "0" */
#define _0002_SAU_CH1_DATA_OUTPUT_1      0x0002U   /* serial data output value is "1" */
/* Serial clock output of channel 0 (CKOm0) */
#define _0000_SAU_CH0_CLOCK_OUTPUT_0      0x0000U   /* serial clock output value is "0" */
#define _0100_SAU_CH0_CLOCK_OUTPUT_1      0x0100U   /* serial clock output value is "1" */
/* Serial clock output of channel 1 (CKOm1) */
#define _0000_SAU_CH1_CLOCK_OUTPUT_0      0x0000U   /* serial clock output value is "0" */
#define _0200_SAU_CH1_CLOCK_OUTPUT_1      0x0200U   /* serial clock output value is "1" */

/*
   Serial Slave Select Function Enable Register (SSE0)
*/
#define   _0006_UARTF0_K_VALUE         0x0006U
 /* Channel 0 SSI00 input setting in CSI communication and slave mode (SSE00) */
#define _00_SSI00_PIN_DISABLE         0x00U   /* disables SSI00 pin input */
#define _01_SSI00_PIN_ENABLE         0x01U   /* enables SSI00 pin input */
/* Channel 0 SSI01 input setting in CSI communication and slave mode (SSE01) */
#define _00_SSI01_PIN_DISABLE         0x00U   /* disables SSI01 pin input */
#define _02_SSI01_PIN_ENABLE         0x02U   /* enables SSI01 pin input */

/* Simple IIC used flag */
#define _00_SAU_IIC_MASTER_FLAG_CLEAR      0x00U
#define _01_SAU_IIC_SEND_FLAG         0x01U
#define _02_SAU_IIC_RECEIVE_FLAG      0x02U
#define _04_SAU_IIC_SENDED_ADDRESS_FLAG      0x04U

/*
   UARTFn control register 0 (UFnCTL0)
*/
#define   _10_UARTF_UFNCTL0_INITIALVALUE      0x10U
/* Transmission operation enable (UFnTXE) */
#define   _00_UARTF_TRANSMISSION_DISABLE      0x00U   /* disable transmission operation */
#define   _40_UARTF_TRANSMISSION_ENABLE      0x40U   /* enable transmission operation */
/* Reception operation enable (UFnRXE) */
#define   _00_UARTF_RECEPTION_DISABLE      0x00U   /* disable reception operation */
#define   _20_UARTF_RECEPTION_ENABLE      0x20U   /* enable reception operation */
/* Transfer direction selection (UFnDIR) */
#define   _00_UARTF_TRANSFDIR_MSB         0x00U   /* MSB-first transfer */
#define   _10_UARTF_TRANSFDIR_LSB         0x10U   /* LSB-first transfer */
/* Parity selection during transmission/reception (UFnPS1, UFnPS0) */
#define   _00_UARTF_PARITY_NONE         0x00U   /* no parity */
#define   _04_UARTF_PARITY_ZREO         0x04U   /* 0 parity */
#define   _08_UARTF_PARITY_ODD         0x08U   /* odd parity */
#define   _0C_UARTF_PARITY_EVEN         0x0CU   /* even parity */
/* Specification of data character length of 1 frame of transmit/receive data (UFnCL) */
#define   _00_UARTF_DATALENGTH_7BIT      0x00U   /* 7 bits */
#define   _02_UARTF_DATALENGTH_8BIT      0x02U   /* 8 bits */
/* Specification of length of stop bit for transmit data (UFnSL) */
#define   _00_UARTF_STOPLENGTH_1BIT      0x00U   /* 1 bit */
#define   _01_UARTF_STOPLENGTH_2BIT      0x01U   /* 2 bits */

/*
   UARTFn control register (UFnCTL1)
*/
/* Prescaler clock frequency division value (UFnPRS2 - UFnPRS0)*/
#define   _0000_UARTF_BASECLK_1         0x0000U   /* fXX */
#define   _2000_UARTF_BASECLK_2         0x2000U   /* fXX/2^1 */
#define   _4000_UARTF_BASECLK_4         0x4000U   /* fXX/2^2 */
#define   _6000_UARTF_BASECLK_8         0x6000U   /* fXX/2^3 */
#define   _8000_UARTF_BASECLK_16         0x8000U   /* fXX/2^4 */
#define   _A000_UARTF_BASECLK_32         0xA000U   /* fXX/2^5 */
#define   _C000_UARTF_BASECLK_64         0xC000U   /* fXX/2^6 */
#define   _E000_UARTF_BASECLK_128         0xE000U   /* fXX/2^7 */

/*
   UARTFn option control register 0 (UFnOPT0)
*/
#define   _14_UARTF_UFNOPT0_INITIALVALUE      0x14U
/* Transmit data level bit (UFnTDL) */
#define   _00_UARTF_TRAN_DATALEVEL_NORMAL      0x00U   /* normal output of transfer data */
#define   _02_UARTF_TRAN_DATALEVEL_INVERTED   0x02U   /* inverted output of transfer data */
/* Receive data level bit (UFnRDL) */
#define   _00_UARTF_REC_DATALEVEL_NORMAL      0x00U   /* normal input of transfer data */
#define   _01_UARTF_REC_DATALEVEL_INVERTED   0x01U   /* inverted input of transfer data */

/*
   UARTFn option control register 1 (UFnOPT1)
*/
/* Transmit data expansion bit enable bit (UFnEBE) */
#define   _00_UARTF_EXPANSIONBIT_UNUSE      0x00U   /* disable expansion bit */
#define   _80_UARTF_EXPANSIONBIT_USE      0x80U   /* enable expansion bit */
/* Transmit data expansion bit detection level (UFnEBL) */
#define   _00_UARTF_EXPANSIONBIT_VALUE_0      0x00U   /* expansion value 0 */
#define   _40_UARTF_EXPANSIONBIT_VALUE_1      0x40U   /* expansion value 1 */
/* Transmit data expansion bit data comparsion enable bit (UFnEBC) */
#define   _00_UARTF_EXPANSIONBIT_COMP_UNUSE   0x00U   /* disable expansion bit with comparsion */
#define   _20_UARTF_EXPANSIONBIT_COMP_USE      0x20U   /* enable expansion bit with comparsion */
/* Communication mode (UFnMD1, UFnMD0) */
#define   _00_UARTF_NORMAL_MODE         0x00U   /* normal mode */

/*
   UARTFn option control register 1 (UFnSTC)
*/
/* Normal error flag clear trigger (UFnOVE, UFnFE, UFnPE) */
#define   _0007_UARTF_COMMONERROE_CLEAR      0x0007U   /* clear commom error flag bit */
/* Buffer transmission/reception completion flag clear trigger (UFnCLBUC) */
#define   _0400_UARTF_BUC_CLEAR         0x0400U   /* clear buffer transmission/reception completion flag bit */
/* Expansion bit detection flag clear trigger (UFnCLEBD) */
#define   _0100_UARTF_EBD_CLEAR         0x0100U   /* clear EBD flag */
/* ID match flag clear trigger (UFnCLIDM) */
#define   _0200_UARTF_IDM_CLEAR         0x0200U   /* clear IDM flag */

/*
   UARTFn option control register 2 (UFnOPT2)
*/
/* Bit to select use of receive data noise filter (UFnRXFL) */
#define   _00_UARTF_DATA_NOISE_FILTER_USED   0x00U   /* enables noise filter */
#define   _02_UARTF_DATA_NOISE_FILTER_UNUSED   0x02U   /* disables noise filter */
/* Transmission interrupt (INTLTn) generation timing select bit (UFnITS) */
#define   _00_UARTF_LT_INT_GENTIME_0      0x00U   /* outputs transmission interrupt request upon transmission start */
#define   _01_UARTF_LT_INT_GENTIME_1      0x01U   /* outputs transmission interrupt request upon transmission completion */

/*
   UARTFn buffer control register (UFnBUCTL)
*/
/* Buffer length bits (UFnBUL3~UFnBUL0) */
#define   _0001_UARTF_BUFFER_LENGTH_1      0x0001U   /* buffer length 1 byte */
#define   _0002_UARTF_BUFFER_LENGTH_2      0x0002U   /* buffer length 2 bytes */
#define   _0003_UARTF_BUFFER_LENGTH_3      0x0003U   /* buffer length 3 bytes */
#define   _0004_UARTF_BUFFER_LENGTH_4      0x0004U   /* buffer length 4 bytes */
#define   _0005_UARTF_BUFFER_LENGTH_5      0x0005U   /* buffer length 5 bytes */
#define   _0006_UARTF_BUFFER_LENGTH_6      0x0006U   /* buffer length 6 bytes */
#define   _0007_UARTF_BUFFER_LENGTH_7      0x0007U   /* buffer length 7 bytes */
#define   _0008_UARTF_BUFFER_LENGTH_8      0x0008U   /* buffer length 8 bytes */
#define   _0009_UARTF_BUFFER_LENGTH_9      0x0009U   /* buffer length 9 bytes */
/* Buffer transmission request bit (UFnTRQ) */
#define   _0000_UARTF_BUFFER_TRAN_START_NOREQUEST   0x0000U   /* no transmission start request */
#define   _0010_UARTF_BUFFER_TRAN_START_REQUEST   0x0010U   /* transmission start request */

/* Buffer address */
#define UARTF0_BUFFER_ADDRESSS         0x024FU   /* UARTF0 transmit buffer address in buffer mode */
#define UARTF1_BUFFER_ADDRESSS         0x026FU   /* UARTF1 transmit buffer address in buffer mode */
/*
*******************************************************************************
**  Macro define
*******************************************************************************
*/
#define   _9E00_IIC11_DIVISOR         0x9E00U
#define IIC11_WAITTIME            (10U)      /* change the waiting time according to the system */
/* Selection of 16-bit counter output clock (UF1BRS7 - UF1BRS0) */
#define   _01A1_UARTF1_K_VALUE         0x01A1U

extern u8  gIic11MasterStatusFlag;   /* iic11 start flag for send address check by master mode */
extern u8  *gpIic11TxAddress;   /* iic11 send data pointer by master mode */
extern u16 gIic11TxCnt;      /* iic11 send data size by master mode */
extern u8  *gpIic11RxAddress;   /* iic11 receive data pointer by master mode */
extern u16 gIic11RxCnt;      /* iic11 receive data size by master mode */
extern u16 gIic11RxLen;      /* iic11 receive data length by master mode */

extern u8  gpUARTF1TxBuffer[MAX_TX_BUF_SIZE];   /* UARTF1 transmit data address */
extern u16 gUARTF1Quotient;      /* UARTF1 transmit buffer data count */
extern u16 gUARTF1Remainder;   /* UARTF1 transmit buffer data remainer */
extern u16 gUARTF1TxCnt;      /* UARTF1 transmit data number */
extern u8  *gpUARTF1RxAddress;   /* UARTF1 receive data address */
extern u16 gUARTF1RxCnt;      /* UARTF1 receive data number */
extern u16 gUARTF1RxLen;      /* UARTF1 receive data length */
extern u8 gUARTF1IndxCnt;
extern u8 Tx_Flag;
extern u8 Q_busy;

extern u8 usart_rx_data[MAX_RX_BUF_SIZE];
/******************************************************************************
**                 G L O B A L   F U N C T I O N S
******************************************************************************/
/*list all global functions*/
void UARTF1_Init(void);
void UARTF1_Start(void);
void UARTF1_Stop(void);
extern void USART1_Rx_Interrupt(u8 data);
void UARTF1_SendEndCallback(void);
void UARTF1_ReceiveEndCallback(void);
void UARTF1_SoftOverRunCallback(void);
void UARTF1_ErrorCallback(u8 err_type);
extern bool UARTF1_Store_Data_In_Q(u8 *txbuf, u16 txnum);
extern void UARTF1_SendData_From_Q(void);
extern u8 Calculate_Checksum(u8 * const buffer, const u8 length);
void UARTF0_Init(void);
void UARTF0_Start(void);
void UARTF0_Stop(void);


#endif


/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
**
** Revision No          :
** Date(dd/mm/yy)       : 
** Author               :
** Description of change: 
**
*******************************************************************************/

