#ifndef INTF_LIN_CONFIG_H
#define INTF_LIN_CONFIG_H


#define Send_Byte(x)   do            \
                       {             \
                          LIN_Response_Transmit_Delay_for_Renesas();\
                          LTMK0 = 1U;\
                          UF0TXB=x;  \
                          LTMK0 = 0U;\
                       }while(0)


typedef struct
{
   unsigned char id;
   unsigned char len;
   unsigned char *buf_ptr;
}Lin_Data_t;

extern const Lin_Data_t Lin_Rx_Msg[2];
extern unsigned char Msg_14_Buffer[7];       
extern unsigned char Msg_5E_Buffer[3];

extern const Lin_Data_t Lin_Tx_Msg[2];
extern unsigned char Msg_14_Buffer0[7];       
extern unsigned char Msg_5E_Buffer0[3];



/***********************************************************************************
Function Prototypes
***********************************************************************************/



#endif 