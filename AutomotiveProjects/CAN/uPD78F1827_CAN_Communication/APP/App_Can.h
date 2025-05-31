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
   CAN_ID_3AA = 0x3AAu, //0x10754040
   CAN_ID_121 = 0x121u, //0x10242040
   CAN_ID_122 = 0x122u, //0x10244045
   CAN_ID_115 = 0x115u, //0x1022A040
   CAN_ID_124 = 0x124u, //0x10248040
   CAN_ID_165 = 0x165u, //0x102CA040
   CAN_ID_167 = 0x167u, //0x102CE040
   CAN_ID_166 = 0x166u, //0x102CC040
   CAN_ID_108 = 0x108u, //0x10210040
   CAN_ID_56C = 0x56Cu, //0x10AD8045
   CAN_ID_574 = 0x574u, //0x10AE8045
   CAN_ID_62C = 0x62Cu

}CanTxID_T;

typedef enum
{
   CAN_ID_1D0 = 0x103A00BBu,
   CAN_ID_1D4 = 0x103A80BBu,
   CAN_ID_396 = 0x1072C0B9u, 
   CAN_ID_397 = 0x1072E05Bu,
   CAN_ID_20E = 0x1041C0B9u,
   CAN_ID_20C = 0x104180B9u,
   CAN_ID_207 = 0x1040E0B9u,
   CAN_ID_576 = 0x10AEC0B9u,
   CAN_ID_300 = 0x106000B9u,
   CAN_ID_200 = 0x104000BBu

}CanRxID_T;
/* CAN wizard update */
#define CAN_NUMBER_OF_TRANSMIT_MESSAGES           12u



#define msg_id_LOOKUP_TABLE_START_BIT     10
#define msg_id_LOOKUP_TABLE_MASK        0x03
#define msg_id_LOOKUP_TABLE_LENGTH_IN_BITS 2

/****************************************************
 * end for wizard read and update
 * *************************************************/

#define NULL_NOTIFIER_PTR ( ( void ( * )( void ) ) 0 )
#define MESSAGE_ID_NOT_IN_TABLE  255


/*******************************
 *  typedef's
 *******************************/

/* no update required */
typedef u8  dlc_T;
typedef u8  buff_size_T;
typedef u32 msg_id_T;

typedef enum
{
   SLEEP_MODE,
   HS_MODE,
   HV_WAKEUP_MODE,
   NORMAL_MODE
}Mode_T;

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

typedef union
{
   
   u8 byte[5];
   struct
   {
     u8 SC_SndTne       : 4;
     u8 SndLocRtRr      : 1;
     u8 SndLocLftRr     : 1;
     u8 SndLocPasFr     : 1;
     u8 SndLocDrFr      : 1;
     u8 SC_SndCdnPrd    : 8;
     u8 SC_NmofRp       : 8;
     u8 SC_SndDutCyc    : 8;
     u8 SndPriority     : 8;
   }bits;

}Chime_T;

typedef union
{
   
   u8 byte[4];
   struct
   {
     u8 PrkAsstRrExtdDistH   : 4;
     u8 PrkAstRrSysStat      : 2;
     u8 reserved             : 2;
     u8 PrkAsstRrExtdDistL   : 8;
     u8 PARrRgn1ObjStat      : 4;
     u8 PARrRgn2ObjStat      : 4;
     u8 PARrRgn3ObjStat      : 4;
     u8 PARrRgn4ObjStat      : 4;

   }bits;

}Rear_Status_T;

typedef union
{
   
   u8 byte;
   struct
   {
     u8 PrkAsstClnPrkAstIO  : 1;
     u8 PrkAsstDisabIO      : 1;
     u8 PrkAstFld           : 1;
     u8 PrkAstSnrsBlk       : 1;
     u8 PrkAstSnsDistrbdIO  : 1;
     u8 dummy_reserved      : 3;
   }bits;

}Gen_Status_T;

typedef union
{
   
   u8 byte[7];
   struct
   {
     u8 DtcTriggered        : 1;
     u8 reserved            : 7; 
     u8 DtcSource           : 8;
    u16 DtcNumber           : 16;
     u8 Fail_Type           : 8;
     u8 CodeSupported       : 1;
     u8 CurrentStatus       : 1;
     u8 TstNPsdCdClrdSt     : 1;
     u8 TstFldCdClrdStat    : 1;
     u8 HistStat            : 1;
     u8 TstNPsdPwrUpSt      : 1;
     u8 TstFldPwrUpSt       : 1;
     u8 WrnIndRqdSt         : 1;
     u8 Fault_Type          : 8;

   }bits;

}DTC_T;
/**************************************************************************
 *       global prototypes
 *************************************************************************/
extern Gen_Status_T General_Status;
extern Chime_T Chime_Command;
extern Rear_Status_T Rear_Status;
/**************************************************************************
 *       global data
 *************************************************************************/

extern const CAN_message_parameter_T tx_can_list[];
extern const CAN_message_parameter_T CAN_Receive_Message_Parameter_Table[];
extern const u8 msg_id_Lookup_Table[];
extern void App_CAN_Data_Transmit_1s( void );
extern void App_CAN_Data_Transmit_100ms( void );
extern void App_CAN_Data_Transmit_250ms( void );
extern void App_CAN_Data_Transmit_500ms( void );
extern void App_Get_Rx_Msg_No(u32 Msg_ID, bool is_ext_frm ,u8 DLC,u8 *data);
#if (GMLAN_ENABLED == TRUE)
extern void App_Trcv_Mode_HVWU_To_Normal(void);
extern void Trcv_Mode(Mode_T mode);
#endif


#endif
