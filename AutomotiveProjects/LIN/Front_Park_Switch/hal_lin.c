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
#include "app_lin.h"
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


/**********************************************************************************
Function Proto
***********************************************************************************/
static void LIN_Send_Data(unsigned char data);

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
   for(id = 0u;id<NO_OF_LIN_IDS;id++)
   {
      Lin_Data[id].id = Get_Lin_Msg_ID(id);
   }

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
    static unsigned char dummy;
    static unsigned char id_receive;
    static unsigned char temp_cksum;
    static unsigned char data_cnt;
    static unsigned char id_index;
    static unsigned char id;
    static unsigned char byte;
    static lin_msg_type_t id_type;


   switch(Lin_State)
   {
     case SYNC_BREAK_WAIT:
          dummy = data;
          data_cnt=0u;
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
          id_receive = data;
          for(id=0;id<NO_OF_LIN_IDS;id++)
          {
             if(id_receive == Get_Lin_Msg_ID(id))
             {
                byte = 0;
                id_index = id;
                id_type = Get_Lin_Msg_Type(id_index);
                if(id_type == RECEIVE_PAYLOAD)
                {
                   Lin_State = DATA_RECEIVE;
                   data_cnt = Get_Lin_Msg_Length(id_index);
                   Lin_Data[id_index].id = id_receive;
                }
                else if(id_type == TRANSMIT_PAYLOAD)
                {
                   Lin_State = DATA_TRANSMIT;
                   data_cnt = Get_Lin_Msg_Length(id_index);
                   Lin_Data[id_index].id = id_receive;
                   dummy = Lin_Data[id_index].data[byte];
                   LIN_Send_Data(dummy);
                }
                else
                {
                   Lin_State = SYNC_BREAK_WAIT;
                }
             }
          }     
          break;
     case DATA_RECEIVE:
          dummy = data;
          Lin_Data[id_index].data[byte] = dummy;
          if(byte == data_cnt)
          {
             Lin_State = CHKSUM_RECEIVE;
          }
          else
          {
             byte++;
          }
          break;
     case DATA_TRANSMIT:
          if((data_cnt) && (byte >= (data_cnt-1u)))
          {
             Lin_State = CHKSUM_TRANSMIT;
             temp_cksum = LIN_Calc_Checksum((unsigned char*)&Lin_Data[id_index],(data_cnt+1u));
             Lin_Data[id_index].checksum = temp_cksum;
             LIN_Send_Data(dummy);
          }
          else
          {
             byte++;
             dummy = Lin_Data[id_index].data[byte];
             LIN_Send_Data(dummy);
          }
          break;
     case CHKSUM_RECEIVE:
          dummy = data;
          temp_cksum = LIN_Calc_Checksum((unsigned char*)&Lin_Data[id_index],(data_cnt+1u));
          if(temp_cksum == dummy)
          {
             Lin_Data[id_index].checksum = dummy;
             Lin_State = SYNC_BREAK_WAIT;
             memcpy(Lin_Msg[id_index].buf_ptr,(unsigned char*)&Lin_Data[id_index].data,8u);
             App_Notify_Frame_Received_USART(id_index);
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
static void LIN_Send_Data(unsigned char data)
{
   while(!TRMT);
   TXREG = data;
}

