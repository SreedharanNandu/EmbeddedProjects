#ifndef APP_CAN_H
#define APP_CAN_H

/*App_Can.h*/
/**************************************************************************
 *       #include's
 *************************************************************************/
#include "system_def.h"
#include "EAL_CAN.h"

/**************************************************
 *       #define's
 ***************************************************/

/* user must specify baud rate */
/* user update */

#define CAN_TRANSMIT_MESSAGE_LENGTH 8


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * Following compile constants are added to enable the code
 * for different message types. User must configure REAL_TIME_MESSAGES
 * and FIFO_TYPE based on the requirements.
 * if No real time messages required, then REAL_TIME_MESSAGES=FALSE.
 * if No FIFO Messages, then FIFO_TYPE = NO_FIFO.
 * if Only FIFO required and Block messages are not required, then
 * FIFO_TYPE = ONLY_FIFO.
 * If both FIFO and Block Messages are required,
 * then FIFO_TYPE = FIFO_AND_BLOCK.
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define NO_FIFO 0
#define FIFO_ONLY 1
#define FIFO_AND_BLOCK 2

#define REAL_TIME_MESSAGES TRUE

#define FIFO_TYPE FIFO_ONLY


/****************************************************
 *
 *
 *  The applications
 * programmer does not have to know the details of the
 * following parameters, as the offline CAN wizard will
 * read the parameter tables (which must be
 * correct) and calculate the appropriate values of the
 * following parameters.  These parameters more properly
 * belong to the physical layer, but in order to make it
 * so that the physical layer does not have to change when
 * the applciation changes, these parameters are in
 * the applications interface layer.
 *
 * Exception: the user must specify the CAN_BAUD_RATE
 *            and CAN_receive_mode as well as CAN_transmit_mode
 *            correctly.
 *
 * *************************************************/
typedef enum
{
   CAN_RX_ID = 0x7E0u,
   CAN_TX_ID = 0x7E8u

}CanID_T;


/* CAN wizard update */
#define CAN_NUMBER_OF_TRANSMIT_MESSAGES           1u



#define msg_id_LOOKUP_TABLE_START_BIT     10
#define msg_id_LOOKUP_TABLE_MASK        0x03
#define msg_id_LOOKUP_TABLE_LENGTH_IN_BITS 2

/****************************************************
 * end for wizard read and update
 * *************************************************/

#define NULL_NOTIFIER_PTR ( ( void ( * )( void ) ) 0u )
#define MESSAGE_ID_NOT_IN_TABLE  255u


/*******************************
 *  typedef's
 *******************************/

/* no update required */
typedef u8  dlc_T;
typedef u8  buff_size_T;
typedef u32 msg_id_T;

typedef enum
{
   REAL_TIME_CAN_MESSAGE,
   FIFO_CAN_MESSAGE
}msg_type_T;

typedef struct
{
   msg_id_T msg_id;
   dlc_T dlc;
   u8 extd_id_or_not;
   void( *notifier_function_ptr ) ( void );
   msg_type_T msg_type;
   buff_size_T buff_size;
}CAN_message_parameter_T;

/**************************************************************************
 *       global prototypes
 *************************************************************************/
/**************************************************************************
 *       global data
 *************************************************************************/

extern const CAN_message_parameter_T tx_can_list[];
extern const CAN_message_parameter_T CAN_Receive_Message_Parameter_Table[];
extern const u8 msg_id_Lookup_Table[];
extern void App_CAN_Data_Transmit( void );
extern u8 App_Get_Tx_Msg_No( u32 Message_ID);


#endif
