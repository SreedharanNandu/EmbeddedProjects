#ifndef LIN_CFG_H
#define LIN_CFG_H

#include "hal_lin.h"
                       
typedef enum
{
   NO_PAYLOAD=0,
   TRANSMIT_PAYLOAD,
   RECEIVE_PAYLOAD    
   
}lin_msg_type_t;


typedef struct
{
   unsigned char  id;
   unsigned char  len;
   lin_msg_type_t type;
   unsigned char  *buf_ptr;
}Lin_Msg_t;

extern const Lin_Msg_t Lin_Msg[NO_OF_LIN_IDS];
extern unsigned char Msg_01_Buffer[MAX_LIN_PAYLOAD_SIZE];       
extern unsigned char Msg_02_Buffer[MAX_LIN_PAYLOAD_SIZE];




/***********************************************************************************
Function Prototypes
***********************************************************************************/



#endif 