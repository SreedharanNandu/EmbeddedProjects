/***********************************************************************************
System Includes
***********************************************************************************/

/***********************************************************************************
User Includes
***********************************************************************************/
#include <CG_macrodriver.h>

#include "string.h"
#include "HAL_LIN.h"
#include "INTF_LIN_CONFIG.h"
#include "APP_LIN.h"
#include "system_def.h"

/***********************************************************************************
User Program Code Prototypes
***********************************************************************************/

/**********************************************************************************
Global definisition
***********************************************************************************/

/**********************************************************************************
Global variables
***********************************************************************************/
u8 Lin_Next_Index0;
u16 Lin_Periodic_Timer0;

lin_rx_data_t lin_rx_data[NO_OF_RX_ID];
lin_tx_data_t lin_tx_data[NO_OF_TX_ID];
volatile Lin_state_t Lin_State;
volatile Lin_state_t Lin_State0;

volatile u8 Lin1_Active; 
volatile u8 Lin1_Fault;
volatile u32 interByteTimer1;


static void LIN_Response_Transmit_Delay_for_Renesas( void );
/***********************************************************************************
User Program Code
***********************************************************************************/
/**********************************************************************************
Function Name:  LIN_Initialize
Description:    Setting of a hardware LIN
Parameters:     none
Return value:   none
***********************************************************************************/
void LIN_Initialize(void)
{
   unsigned char id;

   Lin_State = SYNC_BREAK_WAIT;
   for(id = 0u;id<NO_OF_RX_ID;id++)
   {
      lin_rx_data[id].id_receive = Get_Receive_ID(id);
   }


   Lin_State0 = SYNC_BREAK_WAIT;

   for(id = 0u;id<NO_OF_TX_ID;id++)
   {
      lin_tx_data[id].id_transmit = Get_Transmit_ID(id);
   }
   
   Lin_Next_Transmit_Msg_Timer0 = 0u;
   Lin_Next_Index0 = 0u;
   LIN_Send_Wakeup_Signal0();
   interByteTimer1 = 0u;
   Lin1_Active = 1;
}

/**********************************************************************************
Function Name:  USART0 Interrupt(TLIN)
Description:    Display data 
Parameters:     none
Return value:   none
***********************************************************************************/
void LIN_Task_USART0_Interrupt(u8 data, Usart_Interrupt_t isr_type)
{
    static unsigned char dummy;
    static unsigned char temp_cksum;
    static unsigned char tx_data_cnt;
    static unsigned char id_index;
    unsigned char id,k;
    unsigned char tx_data;

   
   switch(Lin_State0)
   {
     case SYNC_BREAK_WAIT:
          if( (isr_type == TX_INT) )
          {
             dummy = data;
             if(dummy == 0u)
             {
                Lin_State0 = SYNC_WAIT;
                Send_Byte(0x55u);
             }
             else
             {
                Lin_State0 = SYNC_BREAK_WAIT;
             }            
          }
          break;	  
     case SYNC_WAIT:
          if( (isr_type == RX_INT) )
          {
             dummy = data;
             if(dummy == 0x55u)
             {
                id_index = Get_Next_Transmit_Index();
                if(id_index < NO_OF_TX_ID)
                {
                   for(k=0;k<Lin_Tx_Msg[id_index].len;k++)
                   {
                      lin_tx_data[id_index].transmit_data[k] = Lin_Tx_Msg[id_index].buf_ptr[k];
                   }
                   tx_data = Get_Transmit_ID(id_index);
                   Send_Byte(tx_data);
                   Lin_State0 = ID_WAIT;
                }
                else
                {
                   Lin_State0 = SYNC_BREAK_WAIT;
                }
             }
             else
             {
                Lin_State0 = SYNC_BREAK_WAIT;
             }
          }
          break;
     case ID_WAIT:
          if( (isr_type == RX_INT) )
          {
             dummy = data;
             for(id=0;id<NO_OF_TX_ID;id++)
             {
                if(dummy == Get_Transmit_ID(id))
                {
                   tx_data_cnt=0u;
                   Lin_State0 = DATA_WAIT;
                   tx_data = lin_tx_data[id_index].transmit_data[tx_data_cnt];
                   Send_Byte(tx_data);
                   break;
                }
             }     
             if((Lin_State0 != DATA_WAIT) && 
                (Lin_State0 != WAIT_TX_DATA))
             {
                Lin_State0 = SYNC_BREAK_WAIT;
             }            
          }
          break;
     case DATA_WAIT:
          if( (isr_type == RX_INT) )
          {
             /* Waited for reception completion of Data */
             dummy = data;
             if(tx_data_cnt < (Lin_Tx_Msg[id_index].len-1))
             {
                tx_data_cnt++;
                tx_data = lin_tx_data[id_index].transmit_data[tx_data_cnt];
                if((id_index == 0u) && (tx_data == 0u) && (tx_data_cnt > 2u))
                {
                   tx_data = 0xFFu;
                   lin_tx_data[id_index].transmit_data[tx_data_cnt] = 0xFFu;
                }
                Send_Byte(tx_data);
             }
             else
             {
                temp_cksum = LIN_Calc_Checksum((unsigned char*)&lin_tx_data[id_index],(Get_Tx_Length(id_index)+1u));
                lin_tx_data[id_index].checksum = temp_cksum;
                Send_Byte(temp_cksum);
                Lin_State0 = CHSKUM_WAIT;
             }
          }
          break;
     case CHSKUM_WAIT:
          if( (isr_type == RX_INT) )
          {
             dummy = data;
             if(temp_cksum == dummy)
             {
                Lin_State0 = SYNC_BREAK_WAIT;            
                Lin_Next_Transmit_Msg_Timer0 = LIN_NEXT_MSG_PERIODIC_CNT;
                App_Notify_Frame_Transmitted_USART0(id_index);
             }
             else
             {
                 Lin_State0 = SYNC_BREAK_WAIT;            
             }                
          }
          break;
     default:
          break;
   }
}

/**********************************************************************************
Function Name:  
Description:    
Parameters:     none
Return value:   none
***********************************************************************************/
void LIN_Send_Wakeup_Signal0(void)
{
   // Send one break symbol
   UF0BTT = 1u;
}



/**********************************************************************************
Function Name:  
Description:    
Parameters:     none
Return value:   none
***********************************************************************************/
unsigned char LIN_Calc_Checksum(unsigned char *data,unsigned char length)
{
   unsigned int sum = 0u;

   // Unrolled loop for speed (optional for small fixed lengths)
   while(length--)
   {
      sum += *data++;
   }
   
   // Fold carry once (faster than per-byte check)
   sum = (sum & 0xFF) + (sum >> 8);
   // Final inversion
   return (unsigned char)(~sum);
}


/**********************************************************************************
Function Name:  
Description:    
Parameters:     none
Return value:   none
***********************************************************************************/
static void LIN_Response_Transmit_Delay_for_Renesas( void )
{
   u8 cnt;

   // Delay count of 15 assures about 60 microseconds of time for the hosts 50 microsecond stop bit
   cnt = 50u;
   while(cnt > 0u)
   {
      cnt--;
   }
} 



/**********************************************************************************
Function Name:  USART1 Interrupt(ILIN)
Description:    LIN sensor data 
Parameters:     none
Return value:   none
***********************************************************************************/
void LIN_Task_USART1_Interrupt(u8 data)
{ 
   static unsigned char dummy;
   static unsigned char id_receive;
   static unsigned char cksum;
   static unsigned char temp_cksum;
   static unsigned char rx_data_cnt;
   static unsigned char id_index;
   static unsigned char id;
   static unsigned char byte;
   
   interByteTimer1 = 0u;

   switch(Lin_State)
   {
     case SYNC_BREAK_WAIT:
          Lin_State = SYNC_WAIT;
          dummy = data;

          rx_data_cnt=0u;
          byte=0u;
          id_index=0u;
          
          if(dummy == 0u)
          {
             Lin_State = SYNC_WAIT;
          }
          else
          {
             Lin_State = SYNC_BREAK_WAIT;
          }            
          break;
     case SYNC_WAIT:
          dummy = data;
          
          if( dummy == 0x55u)
          {
             Lin_State = ID_WAIT;
          }
          else
          {
             Lin_State = SYNC_BREAK_WAIT;
          }
          break;
     case ID_WAIT:
          /* ID Field */
          /* Waited for reception completion of ID Field  */
          id_receive = data;
          for(id=0;id<NO_OF_RX_ID;id++)
          {
             if(id_receive == Get_Receive_ID(id))
             {
                id_index = id;
                Lin_State = DATA_WAIT;
                rx_data_cnt = Get_Rx_Length(id_index);
                lin_rx_data[id_index].id_receive = id_receive;
                byte = 0;
             }
          }     
          if((Lin_State != DATA_WAIT) && 
             (Lin_State != WAIT_TX_DATA))
          {
             Lin_State = SYNC_BREAK_WAIT;
          }            
          break;
     case DATA_WAIT:
          /* Waited for reception completion of Data 1    */
          dummy = data;
          /* Reading of reception data                    */
          lin_rx_data[id_index].receive_data[byte] = dummy;

          byte++;
          if(byte == rx_data_cnt)
          {
             Lin_State = CHSKUM_WAIT;
          }
          break;
     case CHSKUM_WAIT:
          dummy = data;
          lin_rx_data[id_index].checksum = dummy;
          temp_cksum = LIN_Calc_Checksum((unsigned char*)&lin_rx_data[id_index],(rx_data_cnt+1u));
          if(temp_cksum == dummy)
          {
             Lin_State = SYNC_BREAK_WAIT;
             memcpy(Lin_Rx_Msg[id_index].buf_ptr,(unsigned char*)&lin_rx_data[id_index].receive_data,8u);
             App_Notify_Frame_Received_USART1(id_index);
          }
          else
          {
              Lin_State = SYNC_BREAK_WAIT;            
          }                
          break;
     default:
          break;
   }
}
