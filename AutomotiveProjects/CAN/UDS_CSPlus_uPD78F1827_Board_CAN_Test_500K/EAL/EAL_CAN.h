/******************************************************************************
** Project Name    : 
** File Name       : EAL_CAN.h
** Description     : 
**                 : 
** Date            : 
** Version         : 
** Author          : 
******************************************************************************/
#ifndef EAL_CAN_H
#define EAL_CAN_H

/******************************************************************************
*                      System/Project Includes                            *
******************************************************************************/
#include "system_def.h"
#include "App_Can.h"

/*****************************************************************************
*                             Macro Definitions                              *
******************************************************************************/

#define PAD_VALUE 0xFFu
#define PAD_BYTES PAD_VALUE,PAD_VALUE,PAD_VALUE,PAD_VALUE,PAD_VALUE,PAD_VALUE,PAD_VALUE,PAD_VALUE

/******************************************************************************
*                 DATA -Typedefs/Global varaibles                           *
******************************************************************************/
typedef enum
{
   EVENT,
   EVENT_PER,
   PER
}Msg_Type_T;

typedef enum
{
   NO_BUFFER = 0,
   BUFFER_0=1,
   BUFFER_1=2,
   BUFFER_0_AND_1 = 3,
   BUFFER_2=4,
   BUFFER_0_AND_2 = 5,
   BUFFER_1_AND_2 = 6,
   BUFFER_0_AND_1_AND_2 = 7
}CAN_buffer_available_T;

/*########################### TRANSMIT PART #################################*/

/*
** Transmission buffer - TO BE FILLED BY APPLICATION
*/
typedef struct
{
   u8 DLC;                     /* Transmission buff length         */
   bool is_ext_frm;              /* standard msg or extended msg     */
   u32 Msg_ID;                  /* Message Identifier               */
   u8 *data;                    /* Message data                     */

}CAN_TX_BUFF_S;


/*########################### RECEIVE PART #################################*/


/*
** Reception buffer
*/
typedef struct
{
   u8   DLC;               /* Received buffer length            */
   bool is_ext_frm;               /* standard msg or extended msg     */
   u32 Msg_ID;
   u8 data[8];                  /* Message data                     */

}CAN_RX_BUFF_S;


typedef struct
{
   CAN_TX_BUFF_S txbuf;
   Msg_Type_T  msg_type;
   u16 period;
}Tx_Msg_List_T;




/*
 * The following macros are intended to allow the code to be
 * written at a higher level of abstraction, hopefully resulting
 * in code which is easier to read and maintain.  Of necessity,
 * higher levels of abstraction hide some detail.  In this case,
 * the detail that is hidden is the fact that references to message
 * parameters are references to an element of an array of structures.
 * This fact will have to be kept in mind when reading and maintaining
 * the code.  It is always necessary to have tx_msg_no
 * and/or CAN-receive_message_number defined within the scope of the use
 * of these macros.
 */
#define CAN_BUFFER_SIZE 512u

#define TX_MSG_ID                     tx_can_list[tx_msg_no].msg_id
#define TX_MSG_DLC                    tx_can_list[tx_msg_no].dlc
#define TX_CALLBACK_NOTFN             tx_can_list[tx_msg_no] notifier_function_ptr
#define TX_MSG_TYPE                   tx_can_list[tx_msg_no].msg_type
#define TX_MSG_SIZE                   tx_can_list[tx_msg_no].buff_size
#define TX_EXT_ID                     tx_can_list[tx_msg_no].extd_id_or_not

#define BUFFER_EMPTY 0xffffu

#define RX_QUEUE_SIZE                 1u



/**************************
 *       typedef's
 *************************/

typedef struct {
   u16 id;
   u16 unused;
} standard_id_T;

typedef struct {
   u32 id;
} extended_id_T;

typedef union {
   standard_id_T standard;
   extended_id_T extended;
} CAN_id_T;

/* This structure is used for both transmit and receive buffer although the
 * transmit_priority is NA for receive buffers.
 */
typedef struct {
   CAN_id_T CAN;       /* Named so that dotted name makes sense. */
   u8 data[8];
   u8 length;
   u8 transmit_priority; /* Lower number is higher priority. */
   u16 time_stamp;
} message_buffer_T;

typedef struct
{
#if (FIFO_TYPE == FIFO_ONLY) || (FIFO_TYPE == FIFO_AND_BLOCK)
   u8   buffer_full;
#endif
#if FIFO_TYPE == FIFO_AND_BLOCK
   u8 block_messages_left;
#endif
   u8 number_of_messages_in_buffer;
   u8 buff_msgs;
   u8 *next_data_in_ptr;
   u8 *next_data_out_ptr;
   u8 *buffer_max_address;
   u8 *buffer_base_address; /* ping buffer address*/
#if REAL_TIME_MESSAGES
   u8 *pong_buffer_address; /* pong buffer address*/
#endif
}tx_buff_struct_T;

typedef struct
{
   u8   stale;
#if (FIFO_TYPE == FIFO_ONLY) || (FIFO_TYPE == FIFO_AND_BLOCK)
   u8   buffer_full;
#endif
   u8   message_lost;
   u8   inactive_message;
   u8   never_received_message;
#if FIFO_TYPE == FIFO_AND_BLOCK
   u8 block_messages_left;
#endif
   u8 number_of_messages_in_buffer;
   u8 *next_data_in_ptr;
   u8 *next_data_out_ptr;
   u8 *buffer_max_address;
   u8 *buffer_base_address; /* ping buffer address */
#if REAL_TIME_MESSAGES
   u8 *pong_buffer_address; /* pong buffer address */
#endif
}receive_buffer_ctl_T;

/*
 * The following macros are intended to allow the code to be
 * written at a higher level of abstraction, hopefully resulting
 * in code which is easier to read and maintain.  Of necessity,
 * higher levels of abstraction hide some detail.  In this case,
 * the detail that is hidden is the fact that references to message
 * buffer parameters are references to a pointer to  array of structures.
 * This fact will have to be kept in mind when reading and maintaining
 * the code.  It is always necessary to have tx_msg_no
 * and/or CAN_receive_message_number defined within the scope of the use
 * of these macros.
 */

#define tx_buf_full                           tx_buff_struct_ptr[tx_msg_no]->buffer_full
#define tx_blk_msg_left                       tx_buff_struct_ptr[tx_msg_no]->block_messages_left
#define tx_pending_no_of_msgs                 tx_buff_struct_ptr[tx_msg_no]->number_of_messages_in_buffer
#define tx_buff_msgs                          tx_buff_struct_ptr[tx_msg_no]->buff_msgs
#define tx_in_ptr                             tx_buff_struct_ptr[tx_msg_no]->next_data_in_ptr
#define tx_out_ptr                            tx_buff_struct_ptr[tx_msg_no]->next_data_out_ptr
#define tx_buff_max_address                   tx_buff_struct_ptr[tx_msg_no]->buffer_max_address
#define tx_buff_base_address                  tx_buff_struct_ptr[tx_msg_no]->buffer_base_address
#define tx_pp_buff_address                    tx_buff_struct_ptr[tx_msg_no]->pong_buffer_address

#define CAN_Transmit_Buffer_0_Is_Empty() (CanMsgTransmitted(0) == 0)
#define CAN_Transmit_Buffer_1_Is_Empty() (CanMsgTransmitted(1) == 0)
#define CAN_Transmit_Buffer_2_Is_Empty() (CanMsgTransmitted(2) == 0)


extern const u8  Highest_Priority_Msg_Lookup_Low[];
extern const u8  Highest_Priority_Msg_Lookup_High[];

/**********************************************
 * Global Variables (global to physical and transfer layers)
 **********************************************/
extern tx_buff_struct_T *tx_buff_struct_ptr[];

/******************************************************************************
*                          Functions                                      *
******************************************************************************/

/*
** Initialize all Rx,Tx buffers and related flags
*/ 
extern void EAL_CAN_Init(void) ;
extern void EAL_CAN_End_of_Tx(void);
u8 EAL_CAN_Tx_Msg( u8 tx_msg_no,u8 *message_address );


#endif /* RX_CAN_H */


/******************************************************************************
**               R E V I S I O N      H I S T O R Y
*******************************************************************************
** Please update in the following format:
**
** Revision No      Date(dd/mm/yy)            Author
** Description of change
**
******************************************************************************/

