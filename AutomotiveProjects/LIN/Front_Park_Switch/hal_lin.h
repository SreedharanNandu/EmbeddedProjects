//filename:- HAL_LIN.h
#ifndef HAL_LIN_H
#define HAL_LIN_H



#define MAX_LIN_PAYLOAD_SIZE        8u
#define NO_OF_LIN_IDS               2u


#define Get_Lin_Msg_ID(idx)        (Lin_Msg[idx].id)
#define Get_Lin_Msg_Length(idx)    (Lin_Msg[idx].len)


typedef enum
{
   LIN_IDLE=0,
   SYNC_BREAK_WAIT,
   SYNC_WAIT,
   ID_WAIT,
   DATA_TRANSMIT,
   DATA_RECEIVE,
   CHKSUM_TRANSMIT,
   CHKSUM_RECEIVE
}Lin_State_t;

extern volatile Lin_State_t Lin_State;
 

typedef struct
{
   unsigned char id;
   unsigned char data[MAX_LIN_PAYLOAD_SIZE];
   unsigned char checksum;
}Lin_Data_t;

extern volatile Lin_Data_t Lin_Data[NO_OF_LIN_IDS];

/***********************************************************************************
Function Prototypes
***********************************************************************************/
extern void LIN_Initialize( void );
extern void LIN_Task_USART_Interrupt(unsigned char data);
extern unsigned char LIN_Calc_Checksum(unsigned char *data,unsigned char length);
extern void LIN_Send_Data(unsigned char data);


#endif 
