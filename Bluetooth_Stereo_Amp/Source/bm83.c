
/*
 * bm83.c
 */


#include "bm.h"
#include "uart.h"
#include "system.h"
#include "p30f3010.h"
#include "ports.h"


unsigned short int uart_pkt_timeout = UART_RESET_TIMER;
unsigned char curr_state = SYNC_BYTE_RX_STATE;
unsigned char rx_complete = FALSE;

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Uart_Timeout_Check(void)
{   
   if((curr_state != SYNC_BYTE_RX_STATE) && //reception in progress
      (uart_pkt_timeout > 0u))
   {
      uart_pkt_timeout--;
      if(0u == uart_pkt_timeout)
      {
         curr_state = SYNC_BYTE_RX_STATE;
      }
   }
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Assemble_Uart_Data(unsigned char data)
{
   unsigned char cnt=0;
   unsigned char checksum = 0;
   static unsigned char len_cnt=0;
   static Bm83_t *ctx = &Bm83;
   static unsigned char evt_buf[4] = { 0 };

   
   
   switch(curr_state)
   {
      case SYNC_BYTE_RX_STATE:
           if(BM83_START_HEADER == data)
           {
              evt_buf[0] = data;
              evt_buf[1] = evt_buf[2] = evt_buf[3] = 0;
              len_cnt = 0;
              curr_state = LEN_STATE_RX_STATE1;
              uart_pkt_timeout = UART_RESET_TIMER;
           }
           break; 
      case LEN_STATE_RX_STATE1:
           curr_state = LEN_STATE_RX_STATE2;
           evt_buf[1] = data;
           if(uart_pkt_timeout > 0u)
           {
              uart_pkt_timeout--;
           }
           break; 
      case LEN_STATE_RX_STATE2:
           evt_buf[2] = data;
           curr_state = OPCODE_RX_STATE;
           ctx->event_packet.param_len = (unsigned short int) (((unsigned short int) evt_buf[1] << 8) | evt_buf[2]) - 1;//length byte1,2
           if(uart_pkt_timeout > 0u)
           {
              uart_pkt_timeout--;
           }
           break; 
      case OPCODE_RX_STATE:
           curr_state = DATA_RX_STATE;//op code is 0 for event ack,do not confuse
           evt_buf[3] = data;
           ctx->event_packet.opcode = evt_buf[3];//op code byte3
           if(uart_pkt_timeout > 0u)
           {
              uart_pkt_timeout--;
           }
           break; 
      case DATA_RX_STATE:
           if(len_cnt < ctx->event_packet.param_len)
           {
              ctx->event_packet.param_buf[len_cnt] = data;
              len_cnt++;
              curr_state = DATA_RX_STATE;
           }
           if(len_cnt == ctx->event_packet.param_len)
           {
              len_cnt=0;
              curr_state = CHECKSUM_RX_STATE;
           }
           if(uart_pkt_timeout > 0u)
           {
              uart_pkt_timeout--;
           }
           break; 
      case CHECKSUM_RX_STATE:
           checksum = evt_buf[1] + evt_buf[2] + evt_buf[3];
           for(cnt=0;cnt<ctx->event_packet.param_len;cnt++)
           {
              checksum += ctx->event_packet.param_buf[cnt];//rest of byes as per length
           }
           checksum = ~checksum + 1;//not or ~(len+command+data) 
           curr_state = SYNC_BYTE_RX_STATE;
           if(checksum == data)
           {
              rx_complete = TRUE;
           }
           if(uart_pkt_timeout > 0u)
           {
              uart_pkt_timeout--;
           }
           break;
      default:
           curr_state = SYNC_BYTE_RX_STATE;
           uart_pkt_timeout=0;
           break;
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Generic_Write(unsigned char *data_buf,unsigned short int len) 
{
   UARTSend(data_buf,len);
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Set_MFB_Pin(unsigned char state)
{
  RB2 = 1;
  Delay_Ms(400);
  RB2 = 0;
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Set_RST_pin(unsigned char state)
{
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Mute(void)
{
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_UnMute(void)
{

}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Send_Command(Bm83_t *ctx)
{
   unsigned char cmd_buf[4] = { 0 };
   unsigned char package_len = ctx->command_packet.param_len + 1;
   unsigned short int cnt =0;
   
   cmd_buf[0] = BM83_START_HEADER;
   cmd_buf[1] = (unsigned char) ((package_len >> 8) & 0xFF);
   cmd_buf[2] = (unsigned char) (package_len & 0xFF);
   cmd_buf[3] = ctx->command_packet.opcode;
   
   Delay_Ms(10);
   Bm83_Generic_Write(cmd_buf,4);
   unsigned char checksum = cmd_buf[1] + cmd_buf[2] + cmd_buf[3];
   if(ctx->command_packet.param_len) 
   {
       Bm83_Generic_Write(ctx->command_packet.param_buf,ctx->command_packet.param_len);
       for(cnt = 0; cnt < ctx->command_packet.param_len; cnt++)
       {
           checksum += ctx->command_packet.param_buf[cnt];
       }
   }    
   else
   {
   
   }
   checksum = ~checksum + 1;
   Bm83_Generic_Write(&checksum,1);
   Delay_Ms(10);
}








