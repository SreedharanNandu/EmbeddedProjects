//filename:- HAL_LIN.h
#ifndef HAL_LIN_H
#define HAL_LIN_H

#include "HAL_Init.h"
#include "INTF_LIN_Config.h"

#define TX 0u
#define RX 1u

#define START_COMD (u8)0x55 

#define MAX_LIN 8u

#define LIN_DIAG_REQ_ID  0x3Cu
#define LIN_DIAG_RESP_ID 0x7Du/*for 0x3D*/


#define LIN_IDLE_TIMER  200u

#define BIT(x,p)  ((x&(1<<p))?1u:0u)

#define Get_Rx_ID1(id)        (Lin_Rx_Msg1[id].id)
#define Get_Rx_Length1(id)    (Lin_Rx_Msg1[id].len)
#define Get_Rx_Type_ID1(idx)  (Lin_Rx_Msg1[idx].type)

#define Get_Next_Tx_Slot0()   (Lin_Next_Slot0)

#define Get_Tx_ID0(idx)       (Lin_Tx_Msg0[idx].id)
#define Get_Tx_Length0(id)    (Lin_Tx_Msg0[id].len)
#define Get_Tx_Type_ID0(idx)  (Lin_Tx_Msg0[idx].type)

//disp
#define NO_OF_RX_ID0               1u
#define NO_OF_TX_ID0               1u
#define NO_OF_ID0                  (NO_OF_RX_ID0 + NO_OF_TX_ID0)

//sensor
#define NO_OF_RX_ID1               2u
#define NO_OF_TX_ID1               0u
#define NO_OF_ID1                  (NO_OF_RX_ID1 + NO_OF_TX_ID1)



typedef enum
{
   NO_INT=0,
   RX_INT=1,
   TX_INT=2,
   STATUS_INT=3,
}Usart_Interrupt_t;

typedef enum
{
   LIN_IDLE=0,
   SYNC_BREAK_WAIT,
   SYNC_WAIT,
   ID_WAIT,
   DATA_WAIT_TX,
   DATA_WAIT_RX,
   CHSKUM_WAIT_TX,
   CHSKUM_WAIT_RX
}Lin_state_t;

extern volatile Lin_state_t Lin_State;
extern volatile Lin_state_t Lin_State0;

typedef struct
{
   /* Reception ID                                     */
   unsigned char id;
   /* Transmit data                                   */
   unsigned char data[MAX_LIN];
   unsigned char checksum;
}lin_data_t;

extern lin_data_t lin_rx_data0[NO_OF_RX_ID0];
extern lin_data_t lin_rx_data1[NO_OF_RX_ID1];

typedef struct
{
   /* Transmit ID                                     */
   unsigned char id_transmit;
   /* Transmit data                                   */
   unsigned char transmit_data[MAX_LIN];
   unsigned char checksum;
}lin_tx_data_t;

extern lin_tx_data_t lin_tx_data0[NO_OF_TX_ID0];


extern u8 Lin_Next_Slot0;
extern u16 Lin_Periodic_Timer0;

extern volatile u32 interByteTimer1;
extern volatile u8 Lin1_Active; 
extern volatile u8 Lin1_Fault; 

extern volatile u32 interByteTimer0;
extern volatile u8 Lin0_Active; 
extern volatile u8 Lin0_Fault; 

/***********************************************************************************
Function Prototypes
***********************************************************************************/
extern void LIN_Initialize( void );
extern void LIN_Task_USART0_Interrupt(u8 data ,Usart_Interrupt_t isr_type);
extern void LIN_Task_USART1_Interrupt(u8 data);
extern unsigned char LIN_Calc_Checksum(unsigned char *data,unsigned char length);
extern void LIN_Send_Wakeup_Signal0(void);
#endif 
