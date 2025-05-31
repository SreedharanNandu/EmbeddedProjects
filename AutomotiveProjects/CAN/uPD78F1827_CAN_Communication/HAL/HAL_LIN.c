/***********************************************************************************
HAL_LIN.c
***********************************************************************************/

/***********************************************************************************
User Includes
***********************************************************************************/

#include "string.h"


/***********************************************************************************
User Program Code Prototypes
***********************************************************************************/

/**********************************************************************************
Global definisition
***********************************************************************************/

/**********************************************************************************
Global variables
***********************************************************************************/


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
void LIN_Initialize( void )
{
   unsigned char id;

   Lin_State = SYNC_BREAK_WAIT;
   LIN_Sleep_Timer = LIN_Sleep_Timer_Reset;
   LIN_Wakeup_Timer = LIN_Wakeup_Timer_Expired;
   LIN_Wakeup_Count = LIN_Wakeup_Count_Expired;
   LIN_Wakeup_Pulses = 0u;

   for(id = 0;id<NO_OF_RX_ID;id++)
   {
      lin_rx_data[id].id_receive = Get_Receive_ID(id);
   }


   for(id = 0;id<NO_OF_TX_ID;id++)
   {
      lin_tx_data[id].id_transmit = Get_Transmit_ID(id);
   }
}

/**********************************************************************************
Function Name:  LIN_Task_Interrupt
Description:    LIN Rx
Parameters:     none
Return value:   none
***********************************************************************************/
void LIN_Task_USART0_Interrupt(u8 data)
{
    static unsigned char dummy;
    static unsigned char id_receive;
    static unsigned char cksum,temp_cksum=0u;
    static unsigned char tx_data_cnt,
                        rx_data_cnt,
                        id_index,id,
                        byte;
   
   switch(Lin_State)
   {
     case SYNC_BREAK_WAIT:
          Lin_State = SYNC_WAIT;
          dummy = data;

          tx_data_cnt=0u;
          rx_data_cnt=0u;
          byte=0u;
          id_index=0u;
          
          if(dummy == 0u)
          {
             Lin_State = SYNC_WAIT;
             App_Notify_Sync_Break_Field();
          }
          else
          {
             Lin_State = SYNC_BREAK_WAIT;
             App_Notify_Sync_Break_Error();
          }            
          break;
     case SYNC_WAIT:
          dummy = data;
          
          if( dummy == 0x55u)
          {
             Lin_State = ID_WAIT;
             App_Notify_Sync_Field();
          }
          else
          {
             Lin_State = SYNC_BREAK_WAIT;
             App_Notify_Sync_Field_Error();
          }
          break;
     case ID_WAIT:
          /* ID Field */
          /* Waited for reception completion of ID Field  */
          id_receive = data;
          id = 0;
          for(id=0;id<NO_OF_RX_ID;id++)
          {
             if(id_receive == Get_Receive_ID(id))
             {
                id_index = id;
                Lin_State = DATA_WAIT;
                rx_data_cnt = Get_Rx_Length(id_index);
                lin_rx_data[id_index].id_receive = id_receive;
                byte = 0;
                App_Notify_Rx_ID_Field(id_index);
             }
          }     
          if(Lin_State != DATA_WAIT) 
          {
             //id = 0u;
             for(id=0;id<NO_OF_TX_ID;id++)
             {
                if(id_receive == Get_Transmit_ID(id))
                {
                   id_index = id;
                   tx_data_cnt = Get_Tx_Length(id_index);
                   byte = 0;
                   lin_tx_data[id_index].id_transmit = id_receive;
                   App_Notify_Tx_ID_Field(id_index);
                   memcpy((unsigned char*)&lin_tx_data[id_index].transmit_data[byte],(u8*)Lin_Tx_Msg[id_index].buf_ptr,Lin_Tx_Msg[id_index].len);
                   /*load the first byte*/
                   Send_Byte(lin_tx_data[id_index].transmit_data[byte]);
                   Lin_State = WAIT_TX_DATA;
                }
             }
          }
          if((Lin_State != DATA_WAIT) && 
             (Lin_State != WAIT_TX_DATA))
          {
             Lin_State = SYNC_BREAK_WAIT;
             App_Notify_ID_Field_Error();
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
     case CONF_SEND:
          /* Waited for reception completion of Data     */
          dummy = data;
          /* Reading of reception data                    */
          if(dummy == lin_tx_data[id_index].transmit_data[byte])
          {
             byte++;
             if(byte<tx_data_cnt)/* Transmission of a message permission */
             {
                /*load the first byte*/
                Send_Byte(lin_tx_data[id_index].transmit_data[byte]);
                Lin_State = WAIT_TX_DATA;
             }
             else
             {
                byte = 0;
                if(lin_tx_data[id_index].id_transmit != 0x7Du)
                {
                   cksum = LIN_Calc_Checksum((unsigned char*)&lin_tx_data[id_index],(tx_data_cnt+1u));
                   Send_Byte(cksum);
                }
                else
                {
                   cksum = LIN_Calc_Checksum((unsigned char*)&lin_tx_data[id_index].transmit_data[0u],(tx_data_cnt));
                   Send_Byte(cksum);
                }
                Lin_State = WAIT_TX_CHKSUM;
                App_Notify_Transmit_Data_Field_Complete(id_index);
             }
          }
          else
          {
             Lin_State = SYNC_BREAK_WAIT;
             App_Notify_Data_Error(id_index);
          }            
          break;
     case CHSKUM_WAIT:
          dummy = data;
          lin_rx_data[id_index].checksum = dummy;
          if(lin_rx_data[id_index].id_receive != 0x3Cu)
          {
             temp_cksum = LIN_Calc_Checksum((unsigned char*)&lin_rx_data[id_index],(rx_data_cnt+1u));
          }
          else
          {
             temp_cksum = LIN_Calc_Checksum((unsigned char*)&lin_rx_data[id_index].receive_data,(rx_data_cnt));
          }
          if(temp_cksum == dummy)
          {
             Lin_State = SYNC_BREAK_WAIT;
             memcpy((u8*)Lin_Rx_Msg[id_index].buf_ptr,(unsigned char*)&lin_rx_data[id_index].receive_data,8u);
             if(lin_rx_data[id_index].id_receive == LIN_DIAG_REQ_ID)
             {
                App_Notify_Diag_Req_Received();
             }
             else
             {
                App_Notify_Frame_Received(id_index);
             }

          }
          else
          {
              Lin_State = SYNC_BREAK_WAIT;            
              App_Notify_Receive_Checksum_Error(id_index);
          }                
          // Reset sleep timer now that we've transmitted a valid response
          LIN_Sleep_Timer = LIN_Sleep_Timer_Reset;
          // reset the wakeup count to say that we're awake!
          LIN_Wakeup_Count = LIN_Wakeup_Count_Expired;
          break;
     case CONF_CHKSUM:
          dummy = data;
          /* Waited for reception completion of Data     */
          /* Reading of reception data                    */
          if(dummy == cksum)
          {
             Lin_State = SYNC_BREAK_WAIT;
          }
          else
          { 
             Lin_State = SYNC_BREAK_WAIT;
          }// Reset sleep timer now that we've transmitted a valid response
          LIN_Sleep_Timer = LIN_Sleep_Timer_Reset;
          // reset the wakeup count to say that we're awake!
          LIN_Wakeup_Count = LIN_Wakeup_Count_Expired;
          break;
     case WAIT_TX_DATA:
          Lin_State = CONF_SEND;
          break;
     case CHSKUM_SEND:
          break;
     case WAIT_TX_CHKSUM:
          Lin_State = CONF_CHKSUM;
          break;
     default:
          break;
   }
}

void LIN_Send_Wakeup_Signal(void)
{
   // Send one break symbol
   UF0BTT = 1u;
}


unsigned char LIN_Calc_Parity(unsigned char id)   
{   
   unsigned char parity, p0,p1;   
   
   parity=id;    
   p0=(BIT(parity,0)^BIT(parity,1)^BIT(parity,2)^BIT(parity,4))<<6;   
   p1=(~(BIT(parity,1)^BIT(parity,3)^BIT(parity,4)^BIT(parity,5)))<<7;   
   parity|=(p0|p1);   
   return parity;   
   
}  
unsigned char LIN_Calc_Checksum(unsigned char *data,unsigned char length)
{

   unsigned int sum = 0;
   unsigned char i;

   for(i = 0; i < length; i++)
   {
      sum += data[i];
      if(sum & 0xFF00)
      {
         // Adds carry
         sum = (sum&0x00FF) + 1;
      }
   }
   sum ^= 0x00FF;
   return (unsigned char)sum;
  
}

void LIN_Bus_Wakeup(void)
{
   LIN_Send_Wakeup_Signal();
   LIN_Wakeup_Count = LIN_Wakeup_Count_Start;
   LIN_Wakeup_Count--; // decrement by 1 because we just sent 1!
   LIN_Wakeup_Timer = LIN_Wakeup_Timer_Periods_Normal;
   LIN_Sleep_Timer = LIN_Sleep_Timer_Reset;
   LIN_Wakeup_Pulses = 6u;
   LIN_Wakeup_Pulses--;
}
/*
 Perform the periodic LIN processes at 4 millisecond rate
*/
void LIN_Fast_Periodic_Task( void )
{
   // update the sleep timer
   if(LIN_Sleep_Timer > LIN_Sleep_Timer_Expired)
   {
      LIN_Sleep_Timer--;

      if (LIN_Sleep_Timer == LIN_Sleep_Timer_Expired)
      {
          LIN_Bus_Wakeup();
      }
      else
      {
      }
   }

   // check if we're trying to wakeup the bus
   if (LIN_Wakeup_Count > LIN_Wakeup_Count_Expired)
   {
      // we're waiting to wakeup the bus
      if (LIN_Wakeup_Timer > LIN_Wakeup_Timer_Expired)
      {
          LIN_Wakeup_Timer--;
      }
      else
      {
          if (LIN_Wakeup_Pulses > 0u)
          {
              LIN_Wakeup_Pulses--;

              LIN_Send_Wakeup_Signal();

              if (LIN_Wakeup_Count > LIN_Wakeup_Count_Burst_Complete)
              {
                  LIN_Wakeup_Count--;
                  // wait 200ms before sending the next wakeup signal
                  LIN_Wakeup_Timer = LIN_Wakeup_Timer_Periods_Normal;
              }
              else
              {
                  // wait 1.5 seconds before sending another burst of wakeup signals
                  LIN_Wakeup_Timer = LIN_Wakeup_Timer_Periods_Wait;
                  LIN_Wakeup_Count = LIN_Wakeup_Count_Start;
              }
          }
      }
   }
   else
   {
       // the bus is awake

       // check for interbyte timeouts
       if (LIN_InterByte_Timer > LIN_InterByte_Time_Expired)
       {
           LIN_InterByte_Timer--;
           if (LIN_InterByte_Timer == LIN_InterByte_Time_Expired)
           {
              Lin_State = SYNC_BREAK_WAIT;
              
           }
       }
   }
}
//-----------------------------------------------------------------------------
//
// Delay 1 microsecond (16 nop's at 1 clock per instruction and 16 MHz clock = 1 microsecond)
#define Delay_1_Microsecond     __asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n ")
//
//-----------------------------------------------------------------------------
//
static void LIN_Response_Transmit_Delay_for_Renesas( void )
{
    u8 Transmit_Delay_Count;

    // Delay count of 15 assures about 60 microseconds of
    // time for the hosts 50 microsecond stop bit
    Transmit_Delay_Count = 15u;

    while ( Transmit_Delay_Count > 0u )
    {
        Delay_1_Microsecond;
        Transmit_Delay_Count--;
    } // end while
} // end of function LIN_Response_Transmit_Delay_for_Renesas
