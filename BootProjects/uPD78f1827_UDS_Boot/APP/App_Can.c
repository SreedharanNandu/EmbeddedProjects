/**************************************************************************
 *       #include's
 *************************************************************************/

#include "App_Can.h"
#include "Eal_Can.h"
#include "HAL_PortPin_API.h"
#include "HAL_Timer_API.h"
#include "string.h"


/******************************
* Defines
*******************************/
#define MAX_1S_DELAY                            800000L

/*  Configuration Switch to enable CAN transmission & reception */

/**************************************************************************
 *       file scopic declarations
 *************************************************************************/

/**************************************************************************
 *       global data
 *************************************************************************/

/**************************************************************************
 *       local prototypes
 *************************************************************************/
const CAN_message_parameter_T tx_can_list[] =
{

 /* ID           MSG    EXT_ID    NOTIFIER FUNCTION,     MSG TYPE          BUF
                 LEN                                     DEPTH                */
   {CAN_TX_ID,   8,    0,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
};



/**************************************************************************
 *  global variables
 *************************************************************************/







/*.***************************************************************************
*. Name:Get_Transmit_Message_Number_From_Message_ID
*.
*.   Description:
*.      All CAN functions except this one and the related receive version
*.      take a message number as an
*.      argument instead of the message ID.  If the user has a message
*.      ID and does not know the correct message number to use, then
*.      this procedure will return the correct message number.
*.
*.   Warning: None
*.
*. Shared Variables: tx_can_list
*.
*. Parameters: Message ID
*.
*. Return Value: message_number
*.
*.
******************************************************************************/
u8 App_Get_Tx_Msg_No( u32 Message_ID)
{
   u8 tx_msg_no;
   u8 message_number;

   u8 msg_found = FALSE;

   for( tx_msg_no = 0;tx_msg_no <  CAN_NUMBER_OF_TRANSMIT_MESSAGES;tx_msg_no++ )
   {
      if(TX_MSG_ID == Message_ID )
      {
         message_number = tx_msg_no;
         tx_msg_no = CAN_NUMBER_OF_TRANSMIT_MESSAGES;
         msg_found = TRUE;
      }
   }
   if((CAN_NUMBER_OF_TRANSMIT_MESSAGES == tx_msg_no) && (!msg_found) )
   {
      message_number =  MESSAGE_ID_NOT_IN_TABLE;
   }

   return( message_number );
}

/******************************************************************************
* Changes:
*
* Date      By   Description
* -------- ---  ---------------------------------------------------------------
*
*
*
******************************************************************************/
