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

#define Get_Receive_ID(id)   (Lin_Rx_Msg[id].id)
#define Get_Rx_Length(id)    (Lin_Rx_Msg[id].len)

#define Get_Next_Transmit_Index()  (Lin_Next_Index0)

#define Get_Transmit_ID(idx)  (Lin_Tx_Msg[idx].id)
#define Get_Tx_Length(id)    (Lin_Tx_Msg[id].len)


#define NO_OF_UART_RX_IDS         2u
#define NO_OF_RX_ID               NO_OF_UART_RX_IDS

#define NO_OF_UART_TX_IDS         2u
#define NO_OF_TX_ID               NO_OF_UART_TX_IDS

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
   SYNC_BREAK_WAIT=1,
   SYNC_WAIT=2,
   ID_WAIT=3,
   DATA_WAIT=4,
   CONF_SEND=5,
   WAIT_TX_DATA=6,
   WAIT_TX_CHKSUM=7,
   CHSKUM_WAIT=8,
   CHSKUM_SEND=9,
   CONF_CHKSUM=10
}Lin_state_t;

extern volatile Lin_state_t Lin_State;

typedef struct
{
   /* Reception ID                                     */
   unsigned char id_receive;
   /* Transmit data                                   */
   unsigned char receive_data[MAX_LIN];
   unsigned char checksum;
}lin_rx_data_t;

extern lin_rx_data_t lin_rx_data[NO_OF_RX_ID];

typedef struct
{
   /* Transmit ID                                     */
   unsigned char id_transmit;
   /* Transmit data                                   */
   unsigned char transmit_data[MAX_LIN];
   unsigned char checksum;
}lin_tx_data_t;

extern lin_tx_data_t lin_tx_data[NO_OF_TX_ID];


extern u8 Lin_Next_Index0;
extern u16 Lin_Periodic_Timer0;

extern volatile u32 interByteTimer1;
extern volatile u8 Lin1_Active; 
extern volatile u8 Lin1_Fault; 


/***********************************************************************************
Function Prototypes
***********************************************************************************/
extern void LIN_Initialize( void );
extern void LIN_Task_USART0_Interrupt(u8 data ,Usart_Interrupt_t isr_type);
extern void LIN_Task_USART1_Interrupt(u8 data);
extern unsigned char LIN_Calc_Checksum(unsigned char *data,unsigned char length);
extern void LIN_Send_Wakeup_Signal0(void);
#endif 
