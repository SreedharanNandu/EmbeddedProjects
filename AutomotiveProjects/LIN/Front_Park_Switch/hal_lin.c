/***********************************************************************************
System Includes
***********************************************************************************/
#include <string.h>
#include <stdio.h>
/***********************************************************************************
User Includes
***********************************************************************************/
#include "pic12f1822.h"
#include "types.h"
#include "led.h"
#include "lin_cfg.h"
#include "hal_lin.h"

/***********************************************************************************
User Program Code Prototypes
***********************************************************************************/

/**********************************************************************************
Global definition
***********************************************************************************/

/**********************************************************************************
Global variables
***********************************************************************************/
volatile Lin_Data_t Lin_Data[NO_OF_LIN_IDS];
volatile Lin_State_t Lin_State;

unsigned char dummy;
unsigned char temp_cksum;
unsigned char data_cnt;
unsigned char byte;

/**********************************************************************************
Function Proto
***********************************************************************************/

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
   Lin_State = SYNC_BREAK_WAIT;
   Lin_Data[0].id = Get_Lin_Msg_ID(0u);
   Lin_Data[1].id = Get_Lin_Msg_ID(1u);
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
Function Name:  USART Interrupt
Description:    LIN sensor data 
Parameters:     none
Return value:   none
***********************************************************************************/
void LIN_Task_USART_Interrupt(unsigned char data)
{ 
   switch(Lin_State)
   {
     case SYNC_BREAK_WAIT:
          dummy = data;
          data_cnt=0u;
          byte=0u;
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
          dummy = data;
          if(dummy == Get_Lin_Msg_ID(0))//01
          {
             byte = 0;
             Lin_State = DATA_TRANSMIT;
             data_cnt = Get_Lin_Msg_Length(0);
             Lin_Data[0].id = dummy;
             dummy = Lin_Data[0].data[byte];
             LIN_Send_Data(dummy);
          }
          else
          {
             if(dummy == Get_Lin_Msg_ID(1))//02
             {
                Lin_State = DATA_RECEIVE;
                data_cnt = Get_Lin_Msg_Length(1);
                Lin_Data[1].id = dummy;
             }
             else
             {
                Lin_State = SYNC_BREAK_WAIT;
             }
          }     
          break;
     case DATA_RECEIVE:
          dummy = data;
          Lin_Data[1].data[byte] = dummy;
          if(byte < data_cnt)
          {
             byte++;
             if(byte == data_cnt)
             {
                 Lin_State = CHKSUM_RECEIVE;
             } 
          }
          break;
     case DATA_TRANSMIT:
          if((data_cnt) && (byte >= (data_cnt-1u)))
          {
             Lin_State = CHKSUM_TRANSMIT;
             temp_cksum = LIN_Calc_Checksum((unsigned char*)&Lin_Data[0],(data_cnt+1u));
             Lin_Data[0].checksum = temp_cksum;
             LIN_Send_Data(temp_cksum);
          }
          else
          {
             Lin_State = DATA_TRANSMIT;
             byte++;
             dummy = Lin_Data[0].data[byte];
             LIN_Send_Data(dummy);
          }
          break;
     case CHKSUM_RECEIVE:
          dummy = data;
          temp_cksum = LIN_Calc_Checksum((unsigned char*)&Lin_Data[1],(data_cnt+1u));
          if(temp_cksum == dummy)
          {
             Lin_Data[1].checksum = dummy;
             Lin_State = SYNC_BREAK_WAIT;
             memcpy(Lin_Msg[1].buf_ptr,(unsigned char*)&Lin_Data[1].data,8u);
             App_Notify_Frame_Received_USART(1);
          }
          else
          {
              Lin_State = SYNC_BREAK_WAIT;            
          }                
          break;
     case CHKSUM_TRANSMIT:
          dummy = data;
          if(temp_cksum == dummy)
          {
             Lin_State = SYNC_BREAK_WAIT;            
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

/**********************************************************************************
Function Name:  LIN_Send_Data
Description:    UART Tx 
Parameters:     none
Return value:   none
***********************************************************************************/
void LIN_Send_Data(unsigned char data)
{
   while(!TRMT);
   TXREG = data;
}

