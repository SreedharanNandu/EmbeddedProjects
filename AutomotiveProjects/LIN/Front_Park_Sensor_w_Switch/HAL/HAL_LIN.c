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
u8 Lin_Next_Slot0;
u16 Lin_Periodic_Timer0;

lin_data_t lin_data0[NO_OF_ID0];
lin_data_t lin_data1[NO_OF_ID1];

volatile Lin_state_t Lin_State;
volatile Lin_state_t Lin_State0;

volatile u8 Lin1_Active; 
volatile u8 Lin1_Fault;
volatile u32 interByteTimer1;

volatile u8 Lin0_Active; 
volatile u8 Lin0_Fault;
volatile u32 interByteTimer0;

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
   for(id = 0u;id<NO_OF_ID0;id++)
   {
      lin_data0[id].id = Get_Tx_ID0(id);
   }

   for(id = 0u;id<NO_OF_ID1;id++)
   {
      lin_data1[id].id = Get_Rx_ID1(id);
   }

   Lin_State0 = SYNC_BREAK_WAIT;

   
   Lin_Next_Transmit_Msg_Timer0 = 0u;
   Lin_Next_Slot0 = 0u;
   LIN_Send_Wakeup_Signal0();
   interByteTimer0 = 0u;
   Lin0_Active = 1;
   
   
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
   static unsigned char rx_data_cnt;
   static unsigned char id_index;
   unsigned char id,k;
   unsigned char tx_data;

   
   interByteTimer0 = 0u;
   
   switch(Lin_State0)
   {
     case SYNC_BREAK_WAIT:
          if( (isr_type == TX_INT) )
          {
             dummy = data;
             if(dummy == 0u)
             {
                Lin_State0 = SYNC_WAIT;
                Send_Byte0(0x55u);
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
                id_index = Get_Next_Tx_Slot0();
                if(id_index < NO_OF_ID0)
                {
                   for(k=0;k<Lin_Tx_Msg0[id_index].len;k++)
                   {
                      lin_data0[id_index].data[k] = Lin_Tx_Msg0[id_index].buf_ptr[k];
                   }
                   tx_data = Get_Tx_ID0(id_index);
                   Send_Byte0(tx_data);
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
             for(id=0;id<NO_OF_ID0;id++)
             {
                if(dummy == Get_Tx_ID0(id))
                {
                   if(LIN_HDR_AND_DATA == Get_Tx_Type_ID0(id))
                   {
                      tx_data_cnt=0u;
                      Lin_State0 = DATA_WAIT_TX;
                      tx_data = lin_data0[id_index].data[tx_data_cnt];
                      Send_Byte0(tx_data);
                   }
                   else if(LIN_HDR_ONLY == Get_Tx_Type_ID0(id))
                   {
                      rx_data_cnt = 0;
                      Lin_State0 = DATA_WAIT_RX;
                   }
                   else
                   {
                      Lin_State0 = SYNC_BREAK_WAIT;
                   }
                }
             }     
          }
          break;
     case DATA_WAIT_TX:
          if( (isr_type == RX_INT) )
          {
             /* Waited for reception completion of Data */
             dummy = data;
             if(tx_data_cnt < (Lin_Tx_Msg0[id_index].len-1))
             {
                tx_data_cnt++;
                tx_data = lin_data0[id_index].data[tx_data_cnt];
                Send_Byte0(tx_data);
             }
             else
             {
                temp_cksum = LIN_Calc_Checksum((unsigned char*)&lin_data0[id_index],(Get_Tx_Length0(id_index)+1u));
                lin_data0[id_index].checksum = temp_cksum;
                Send_Byte0(temp_cksum);
                Lin_State0 = CHSKUM_WAIT_TX;
             }
          }
          break;
     case DATA_WAIT_RX:
          if( (isr_type == RX_INT) )
          {
             dummy = data;
             if(rx_data_cnt < Lin_Tx_Msg0[id_index].len)
             {
                lin_data0[id_index].data[rx_data_cnt] = dummy;
                rx_data_cnt++;
             }
             else
             {
                temp_cksum = LIN_Calc_Checksum((unsigned char*)&lin_data0[id_index],(Get_Tx_Length0(id_index)+1u));
                lin_data0[id_index].checksum = temp_cksum;
                Lin_State0 = CHSKUM_WAIT_RX;
             }
          }
          break;
     case CHSKUM_WAIT_TX:
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
     case CHSKUM_WAIT_RX:
          if( (isr_type == RX_INT) )
          {
             dummy = data;
             if(temp_cksum == dummy)
             {
                Lin_State0 = SYNC_BREAK_WAIT;            
                Lin_Next_Transmit_Msg_Timer0 = LIN_NEXT_MSG_PERIODIC_CNT;
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
          for(id=0;id<NO_OF_RX_ID1;id++)
          {
             if(id_receive == Get_Rx_ID1(id))
             {
                id_index = id;
                Lin_State = DATA_WAIT_RX;
                rx_data_cnt = Get_Rx_Length1(id_index);
                lin_data1[id_index].id = id_receive;
                byte = 0;
             }
          }     
          if(Lin_State != DATA_WAIT_RX)
          {
             Lin_State = SYNC_BREAK_WAIT;
          }            
          break;
     case DATA_WAIT_RX:
          /* Waited for reception completion of Data 1    */
          dummy = data;
          /* Reading of reception data                    */
          lin_data1[id_index].data[byte] = dummy;

          byte++;
          if(byte == rx_data_cnt)
          {
             Lin_State = CHSKUM_WAIT_RX;
          }
          break;
     case CHSKUM_WAIT_RX:
          dummy = data;
          lin_data1[id_index].checksum = dummy;
          temp_cksum = LIN_Calc_Checksum((unsigned char*)&lin_data1[id_index],(rx_data_cnt+1u));
          if(temp_cksum == dummy)
          {
             Lin_State = SYNC_BREAK_WAIT;
             memcpy(Lin_Rx_Msg1[id_index].buf_ptr,(unsigned char*)&lin_data1[id_index].data,8u);
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
