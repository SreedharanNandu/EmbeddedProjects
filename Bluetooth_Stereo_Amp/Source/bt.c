/*
 * bt.c
 */


#include "BM.h"
#include <string.h>
#include "led.h"


Bm83_t Bm83;
Bt_State_T Bt_State;


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Init(void)
{
   Bm83_Default_Cfg(&Bm83);
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Task(void) 
{
   Bm83_Uart_Timeout_Check();
   if(TRUE == rx_complete)
   {
      Bm83_Event_Handler(&Bm83);//process the packet received 
      rx_complete = FALSE;
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : first checks received event from BT83 ,then sends command to BT83
******************************************************************************/
unsigned char Bm83_Event_Handler(Bm83_t *ctx)
{
   unsigned char error_flag = TRUE;
   static Bm83_eq_mode_t eq_mode = BTAUDIO2_EQ_MODE_OFF;
   switch(ctx->event_packet.opcode)
   {
      case BM83_EVT_BTM_STATE:
           switch(ctx->event_packet.param_buf[0])
           {
               case BM83_BTM_STATE_A2DP_LINK_ESTABLISHED:
                    if(Power_Mode == PWR_ON)
                    {
                       Led(blink_a2dpConnect);
                       Led(blink_PwnOn);
                    }
                    break;
               case BM83_BTM_STATE_AVRCP_LINK_DISCONNECTED:
                    break;
               case BM83_BTM_STATE_ACL_DISCONNECTED:
                    break;
               case BM83_BTM_STATE_A2DP_LINK_DISCONNECTED:
                    if(Power_Mode == PWR_ON)
                    {
                       Led(blink_a2dpConnect);
                       Led(blink_PwnOn);
                    }
                    break;
               case BM83_BTM_STATE_STANDBY_STATE:
                    if(Power_Mode == PWR_ON)
                    {
                       Bm83_Enter_Pairing_Mode(&Bm83);
                    }
                    break;
               case BM83_BTM_STATE_PAIRING_SUCCESSFULL:
               case BM83_BTM_STATE_SCO_LINK_CONNECTED:
               case BM83_BTM_STATE_SCO_LINK_DISCONNECTED:
               case BM83_BTM_STATE_ACL_CONNECTED:
               case BM83_BTM_STATE_UNKNOWN_AUDIO_SOURCE:
               case BM83_BTM_STATE_AUX_IN_AUDIO_SOURCE:
               case BM83_BTM_STATE_A2DP_AUDIO_SOURCE:
               default:
                    break;
           }
           break;
      default: 
           break;	
   }
   return error_flag;
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Default_Cfg(Bm83_t *ctx) 
{
   Bm83_Set_MFB_Pin(1);
   Bm83_UnMute();
}




/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Clear_Events(Bm83_t *ctx)
{
   ctx->event_packet.opcode = 0;
   memset(ctx->event_packet.param_buf,0,256);
   ctx->event_packet.param_len = 0;
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Send_Mmi_Action(Bm83_t *ctx,unsigned char database_index,unsigned char action)
{
   ctx->command_packet.opcode = BM83_CMD_MMI_ACTION;
   ctx->command_packet.param_buf[0] = database_index;
   ctx->command_packet.param_buf[1] = action;
   ctx->command_packet.param_len = 2;
   Bm83_Send_Command(ctx);
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Power_On_Device(Bm83_t *ctx)
{
   Bm83_Clear_Events(ctx);
   Bm83_Send_Mmi_Action(ctx,0,BM83_MMI_POWER_ON_BUTTON_PRESS);
   Delay_Ms(10);
   Bm83_Send_Mmi_Action(ctx,0,BM83_MMI_POWER_ON_BUTTON_RELEASE);
    
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Power_Off_Device(Bm83_t *ctx)
{
   Bm83_Clear_Events(ctx);
   Bm83_Send_Mmi_Action(ctx,0,BM83_MMI_POWER_OFF_BUTTON_PRESS);
   Delay_Ms(10);
   Bm83_Send_Mmi_Action(ctx,0,BM83_MMI_POWER_OFF_BUTTON_RELEASE);
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Enter_Pairing_Mode(Bm83_t *ctx)
{
   Bm83_Clear_Events(ctx);
   Bm83_Send_Mmi_Action(ctx,0,BM83_MMI_ENTER_PAIRING_MODE);
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Music_Control( Bm83_t *ctx,Bm83_music_control_t action)
{
   ctx->command_packet.opcode = BM83_CMD_MUSIC_CONTROL;
   ctx->command_packet.param_buf[0] = 0x00;
   ctx->command_packet.param_buf[1] = action;
   ctx->command_packet.param_len = 2;
   Bm83_Send_Command(ctx);
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Volume_Up(Bm83_t *ctx,unsigned char database_index,unsigned char mask)
{
   ctx->command_packet.opcode = BM83_CMD_SET_OVERALL_GAIN;
   ctx->command_packet.param_buf[0] = database_index;
   ctx->command_packet.param_buf[1] = mask;
   ctx->command_packet.param_buf[2] = 0x01;
   ctx->command_packet.param_buf[3] = 0x00;
   ctx->command_packet.param_buf[4] = 0x00;
   ctx->command_packet.param_buf[5] = 0x00;
   ctx->command_packet.param_len = 6;
   Bm83_Send_Command(ctx);
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Volume_Down(Bm83_t *ctx,unsigned char database_index,unsigned char mask)
{
   ctx->command_packet.opcode = BM83_CMD_SET_OVERALL_GAIN;
   ctx->command_packet.param_buf[0] = database_index;
   ctx->command_packet.param_buf[1] = mask;
   ctx->command_packet.param_buf[2] = 0x02;
   ctx->command_packet.param_buf[3] = 0x00;
   ctx->command_packet.param_buf[4] = 0x00;
   ctx->command_packet.param_buf[5] = 0x00;
   ctx->command_packet.param_len = 6;
   Bm83_Send_Command(ctx);
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Bm83_Set_Eq_Mode(Bm83_t *ctx,Bm83_eq_mode_t eq_mode)
{
   ctx->command_packet.opcode = BM83_CMD_EQ_MODE_SETTING;
   ctx->command_packet.param_buf[0] = eq_mode;
   ctx->command_packet.param_buf[1] = 0x00;
   Bm83_Send_Command(ctx);
}

