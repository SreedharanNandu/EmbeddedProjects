
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
#define UPA_ALIVE_RESET_COUNT_3S                3u
#define MAX_1S_DELAY                            800000L

/*  Configuration Switch to enable CAN transmission & reception */

/**************************************************************************
 *       file scopic declarations
 *************************************************************************/

/**************************************************************************
 *       global data
 *************************************************************************/
Gen_Status_T General_Status;
Chime_T Chime_Command;
Rear_Status_T Rear_Status;
/**************************************************************************
 *       local prototypes
 *************************************************************************/




const CAN_message_parameter_T tx_can_list[] =
{

 /* ID           MSG    EXT_ID    NOTIFIER FUNCTION,     MSG TYPE          BUF
                 LEN                                     DEPTH                */
   {CAN_ID_3AA,   3,    1,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
   {CAN_ID_121,   1,    1,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
   {CAN_ID_115,   8,    1,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
   {CAN_ID_124,   7,    1,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
   {CAN_ID_165,   8,    1,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
   {CAN_ID_167,   8,    1,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
   {CAN_ID_166,   8,    1,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
   {CAN_ID_108,   8,    1,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
   {CAN_ID_56C,   8,    1,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
   {CAN_ID_574,   8,    1,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
   {CAN_ID_122,   1,    1,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1},
   {CAN_ID_62C,   8,    0,        NULL_NOTIFIER_PTR,     FIFO_CAN_MESSAGE,   1}
};



/**************************************************************************
 *  global variables
 *************************************************************************/

/************************
* Function Definitions
*************************/
u8 App_Get_Tx_Msg_No( u32 Message_ID);

/*.****************************************************************************
*. Name: CAN_Data_Transmit_1s()
*.   This function sets the elements of the current_message_data array to the
*.   appropriate values and transmits it every 10ms.
*.
*. Parameters: None
*.
*. Return Value: None
*.
*. Shared Variables: current_message_data
*.
*. Design Information:

******************************************************************************/

void App_CAN_Data_Transmit_1s( void )
{
   u8 current_message_data[8] = {0,0,0,0,0,0,0,0};
   u8 test_msg_number=0;

   //vnmf
   current_message_data[0] = 1;
   current_message_data[1] = 0xFF;
   current_message_data[2] = 0;
   current_message_data[3] = 0;
   current_message_data[4] = 0;
   current_message_data[5] = 0;
   current_message_data[6] = 0;
   current_message_data[7] = 0;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_62C);
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );

   //odi action ipc
   current_message_data[0] = 0x03;
   current_message_data[1] = 0x0D;
   current_message_data[2] = 0x1A;
   current_message_data[3] = 0x1A;
   current_message_data[4] = 0x19;
   current_message_data[5] = 0x99;
   current_message_data[6] = 0x99;
   current_message_data[7] = 0x9A;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_56C);
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );

   //odidyndatamulti
   current_message_data[0] = 0x00;
   current_message_data[1] = 0x00;
   current_message_data[2] = 0x00;
   current_message_data[3] = 0x00;
   current_message_data[4] = 0x00;
   current_message_data[5] = 0x00;
   current_message_data[6] = 0x00;
   current_message_data[7] = 0x00;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_574);
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );


}  /* End function CAN_Data_Transmit_1s() */


/*.****************************************************************************
*. Name: CAN_Data_Transmit_500ms()
*.   This function sets the elements of the current_message_data array to the
*.   appropriate values and transmits it every 10ms.
*.
*. Parameters: None
*.
*. Return Value: None
*.
*. Shared Variables: current_message_data
*.
*. Design Information:

******************************************************************************/
void App_CAN_Data_Transmit_500ms( void )
{

   u8 current_message_data[8] = {0,0,0,0,0,0,0,0};
   u8 test_msg_number=0;
   

   //power mode info
   current_message_data[0] = 0;
   current_message_data[1] = 2;
   current_message_data[2] = 0;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_3AA);//message will see as 0x754000 i.e shift 3AA<<13
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );

   //system power mode
   current_message_data[0] = 2;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_121);//message will see as 0x242000 i.e shift 121<<13
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );

   //backup system power mode
   current_message_data[0] = 6;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_122);//message will see as 0x244000 i.e shift 122<<13
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );
 


}
/*.****************************************************************************
*. Name: CAN_Data_Transmit_250ms()
*.   This function sets the elements of the current_message_data array to the
*.   appropriate values and transmits it every 10ms.
*.
*. Parameters: None
*.
*. Return Value: None
*.
*. Shared Variables: current_message_data
*.
*. Design Information:

******************************************************************************/
void App_CAN_Data_Transmit_250ms( void )
{
   u8 current_message_data[8] = {0,0,0,0,0,0,0,0};
   u8 test_msg_number=0;
   

   //engine info 1
   current_message_data[0] = 1;
   current_message_data[1] = 0x50;
   current_message_data[2] = 0x40;
   current_message_data[3] = 1;
   current_message_data[4] = 0x0D;
   current_message_data[5] = 0x0D;
   current_message_data[6] = 0x65;
   current_message_data[7] = 0x02;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_165);
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );

   //veh speed
   current_message_data[0] = 0x0C;
   current_message_data[1] = 0xCC;
   current_message_data[2] = 0;
   current_message_data[3] = 0;
   current_message_data[4] = 0x0C;
   current_message_data[5] = 0xCC;
   current_message_data[6] = 0x00;
   current_message_data[7] = 0x00;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_108);
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );


}


/*.****************************************************************************
*. Name: CAN_Data_Transmit_100ms()
*.   This function sets the elements of the current_message_data array to the
*.   appropriate values and transmits it every 10ms.
*.
*. Parameters: None
*.
*. Return Value: None
*.
*. Shared Variables: current_message_data
*.
*. Design Information:

******************************************************************************/
void App_CAN_Data_Transmit_100ms( void )
{

   u8 current_message_data[8] = {0,0,0,0,0,0,0,0};
   u8 test_msg_number=0;

   //chassis info 
   current_message_data[0] = 0;
   current_message_data[1] = 0xF1;
   current_message_data[2] = 0;
   current_message_data[3] = 0;
   current_message_data[4] = 0;
   current_message_data[5] = 0;
   current_message_data[6] = 0;
   current_message_data[7] = 0;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_115);
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );

   //battery volt 
   current_message_data[0] = 0;
   current_message_data[1] = 0;
   current_message_data[2] = 0x70;
   current_message_data[3] = 0;
   current_message_data[4] = 0;
   current_message_data[5] = 0x0d;
   current_message_data[6] = 0x5;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_124);
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );

   //engine info 2 
   current_message_data[0] = 0;
   current_message_data[1] = 1;
   current_message_data[2] = 0xc4;
   current_message_data[3] = 0x27;
   current_message_data[4] = 0x34;
   current_message_data[5] = 0x41;
   current_message_data[6] = 0x06;
   current_message_data[6] = 0xa0;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_166);
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );

   //engine info 3 
   current_message_data[0] = 0x01;
   current_message_data[1] = 0x30;
   current_message_data[2] = 0;
   current_message_data[3] = 0;
   current_message_data[4] = 0;
   current_message_data[5] = 0x06;
   current_message_data[6] = 0xa0;
   test_msg_number = App_Get_Tx_Msg_No(CAN_ID_167);
   EAL_CAN_Tx_Msg( test_msg_number, current_message_data );
}
  /* End function CAN_Data_Transmit_100ms() */


/*.***************************************************************************
*. Name:App_Get_Rx_Msg_No
*.
*.   Description:
*.
*.   Warning: None
*.
*. Shared Variables: 
*.
*. Parameters: Message ID
*.
*. Return Value: 
*.
*.
******************************************************************************/
void App_Get_Rx_Msg_No(u32 Msg_ID, bool is_ext_frm ,u8 DLC,u8 *data)
{
   switch(Msg_ID)
   {
      case CAN_ID_20E:
           break;
      case CAN_ID_20C:
           break;
      case CAN_ID_396:
           break;
       case CAN_ID_1D0:
           memcpy((u8*)&General_Status.byte,data,DLC);
           break;
      case CAN_ID_200:
           memcpy((u8*)&Chime_Command.byte,data,DLC);
           break;
      case CAN_ID_1D4:
           memcpy((u8*)&Rear_Status.byte,data,DLC);
           break;
      default:
           break;
   }
}

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
   static u8 tx_msg_no;
   static u8 message_number;

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

#if (GMLAN_ENABLED == TRUE)
/*.****************************************************************************
*. Name: Trcv_Mode_Delay_1s()
*.
*. Parameters: None
*.
*. Return Value: None
*.
*. Shared Variables: 
*.
*. Design Information:

******************************************************************************/
void App_Trcv_Mode_HVWU_To_Normal(void)
{
   unsigned long delay = MAX_1S_DELAY;
   
   Trcv_Mode(HV_WAKEUP_MODE);
   while(delay--);
   Trcv_Mode(HS_MODE);

}

/****************************************************************************
*. Name:Trcv_Mode
*.
*.   Description:
*.
*.   Warning: None
*.
*. Shared Variables: 
*.
*. Parameters: Message ID
*.
*. Return Value: 
*.
*.
******************************************************************************/
void Trcv_Mode(Mode_T mode)
{
  switch(mode)
  {
     case SLEEP_MODE:
         break; 
     case HS_MODE:
         break; 
     case HV_WAKEUP_MODE:
          NFC_PROGM_ALT_MODE_0(1);     //mode 1 of SWCAN Transceiver 
          WAKE_UP_FIDM_ALT_MODE_1(0);  //mode 0 of SWCAN Transceiver
         break; 
     case NORMAL_MODE:
          NFC_PROGM_ALT_MODE_0(1);     //mode 1 of SWCAN Transceiver 
          WAKE_UP_FIDM_ALT_MODE_1(1);  //mode 0 of SWCAN Transceiver
         break; 
  
  }

}
#endif
/******************************************************************************
* Changes:
*
* Date      By   Description
* -------- ---  ---------------------------------------------------------------
*
*
*
******************************************************************************/
