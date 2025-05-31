/*****************************************************************************
*                                 System Includes                            *
******************************************************************************/
#include <stdio.h>
#include <ctype.h> 
#include <stdlib.h>
#include <string.h>
#include "system_def.h"
#include "HAL_CAN_API.h"
#include "EAL_CAN.h"
#include "HAL_Init.h"
#include "APP_CAN.h"

/*****************************************************************************
*                                 Macro Definitions                          *
******************************************************************************/

/*****************************************************************************
*                                 Type Declarations                          *
******************************************************************************/
/*
** Structure which maintainds message misisng timeout values and missing timers
*/

#define BUS_OFF_INIT_VALUE 0


/*****************************************************************************
*                    Globally  accessed Variable Declarations              *
******************************************************************************/
/******************************************************************************
**               M E M O R Y      A L L O C A T I O N
******************************************************************************/
u8 CAN_Rx_Interrupt_Count;
u8 CAN_Tx_Interrupt_Count;
u8 CAN_Error_Interrupt_Count;
u8 CAN_Wake_Interrupt_Count;

/**************************************************************************
 *       Global Data - Global to Physical and Transfer Layer
 *************************************************************************/

static u8 *buff_ptr;
static u8 CAN_Buffer[CAN_BUFFER_SIZE];
tx_buff_struct_T *tx_buff_struct_ptr[CAN_NUMBER_OF_TRANSMIT_MESSAGES];




static CAN_RX_BUFF_S  fl_rx_buff_Q_S[RX_QUEUE_SIZE];
static u8 fl_in_pos_U8;

/******************************************************************************
**               L O C A L   F U N C T I O N S
******************************************************************************/

/*******************************
* Local Function Prototypes
********************************/
void EAL_CAN_Set_MultiBuff_And_Tx_Msg( void );
static void EAL_CAN_Is_Out_Buff_Maxed(u8 tx_msg_no);
void EAL_CAN_Is_In_Buff_Maxed(u8 tx_msg_no);
u8 *EAL_CAN_Allocate_Buffer_Space( u16 alloc_size );
void EAL_CAN_Reset_Init(void);
static void EAL_CAN_Load_Msg_Tx_MultiBuffer(u8 tx_msg_no,u8 buff_no);


/******************************************************************************
**               F U N C T I O N      D E F I N I T I O N S
******************************************************************************/
/******************************************************************************
**   Function Name  : EAL_CAN_Init
**   Input Params   : None
**   Output Params  : void
**   Description    : This function initializes all message buffers,timers
**                  : and flags related to CAN rx and tx messages.
******************************************************************************/
void EAL_CAN_Init(void)
{
   EAL_CAN_Reset_Init();
}

/******************************************************************************
**   Function Name   : EAL_CAN_End_of_Tx
**   Input Params    : None
**   Output Params   : void
**   Description     : 
**                   : 
******************************************************************************/
void EAL_CAN_End_of_Tx(void)
{
   EAL_CAN_Set_MultiBuff_And_Tx_Msg();

}

/*.***************************************************************************
*. Name: CAN_Reset_Init
*.
*.   Description:
*.      Initializes CAN hardware registers (calls physical layer to
*.      initialize hardware)  and software buffers
*.
*.   Warning: none
*.
*. Shared Variables: tx_can_list (constant)
*.                   CAN_Receive_Message_Parameter_Table (constant)
*.
*. Parameters: none
*.
*. Return Value: none
*.
*.
******************************************************************************/
void EAL_CAN_Reset_Init(void)
{
   u8 idx;

   buff_ptr = CAN_Buffer;

   /* initialize CAN buffer and CAN buffer ctl pointers and structures */
   for(idx =0; idx < (u8)CAN_NUMBER_OF_TRANSMIT_MESSAGES;idx++)
   {

      if(idx < (u8)CAN_NUMBER_OF_TRANSMIT_MESSAGES)
      {
         tx_buff_struct_ptr[idx]  = ( tx_buff_struct_T * )EAL_CAN_Allocate_Buffer_Space( (u16)(sizeof(tx_buff_struct_T ) ));

         #if (FIFO_TYPE == FIFO_ONLY) || (FIFO_TYPE == FIFO_AND_BLOCK)
            tx_buff_struct_ptr[idx]->buffer_full = FALSE;
         #endif

         tx_buff_struct_ptr[idx]->number_of_messages_in_buffer = 0;

         tx_buff_struct_ptr[idx]->buff_msgs = FALSE;

         #if FIFO_TYPE == FIFO_AND_BLOCK
           tx_buff_struct_ptr[idx]-> block_messages_left = 0;
         #endif

         tx_buff_struct_ptr[idx]-> buffer_base_address = EAL_CAN_Allocate_Buffer_Space( ( u16 )(tx_can_list[idx].dlc *( tx_can_list[idx].buff_size + 1 ) ) );

         tx_buff_struct_ptr[idx]->buffer_max_address = tx_buff_struct_ptr[idx]->buffer_base_address + (tx_can_list[idx].dlc * (tx_can_list[idx].buff_size + 1 ) );

         #if REAL_TIME_MESSAGES
            tx_buff_struct_ptr[idx]->pong_buffer_address = ( u8 * )(( u16 )tx_buff_struct_ptr[idx]->buffer_base_address + tx_can_list[idx].dlc );
         #endif

         tx_buff_struct_ptr[idx]->next_data_in_ptr = tx_buff_struct_ptr[idx]->buffer_base_address;
         tx_buff_struct_ptr[idx]->next_data_out_ptr = tx_buff_struct_ptr[idx]->buffer_base_address;

      }

   }
}

/*.***************************************************************************
*. Name: Allocate_Buffer_Space
*.
*.   Description:
*.      A single character array is allocated in can_aif.c to buffer all
*.      CAN messages.  This space is then allocated to individual messages
*.      at execution time.  This scheme is used since each message may be
*.      of a differing length, and differing depth, making the use of
*.      run time allocation of memory for  each message necessary.
*.
*.   Warning: This routine does not check array bounds.  If the application
*.      fails to allocate enough memory in the single character arrary, then
*.      overruning this memory could corrupt other areas of memory.  Because
*.      of this, the CAN wizard will compute and update the amount of memory
*.      required, and update the can_aif.c file as required, if requested.
*.
*. Shared Variables: CAN_Buffer
*.
*. Parameters: requested allocation in bytes
*.
*. Return Value: pointer to beginning address of allocated memory
*.
*.
******************************************************************************/
u8 *EAL_CAN_Allocate_Buffer_Space( u16 alloc_size )
{
   u8 *alloc_ptr = buff_ptr;
   buff_ptr = ( u8 * ) ( ( u16 ) buff_ptr + alloc_size );
   return( alloc_ptr );
}

/*.***************************************************************************
*. Name: Check_For_FIFO_tx_buf_full MACRO
*.
*.   Description:
*.      Checks for buffer full, and sets flag if necessary
*.
*.   Warning: tx_msg_no must be defined within the scope
*.            of the invocation of this macro.
*.
*. Shared Variables: tx_buff_struct_ptr structure (variable)
*.
*. Parameters: none
*. Return Value: none
*.
*.
******************************************************************************/
void EAL_CAN_Is_FIFO_Full(u8 tx_msg_no)
{
   if(tx_pending_no_of_msgs == TX_MSG_SIZE)
   {
      tx_buf_full = TRUE;
   }
}

/*.***************************************************************************
*. Name: EAL_CAN_Is_Out_Buff_Maxed MACRO
*.
*.   Description:
*.      Checks to see if the next_data_out_ptr in the transmit buffer for
*.      the specified message should be wrapped and does the wrapping if
*.      required.
*.
*.   Warning: Is is assumed that tx_msg_no is defined
*.            in scope when this macro is invoked.
*.
*. Shared Variables: none
*.
*. Parameters: none
*.
*. Return Value: none
*.
*.
*. Updated:

******************************************************************************/
void EAL_CAN_Is_Out_Buff_Maxed(u8 tx_msg_no )
{
   if( tx_out_ptr == tx_buff_max_address )
   {
      tx_out_ptr = tx_buff_base_address;
   }
}
/*.***************************************************************************
*. Name:  Check_FIFO_Transmit_Buffer_Pointers_For_Wrap MACRO
*.
*.   Description:
*.      Checks to see if the buffer in pointer needs to wrap for a transmit
*.      message. Wrap the pointer if required.
*.
*.   Warning: tx_msg_no must be defined within the scope
*.            of the code that invokes this macro.
*.
*. Shared Variables: tx_buff_struct_ptr structure (variable)
*.
*. Parameters: None
*.
*. Return Value: none
*.
*.
******************************************************************************/
void EAL_CAN_Is_In_Buff_Maxed(u8 tx_msg_no)
{
   if(tx_in_ptr == tx_buff_max_address)
   {
      tx_in_ptr = tx_buff_base_address;
   }
}
/*.***************************************************************************
*. Name: Transmit_Message
*.
*.   Description:
*.      Loads a message into the software buffers and calls the physical
*.      layer function to transmit the message.
*.
*.   Warning: none
*.
*. Shared Variables: tx_can_list (constant)
*.                   Receive_Buffer_Ctl_Ptr structure (variable)
*. Parameters: tx_msg_no
*.             message_address
*.
*. Return Value: TRUE if the request could be honored, else FALSE.
******************************************************************************/
u8 EAL_CAN_Tx_Msg( u8 tx_msg_no,u8 *message_address )
{
   u8 idx;
   u8 request_honored;


   Disable_Interrupts();

   #if ((FIFO_TYPE == FIFO_ONLY) || (FIFO_TYPE == FIFO_AND_BLOCK))

   if(tx_buf_full && (FIFO_CAN_MESSAGE == TX_MSG_TYPE))
   {
      /* CAN software transmit buffer overflow */
      /* Call function to set transmit buffer overflow flag */

      /* can't transmit when FIFO buffer is full */
      request_honored = FALSE;
   }
   else
   {

   #endif
      for(idx=0;idx<TX_MSG_DLC;idx++ )
      {
         /* the next two statements should be able to be coded
          * as a single statement, but it appears a compiler
          * bug prevents the single statement version from working.
          */

         *tx_in_ptr = *message_address;
         message_address++;

         tx_in_ptr++;
      }

      #if ((FIFO_TYPE == FIFO_ONLY) || (FIFO_TYPE == FIFO_AND_BLOCK))
      tx_pending_no_of_msgs++;
      EAL_CAN_Is_FIFO_Full(tx_msg_no);
      EAL_CAN_Is_In_Buff_Maxed( tx_msg_no );
      #endif


      request_honored = TRUE;

   #if ((FIFO_TYPE == FIFO_ONLY) || (FIFO_TYPE == FIFO_AND_BLOCK))
   }
   #endif


   /* don't call this routine if a message is in
    * a hardware buffer, and an interrupt can go
    * off, resulting in this routine being called
    * again before it is exited.
    */
   EAL_CAN_Set_MultiBuff_And_Tx_Msg();
   Enable_Interrupts();

   return( request_honored );

}  /* End function Transmit_Message(...) */






/******************************************************************************
**   Function Name   : EAL_CAN_Fast_Periodic
**   Input Params    : 
**   Output Params   : 
**   Description     : 
******************************************************************************/
void EAL_CAN_Set_MultiBuff_And_Tx_Msg( void )
{
   /* ID of message being transmitted in each of the three CAN buffers */
   static u16 number_of_message_in_CAN_buffer[]= {BUFFER_EMPTY, BUFFER_EMPTY, BUFFER_EMPTY,
                                                  BUFFER_EMPTY, BUFFER_EMPTY, BUFFER_EMPTY,
                                                  BUFFER_EMPTY};

   /* Set up a single variable to show which buffers are available */
   static CAN_buffer_available_T available_CAN_buffers =  {(BUFFER_0 | BUFFER_1 |BUFFER_2)};

   static CAN_buffer_available_T  available_buffer = NO_BUFFER;
   static u8  tx_msg_no;


   /* find out what buffer just became empty, if any, and
    * set parameters accordingly
    */
   if( BUFFER_EMPTY != number_of_message_in_CAN_buffer[ BUFFER_0 ] && CAN_Transmit_Buffer_0_Is_Empty() )
   {
      tx_buff_struct_ptr[number_of_message_in_CAN_buffer[BUFFER_0]]->buff_msgs = FALSE;
      number_of_message_in_CAN_buffer[BUFFER_0] = BUFFER_EMPTY;
      available_CAN_buffers |= BUFFER_0;
   }
   else if((BUFFER_EMPTY != number_of_message_in_CAN_buffer[BUFFER_1]) && CAN_Transmit_Buffer_1_Is_Empty())
   {
      tx_buff_struct_ptr[number_of_message_in_CAN_buffer[BUFFER_1]]->buff_msgs = FALSE;
      number_of_message_in_CAN_buffer[BUFFER_1] = BUFFER_EMPTY;
      available_CAN_buffers |= BUFFER_1;
   }
   else if((BUFFER_EMPTY != number_of_message_in_CAN_buffer[ BUFFER_2 ]) && CAN_Transmit_Buffer_2_Is_Empty())
   {

      tx_buff_struct_ptr[number_of_message_in_CAN_buffer[BUFFER_2]]->buff_msgs = FALSE;
      number_of_message_in_CAN_buffer[BUFFER_2] = BUFFER_EMPTY;
      available_CAN_buffers |= BUFFER_2;
   }

   tx_msg_no = 0;
   while(available_CAN_buffers != 0  && (tx_msg_no < (u8)CAN_NUMBER_OF_TRANSMIT_MESSAGES))
   {
      if( available_CAN_buffers & BUFFER_0 )
      {
         available_buffer = BUFFER_0;
      }
      else if( available_CAN_buffers & BUFFER_1 )
      {
         available_buffer = BUFFER_1;
      }
      else if( available_CAN_buffers & BUFFER_2 )
      {
         available_buffer = BUFFER_2;
      }/* end if */
      
      if((tx_pending_no_of_msgs > 0) && (!tx_buff_msgs))
      {
         number_of_message_in_CAN_buffer[available_buffer] = tx_msg_no;
         EAL_CAN_Load_Msg_Tx_MultiBuffer( tx_msg_no,available_buffer);
         tx_buff_msgs = TRUE;
         available_CAN_buffers = ( CAN_buffer_available_T )( ( ~available_buffer ) & available_CAN_buffers );
      }/*end if        */

      tx_msg_no++;
   }/*end while        */

/* Get_Event_Duration( HW_TRANSMIT_MESSAGE ); */
}/*end procedure       */


/*.******************************************************************
*. Name: Load_Message_Into_CAN_Transmit_Buffer
*.    This routine is the interrupt handler for transmitting
*.    a message.
*.
*. Shared Variables: tx_buff_struct_ptr
*.                   CAN_Transmit_Message_Parameter_Table
*.
*. Parameters: tx_msg_no
               buffer_number (0-2, CAN hardware buffer)
*. Return Value: none.
*.
*.
*  Updates:
********************************************************************/
static void EAL_CAN_Load_Msg_Tx_MultiBuffer(u8 tx_msg_no,u8 buff_no)
{
   static u8 *destination_address;
   static u8 idx;
   static u8  stack_buff[8]={PAD_BYTES};
   CAN_TX_BUFF_S msgObject;

   switch( buff_no )
   {
      case BUFFER_0:
           msgObject.DLC = TX_MSG_DLC;
           if(TX_EXT_ID)
           {
              msgObject.is_ext_frm = TRUE;
              msgObject.Msg_ID = TX_MSG_ID<<13;
           }
           else
           {
              msgObject.is_ext_frm = FALSE;
              msgObject.Msg_ID = TX_MSG_ID;
           }
           destination_address = stack_buff;


           for( idx = 0;idx < TX_MSG_DLC;idx++ )
           {
              *destination_address++ = *tx_out_ptr;

              tx_out_ptr++;
           }
           msgObject.data = stack_buff;
           if(TX_MSG_DLC>0)
           {
              HAL_CAN_Transmit(&msgObject,0);
           }
           break;

      case BUFFER_1:
           msgObject.DLC = TX_MSG_DLC;
           if(TX_EXT_ID)
           {
              msgObject.is_ext_frm = TRUE;
              msgObject.Msg_ID = TX_MSG_ID<<13;
           }
           else
           {
              msgObject.is_ext_frm = FALSE;
              msgObject.Msg_ID = TX_MSG_ID;
           }


           destination_address = stack_buff;


           for( idx = 0;idx < TX_MSG_DLC;idx++ )
           {
              *destination_address++ = *tx_out_ptr;

              tx_out_ptr++;
           }
           msgObject.data = stack_buff;
           if(TX_MSG_DLC>0)
           {
              HAL_CAN_Transmit(&msgObject,1);
           }
           break;

      case BUFFER_2:
           msgObject.Msg_ID = TX_MSG_ID;
           msgObject.DLC = TX_MSG_DLC;
           if(TX_EXT_ID)
           {
              msgObject.is_ext_frm = TRUE;
              msgObject.Msg_ID = TX_MSG_ID<<13;
           }
           else
           {
              msgObject.is_ext_frm = FALSE;
              msgObject.Msg_ID = TX_MSG_ID;
           }


           destination_address = stack_buff;


           for( idx = 0;idx < TX_MSG_DLC;idx++ )
           {
              *destination_address++ = *tx_out_ptr;

              tx_out_ptr++;
           }
           msgObject.data = stack_buff;
           if(TX_MSG_DLC>0)
           {
              HAL_CAN_Transmit(&msgObject,2);
           }
         break;
      default:
         break; /* not possible */
   }
   tx_pending_no_of_msgs--;
   #if ((FIFO_TYPE == FIFO_ONLY) || (FIFO_TYPE == FIFO_AND_BLOCK))
   tx_buf_full = FALSE;
   #endif
   EAL_CAN_Is_Out_Buff_Maxed(tx_msg_no);
   idx = buff_no;
}




/******************************************************************************
**   Function Name   : EAL_CAN_Fast_Periodic
**   Input Params    : 
**   Output Params   : 
**   Description     : 
******************************************************************************/
void EAL_CAN_Fast_Periodic(void)
{
}
/******************************************************************************
**   Function Name   : EAL_CAN_AddtoRxQueue
**   Input Params   : BYTE *p_rx_buff_S- Received CAN buffer
**   Output Params   : void
**   Description      : This function adds the messages into receive queue
******************************************************************************/
void EAL_CAN_AddtoRxQueue(CAN_RX_BUFF_S *p_rx_buf_S)
{

   /* Copy received CAN messge into local buffers */
   memcpy(&(fl_rx_buff_Q_S[fl_in_pos_U8]), p_rx_buf_S, sizeof(CAN_RX_BUFF_S));

   #if 0
   App_Get_Rx_Msg_No(fl_rx_buff_Q_S[fl_in_pos_U8].Msg_ID,
                     fl_rx_buff_Q_S[fl_in_pos_U8].is_ext_frm,
                     fl_rx_buff_Q_S[fl_in_pos_U8].DLC,
                     fl_rx_buff_Q_S[fl_in_pos_U8].data);
   #endif
   /* Point to next location */
   fl_in_pos_U8++;

   if(RX_QUEUE_SIZE == fl_in_pos_U8)
   {
      /* Roll over */
      fl_in_pos_U8 = 0;
   }
}
/******************************************************************************
**               R E V I S I O N      H I S T O R Y
*******************************************************************************
** Please update in the following format:
**
** Revision No      Date(dd/mm/yy)            Author
** Description of change
**
******************************************************************************/


