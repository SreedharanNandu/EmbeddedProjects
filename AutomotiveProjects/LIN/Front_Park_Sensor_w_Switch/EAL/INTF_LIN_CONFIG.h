#ifndef INTF_LIN_CONFIG_H
#define INTF_LIN_CONFIG_H

#include "HAL_LIN.h"

#define Send_Byte0(x)   do            \
                       {             \
                          LIN_Response_Transmit_Delay_for_Renesas();\
                          LTMK0 = 1U;\
                          UF0TXB=x;  \
                          LTMK0 = 0U;\
                       }while(0)
typedef enum
{
   LIN_HDR_ONLY=0,   
   LIN_HDR_AND_DATA
}lin_msg_type_t;


typedef struct
{
   unsigned char id;
   unsigned char len;
   lin_msg_type_t type;
   unsigned char *buf_ptr;
}Lin_Data_t;

extern const Lin_Data_t Lin_Rx_Msg1[2];
extern unsigned char Msg_14_Buffer[7];       
extern unsigned char Msg_5E_Buffer[3];

extern const Lin_Data_t Lin_Tx_Msg0[2];
extern unsigned char Msg_01_Buffer[8];       
extern unsigned char Msg_02_Buffer[8];



/***********************************************************************************
Function Prototypes
***********************************************************************************/



#endif 