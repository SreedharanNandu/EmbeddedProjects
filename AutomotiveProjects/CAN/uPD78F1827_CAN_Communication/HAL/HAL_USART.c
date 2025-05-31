/******************************************************************************
*******************************************************************************
** Project Name         :
** File Name            : HAL_USART.c
** Description          : This module is used to define USART driver
** Date                 : 
** Hardware Platform    : 
******************************************************************************/


/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
#include <stdio.h>
#include <ctype.h> 
#include <stdlib.h>
#include <string.h>




/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/
#include <io78f1827_48.h>
#include <io78f1827_48_ext.h>
#include "system_def.h"
#include "HAL_USART.h"
#include "INTF_VTMR_API.h"
#include "system_def.h"
#include "APP_Serial.h"


/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
/* Function Prototypes */
/* local macro declarations */


/******************************************************************************
**               D A T A
******************************************************************************/
/* local variables  */
u8  gIic11MasterStatusFlag;   /* iic11 start flag for send address check by master mode */
u8  *gpIic11TxAddress;   /* iic11 send data pointer by master mode */
u16 gIic11TxCnt;      /* iic11 send data size by master mode */
u8  *gpIic11RxAddress;   /* iic11 receive data pointer by master mode */
u16 gIic11RxCnt;      /* iic11 receive data size by master mode */
u16 gIic11RxLen;      /* iic11 receive data length by master mode */
u8  gpUARTF1TxBuffer[MAX_TX_BUF_SIZE];   /* UARTF1 transmit data address */
u16 gUARTF1Quotient;      /* UARTF1 transmit buffer data count */
u16 gUARTF1Remainder;   /* UARTF1 transmit buffer data remainer */
u16 gUARTF1TxCnt;      /* UARTF1 transmit data number */
u8  *gpUARTF1RxAddress;   /* UARTF1 receive data address */
u8 gUARTF1IndxCnt;
u8 Tx_Flag;
u8 Q_busy;


u8 usart_rx_data[MAX_RX_BUF_SIZE];
volatile USART_WAIT_STATE state;
/******************************************************************************
**                  F U N C T I O N     D E F I N I T I O N S
******************************************************************************/
/*<list all function definitins here>  */
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function initializes the UARTF0 module.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void UARTF0_Init(void)
{
   LIN0EN = 1U;
   UF0CTL0 &= (u8)(~_40_UARTF_TRANSMISSION_ENABLE & ~_20_UARTF_RECEPTION_ENABLE);   /* disable UARTF0 operation */
   LTMK0 = 1U;   /* disable INTLT0 interrupt */
   LTIF0 = 0U;   /* clear INTLT0 interrupt flag */
   LRMK0 = 1U;   /* disable INTLR0 interrupt */
   LRIF0 = 0U;   /* clear INTLR0 interrupt flag */
   LSMK0 = 1U;   /* disable INTLS0 interrupt */
   LSIF0 = 0U;   /* clear INTLS0 interrupt flag */
   /* Set INTLT0 low priority */
   LTPR10 = 0U;
   LTPR00 = 0U;
   /* Set INTLR0 low priority */
   LRPR10 = 0U;
   LRPR00 = 0U;
   /* Set INTLS0 low priority */
   LSPR10 = 1U;
   LSPR00 = 1U;
   UF0CTL1 = 0x8019;//_E000_UARTF_BASECLK_128 | _0006_UARTF0_K_VALUE;
   UF0OPT0 = _14_UARTF_UFNOPT0_INITIALVALUE | _00_UARTF_TRAN_DATALEVEL_NORMAL | _00_UARTF_REC_DATALEVEL_NORMAL;
   UF0OPT1 = _00_UARTF_EXPANSIONBIT_UNUSE | _00_UARTF_NORMAL_MODE;
   UF0OPT2 = _01_UARTF_LT_INT_GENTIME_1 | _00_UARTF_DATA_NOISE_FILTER_USED;
   UF0CTL0 = _10_UARTF_TRANSFDIR_LSB | _00_UARTF_PARITY_NONE | _02_UARTF_DATALENGTH_8BIT | _00_UARTF_STOPLENGTH_1BIT;
   
   /* Set LTXD0 pin */
   P1 |= 0x08U;
   PM1 &= 0xF7U;
   /* Set LRXD0 pin */
   PM1 |= 0x10U;
   UARTF0_Start();
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function starts the UARTF0 operation.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void UARTF0_Start(void)
{
   LTIF0 = 0U;   /* clear INTLT0 interrupt flag */
   LTMK0 = 0U;   /* enable INTLT0 interrupt */
   LRIF0 = 0U;   /* clear INTLR0 interrupt flag */
   LRMK0 = 0U;   /* enable INTLR0 interrupt */
   LSIF0 = 0U;   /* clear INTLS0 interrupt flag */
   LSMK0 = 0U;   /* enable INTLS0 interrupt */
   UF0CTL0 |= _40_UARTF_TRANSMISSION_ENABLE | _20_UARTF_RECEPTION_ENABLE;   /* enable UARTF0 operation */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function stops the UARTF0 operation.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void UARTF0_Stop(void)
{
   UF0CTL0 &= (u8)(~_40_UARTF_TRANSMISSION_ENABLE & ~_20_UARTF_RECEPTION_ENABLE);   /* disable UARTF0 operation */
   LTMK0 = 1U;   /* disable INTLT0 interrupt */
   LTIF0 = 0U;   /* clear INTLT0 interrupt flag */
   LRMK0 = 1U;   /* disable INTLR0 interrupt */
   LRIF0 = 0U;   /* clear INTLR0 interrupt flag */
   LSMK0 = 1U;   /* disable INTLS0 interrupt */
   LSIF0 = 0U;   /* clear INTLS0 interrupt flag */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function initializes the UARTF1 module.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void UARTF1_Init(void)
{
   LIN1EN = 1U;
   UF1CTL0 &= (u8)(~_40_UARTF_TRANSMISSION_ENABLE & ~_20_UARTF_RECEPTION_ENABLE);   /* disable UARTF1 operation */
   LTMK1 = 1U;   /* disable INTLT1 interrupt */
   LTIF1 = 0U;   /* clear INTLT1 interrupt flag */
   LRMK1 = 1U;   /* disable INTLR1 interrupt */
   LRIF1 = 0U;   /* clear INTLR1 interrupt flag */
   LSMK1 = 1U;   /* disable INTLS1 interrupt */
   LSIF1 = 0U;   /* clear INTLS1 interrupt flag */
   /* Set INTLT1 low priority */
   LTPR11 = 1U;
   LTPR01 = 1U;
   /* Set INTLR1 low priority */
   LRPR11 = 1U;
   LRPR01 = 1U;
   /* Set INTLS1 low priority */
   LSPR11 = 1U;
   LSPR01 = 1U;
   UF1CTL1 = _E000_UARTF_BASECLK_128 | _0006_UARTF0_K_VALUE;
   UF1OPT0 = _14_UARTF_UFNOPT0_INITIALVALUE | _00_UARTF_TRAN_DATALEVEL_NORMAL | _00_UARTF_REC_DATALEVEL_NORMAL;
   UF1OPT1 = _00_UARTF_EXPANSIONBIT_UNUSE | _00_UARTF_NORMAL_MODE;
   UF1OPT2 = _01_UARTF_LT_INT_GENTIME_1 | _02_UARTF_DATA_NOISE_FILTER_UNUSED;
   UF1CTL0 = _10_UARTF_TRANSFDIR_LSB | _00_UARTF_PARITY_NONE | _02_UARTF_DATALENGTH_8BIT | _00_UARTF_STOPLENGTH_1BIT;
   STSEL &= 0x7FU;
   /* Set LTXD1 pin */
   P1 |= 0x01U;
   PM1 &= 0xFEU;
   /* Set LRXD1 pin */
   PM1 |= 0x02U;

   UARTF1_Start();
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function starts the UARTF1 operation.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void UARTF1_Start(void)
{
   LTIF1 = 0U;   /* clear INTLT1 interrupt flag */
   LTMK1 = 0U;   /* enable INTLT1 interrupt */
   LRIF1 = 0U;   /* clear INTLR1 interrupt flag */
   LRMK1 = 0U;   /* enable INTLR1 interrupt */
   LSIF1 = 0U;   /* clear INTLS1 interrupt flag */
   LSMK1 = 0U;   /* enable INTLS1 interrupt */
   UF1CTL0 |= _40_UARTF_TRANSMISSION_ENABLE | _20_UARTF_RECEPTION_ENABLE;   /* enable UARTF1 operation */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function stops the UARTF1 operation.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void UARTF1_Stop(void)
{
   UF1CTL0 &= (u8)(~_40_UARTF_TRANSMISSION_ENABLE & ~_20_UARTF_RECEPTION_ENABLE);   /* disable UARTF1 operation */
   LTMK1 = 1U;   /* disable INTLT1 interrupt */
   LTIF1 = 0U;   /* clear INTLT1 interrupt flag */
   LRMK1 = 1U;   /* disable INTLR1 interrupt */
   LRIF1 = 0U;   /* clear INTLR1 interrupt flag */
   LSMK1 = 1U;   /* disable INTLS1 interrupt */
   LSIF1 = 0U;   /* clear INTLS1 interrupt flag */
}





/******************************************************************************/
u8 Calculate_Checksum( u8 * const buffer,const u8 length )
{
  unsigned int checksum=0;
  
  // Process the received character
  //-----------------------------------------------------------------
  
  //
  // A Checksum Error shall be detected if the inverted modulo-256 sum
  // over all received data bytes and the protected identifier (when using
  // enhanced checksum) and the received checksum byte field does not
  // result in $FF.
  // Calculate checksum using the received data
  for (int i = 0u; i < length; i++)
  {
    checksum = checksum + (unsigned short)buffer[i];
    checksum = ( (unsigned char)checksum + ( checksum >> 8 ) );
  }
  return ((u8)checksum);
}



/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
**
** Revision No          :
** Date(dd/mm/yy)       : 
** Author               :
** Description of change:
*******************************************************************************/

