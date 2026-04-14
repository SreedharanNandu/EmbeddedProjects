/*----------------------------------------------------------------------------
 * Name:   radio.c
 *----------------------------------------------------------------------------*/

/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/


/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/

#include <stdio.h>
#include "type.h"
#include <msp430.h>
#include<msp430g2553.h>
/*****************************************************************************
*                                 Macro Definitions                          *
******************************************************************************/
#define RADIO_START_SEC_VAR
#include "memmap_radio.h"

#define Amp_Enable()    \
                     P2OUT |= BIT3;

#define Amp_Disable()    \
                     P2OUT &= (~BIT3);

/*****************************************************************************
*                                 Type Declarations                          *
******************************************************************************/
unsigned char App_Si_EE_Check_Pending;
unsigned char power_state;
unsigned char monitor_mode;
volatile si4703_read_reg_T si4703_read_reg;
volatile radio_T radio_state;
volatile unsigned char Is_Manual_Tune;
volatile unsigned char Is_Vol_Changed;
volatile unsigned char preset_mode;
unsigned short int fm_freq,prev_fm_freq;
unsigned char preset_index;
unsigned char si_vol;
unsigned char fm_stereo;
unsigned short int K_Radio_Data_Read[MAX_CH+1u];
unsigned short int K_Radio_Data_Write[MAX_CH+1u];
unsigned short int reset_Vol_Timer;
unsigned char disp_timer;
unsigned char prev_fm_stereo;
unsigned char Scan_Cnt;
unsigned char first_time_power_up;

static void Validate_FM_Data(void);


#define RADIO_STOP_SEC_VAR
#include "memmap_radio.h"

/*****************************************************************************
*                            Function declarations                          *
******************************************************************************/


/*****************************************************************************
*                    Globally  accessed Variable Declarations    
******************************************************************************/
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Init_Si(void)
{
  /*
   To  use  this  busmode
   selection  method,  the  GPIO3  and  SDIO  pins  must  be
   sampled low by the device on the rising edge of RST.
   
   The user may either drive the GPIO3 pin low externally,
   or leave the pin floating. If the pin is not driven by the
   user,  it  will be pulled low  by an  internal  1 M?  resistor
   which  is  active  only  while  RST  is  low.  The  user  must
   drive the SEN and SDIO pins externally to the proper
   state.
   
   To  select  2-wire  operation,  the  SEN  pin  must  be
   sampled high by the device on the rising edge of RST.   */
   
   /*MSP430 pin12 - RST - P2.4*/
   /*MSP430 pin13 - RSEN - P2.5*/
   /*MSP430 pin14 - SCL - P1.6*/
   /*MSP430 pin15 - SDA - P1.7*/

   P1SEL &= ~BIT7;      
   P1SEL2 &= ~BIT7;     
   P1DIR |= BIT7;       
   P1OUT &= ~BIT7;      
   P2DIR |= (BIT4 | BIT5);

   //SDI (SDA) must be LOW as per data sheet 
   Delay_Ms(200u);
   //si in reset state
   si4703_Rst(0u);
   //make RSEN High
   si4703_Rsen(1u);
   Delay_Ms(200u);
   //out of reset    
   si4703_Rst(1u);
   Delay_Ms(200u);

   /*Note: This pin functionality change from GPIO to I2c here is important dont place it before*/
   P1SEL |= BIT6 + BIT7; 
   P1SEL2|= BIT6 + BIT7; 

   InitI2C(SI_IC_ADDRESS);
   
   //Reconfigure delay for I2C
   Delay_Ms(300u);
   Power_Radio(1);
   Is_Manual_Tune = 0u;
   power_state = ON_STATE;
   Delay_Ms(300u);

}



/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Init_Radio(void)
{
   unsigned char i;

   for(i=0;i<MAX_CH;i++)
   {
      K_Radio_Data_Read[i] = K_Fm_Min_Freq;
      K_Radio_Data_Write[i] = K_Fm_Min_Freq;
   }
   K_Radio_Data_Read[MAX_CH] = 0u;
   K_Radio_Data_Write[MAX_CH] = 0u;
   
   radio_state = RADIO_INIT;
   disp_timer = DISP_RELOAD;
   Init_Si();
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
unsigned char Write_si4703(void)
{
   volatile unsigned char status;
   volatile unsigned char data_wr[12];
   
   data_wr[0] = si4703_read_reg.powercfg>>8u;
   data_wr[1] = (unsigned char)si4703_read_reg.powercfg;
   data_wr[2] = si4703_read_reg.channel>>8u;
   data_wr[3] = (unsigned char)si4703_read_reg.channel;
   data_wr[4] = si4703_read_reg.sysconfig1>>8u;
   data_wr[5] = (unsigned char)si4703_read_reg.sysconfig1;
   data_wr[6] = si4703_read_reg.sysconfig2>>8u;
   data_wr[7] = (unsigned char)si4703_read_reg.sysconfig2;
   data_wr[8] = si4703_read_reg.sysconfig3>>8u;
   data_wr[9] = (unsigned char)si4703_read_reg.sysconfig3;
   data_wr[10] = si4703_read_reg.test1>>8u;
   data_wr[11] = (unsigned char)si4703_read_reg.test1;
   
   InitI2C(SI_IC_ADDRESS);
   status = Write_Si_I2C((unsigned char*)&data_wr[0],12u);
   if(status)
   {   
      Delay_Ms(10u); 
   }
   return status;
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
unsigned char Read_si4703(void)
{
   volatile unsigned char status;
   volatile unsigned char data_rd[32];

   InitI2C(SI_IC_ADDRESS);
   status  = Read_Si_I2C((unsigned char*)&data_rd[0],32u);

   si4703_read_reg.statusrssi     = (data_rd[0]<<8)|data_rd[1]; 
   si4703_read_reg.readchan       = (data_rd[2]<<8)|data_rd[3]; 
   si4703_read_reg.rdsa           = (data_rd[4]<<8)|data_rd[5]; 
   si4703_read_reg.rdsb           = (data_rd[6]<<8)|data_rd[7]; 
   si4703_read_reg.rdsc           = (data_rd[8]<<8)|data_rd[9]; 
   si4703_read_reg.rdsd           = (data_rd[10]<<8)|data_rd[11]; 
   si4703_read_reg.device_id      = (data_rd[12]<<8)|data_rd[13]; 
   si4703_read_reg.chipid         = (data_rd[14]<<8)|data_rd[15];
   si4703_read_reg.powercfg       = (data_rd[16]<<8)|data_rd[17];
   si4703_read_reg.channel        = (data_rd[18]<<8)|data_rd[19];
   si4703_read_reg.sysconfig1     = (data_rd[20]<<8)|data_rd[21];
   si4703_read_reg.sysconfig2     = (data_rd[22]<<8)|data_rd[23];
   si4703_read_reg.sysconfig3     = (data_rd[24]<<8)|data_rd[25];
   si4703_read_reg.test1          = (data_rd[26]<<8)|data_rd[27];
   si4703_read_reg.test2          = (data_rd[28]<<8)|data_rd[29];
   si4703_read_reg.bootconfig     = (data_rd[30]<<8)|data_rd[31];

   return status;
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
unsigned char Power_Radio(unsigned char power_up)
{
   unsigned char status;
   status = Read_si4703();
   if(status)
   {
      Delay_Ms(200u);//delay for XTAL stabilization
      si4703_read_reg.test1 = 0x8100u;
      status = Write_si4703();
      if(status)
      {
         Delay_Ms(300u);//delay for XTAL stabilization
         if(1 == power_up)
         {
            si4703_read_reg.powercfg = 0x4001u;
            status = Write_si4703();
            if(status)
            {
               si4703_read_reg.sysconfig1 = 0x1080u; 
               status = Write_si4703();
               if(status)
               {
                  si4703_read_reg.sysconfig2 &= ~(3<<4); //clear spacing bits
                  si4703_read_reg.sysconfig2 |= (1<<4);  //100kHz channel spacing
                  si4703_read_reg.sysconfig2 &= 0xFFF0u ; 
                  si4703_read_reg.sysconfig2 |= 0x0010u ; //make very volume MUTE
                  status = Write_si4703();
               }                          
            }
         }
         else if(0 == power_up)
         {
            si4703_read_reg.powercfg = 0x4041u;
            status = Write_si4703();
         }
      }
   }
   Fault(FLT_RADIO_INIT,status);
   return status;
}
/******************************************************************************
* Function Name     : 4 bits VOLEXT
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Set_Vol(unsigned char vol_level)
{
   volatile unsigned char status;
   si4703_read_reg.sysconfig2 &= 0xFFF0u ; //make mute
   si4703_read_reg.sysconfig2 |= (unsigned short int)(vol_level) ; //make very low volume
   status = Write_si4703();
   Fault(FLT_RADIO_VOL,status);
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Set_Fm_Tune_Freq(unsigned short int freq)
{
   volatile unsigned char status;
   unsigned char scale;
   unsigned short int cnt;
   if((freq > K_Fm_Min_Freq) & (freq <= K_Fm_Max_Freq))
   {
      scale = (freq - K_Fm_Min_Freq)/10u;
   }
   else
   {
      scale = 0u;
   }
   status = Read_si4703();      
   si4703_read_reg.channel &= 0xFE00u; //Clear out the channel bits
   si4703_read_reg.channel |= scale; //Mask in the new channel
   si4703_read_reg.channel |= 0x8000u; //Set the TUNE bit to start
   status = Write_si4703();    
   cnt  = SET_FM_TUNE_RSSI_THRESHOLD;
   while(cnt)
   {
     cnt -- ;
     status = Read_si4703();      
     if((si4703_read_reg.statusrssi & (1<<14)) != 0u)//Tune complete
     {
        break;
     }
   }
   status = Read_si4703();
   si4703_read_reg.channel  &= ~(1<<15); 
   status = Write_si4703();      
   cnt  = SET_FM_TUNE_RSSI_THRESHOLD;
   while(cnt)
   {
     cnt--;
     status = Read_si4703();    
     Delay_Ms(10u)  ;  
     if((si4703_read_reg.statusrssi & (1<<14)) == 0u)
     {
        break;
     }
   }
   Fault(FLT_RADIO_SET_FM,status);

}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
unsigned short int Get_Fm_Tune_Status(void)
{
   return(si4703_read_reg.statusrssi);
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Radio_Enc1_Operation(unsigned char dir)
{
   if((ON_STATE == power_state ))
   {
      if(dir == RIGHT_TURN)/*right turn*/
      { 
         Led(K_blink_RotChange);
         if(preset_mode)
         {
            if(preset_index < (MAX_CH-1U))
            {
               preset_index++;
               if(K_Radio_Data_Read[preset_index] <= K_Fm_Max_Freq)
               {
                  fm_freq = K_Radio_Data_Read[preset_index];
                  Is_Manual_Tune = 1;
                  Log_Printf(SEND_CH);
                  Log_Printf(SEND_FREQ);
               }
            }
         }
         else
         {
            if(fm_freq < K_Fm_Max_Freq)
            {
               fm_freq = fm_freq + 10;
               Is_Manual_Tune = 1;
               Log_Printf(SEND_FREQ);
            }
         }
         
      }
      else if(dir == LEFT_TURN)/*left*/
      {
         Led(K_blink_RotChange);
         if(preset_mode)
         {
            if(preset_index > 0u)
            {
               preset_index--;
               if(K_Radio_Data_Read[preset_index] >= K_Fm_Min_Freq)
               {
                  fm_freq = K_Radio_Data_Read[preset_index];
                  Is_Manual_Tune = 1;
                  Log_Printf(SEND_CH);
                  Log_Printf(SEND_FREQ);
               }
            }
         }
         else
         {
            if(fm_freq > K_Fm_Min_Freq)
            {
               fm_freq = fm_freq - 10;
               Is_Manual_Tune = 1;
               Log_Printf(SEND_FREQ);
            }
         }
      }
      else
      {
         /*do nothing*/
      }
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Radio_Enc2_Operation(unsigned char dir)
{
   if((ON_STATE == power_state ))
   {
      if(dir == RIGHT_TURN)/*right turn*/
      { 
         Led(K_blink_RotChange);
         if(si_vol < K_Max_Si_Vol_Level)
         {
           si_vol++;
           Log_Printf(SEND_VOL);
         }             
         Is_Vol_Changed = 1;
         reset_Vol_Timer = RESET_VOL_TIMER_TH;
      }
      else if(dir == LEFT_TURN)/*left*/
      {
         Led(K_blink_RotChange);
         if(si_vol > K_Min_Si_Vol_Level)
         {
           si_vol--;
           Log_Printf(SEND_VOL);
         }             
         Is_Vol_Changed = 1;
         reset_Vol_Timer = RESET_VOL_TIMER_TH;
      }
      else
      {
         /*do nothing*/
      }
   }
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Radio_Enc1_Button(unsigned char duration)
{
   if(power_state == ON_STATE)
   {
      if(duration == SHORT_PRESS)
      {
         preset_mode = !preset_mode;
         if(preset_mode)
         {
            Log_Printf(SEND_PRESET);
            Log_Printf(SEND_CH);
            fm_freq = K_Radio_Data_Read[preset_index];
         }
         else
         {
            Log_Printf(SEND_MANUAL);
         }
         Log_Printf(SEND_FREQ);
         Is_Manual_Tune = 1;
         Led(K_blink_ModeChange);
         
      }
      else if(duration == LONG_PRESS)
      {
         K_Radio_Data_Write[preset_index] = fm_freq;
         K_Radio_Data_Read[preset_index] = K_Radio_Data_Write[preset_index];
         ee_state = UPDATE_EE;
         preset_mode = 1u;
         Log_Printf(SEND_SAVED);
         Log_Printf(SEND_CH);
         Log_Printf(SEND_FREQ);
         Led(K_blink_Store);
      }
      else
      {
         /* do nothing */      
      }
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Radio_Enc2_Button(unsigned char duration)
{
   if(duration == SHORT_PRESS)
   {
      preset_mode = 0;
      if(power_state == ON_STATE)
      {
         power_state = OFF_STATE;
      }
      else if(power_state == OFF_STATE)
      {
         power_state = ON_STATE;
      }
      else
      {
      }
   }
   else
   {
   
   }
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Set_GPIO1(unsigned char state)
{
   volatile unsigned char status = 0;
   si4703_read_reg.sysconfig1 &= 0xFFFCu;
   si4703_read_reg.sysconfig1 |= Set_GPIO1_Val(!state);
   status = Write_si4703();
   Fault(FLT_RADIO_GPIO1,status);
   
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Set_GPIO2(unsigned char state)
{
   volatile unsigned char status = 0;
   si4703_read_reg.sysconfig1 &= 0xFFF3u;
   si4703_read_reg.sysconfig1 |= Set_GPIO2_Val(!state);
   status = Write_si4703();
   Fault(FLT_RADIO_GPIO2,status);
   
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
static void Validate_FM_Data(void)
{
   unsigned char i = 0;
   for(i=0;i<MAX_CH;i++)
   {
      if((K_Radio_Data_Read[i] > K_Fm_Max_Freq) || (K_Radio_Data_Read[i] < K_Fm_Min_Freq))
      {
         K_Radio_Data_Read[i] = K_Fm_Min_Freq;
         K_Radio_Data_Write[i] = K_Fm_Min_Freq;
      }
      else
      {
         K_Radio_Data_Write[i] = K_Radio_Data_Read[i];
      }
   }    
   if(K_Radio_Data_Read[MAX_CH] > (MAX_CH-1u))
   {
      K_Radio_Data_Read[MAX_CH] = 0u;
      K_Radio_Data_Write[MAX_CH] = 0u;
      preset_index = 0u;
   }
   else
   {
      preset_index = (unsigned char)K_Radio_Data_Read[MAX_CH];
      K_Radio_Data_Write[MAX_CH] = (unsigned short int)preset_index;
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Process_App_Si(void)
{
   volatile unsigned char status = 0;
   switch(radio_state)
   {
      case RADIO_RUNNING:
           if(power_state == OFF_STATE)
           {
              radio_state = RADIO_OFF;
              Set_Vol(K_Min_Si_Vol_Level);
              Set_GPIO2(FALSE);//Blue Led Power OFF
              Log_Printf(SEND_PWR_OFF);
              monitor_mode = 1;
              Power_Radio(0);
           }
           else if(power_state == ON_STATE)
           {
              if(reset_Vol_Timer > 0u)
              {
                 reset_Vol_Timer--;
              }
              if(Is_Vol_Changed)
              {
                 disp_timer = DISP_RELOAD;
                 if(si_vol <= K_Max_Si_Vol_Level)
                 {
                    Set_Vol(si_vol);
                 }
                 Is_Vol_Changed = 0u;
              }
              
              if(Is_Manual_Tune)
              {
                 disp_timer = DISP_RELOAD;
                 Set_Fm_Tune_Freq(fm_freq);
                 Is_Manual_Tune = 0;
                 fm_stereo = 0;
              }
              else
              {
                 if(monitor_mode)
                 {
                    Scan_Cnt++;
                    if(Scan_Cnt == 30u)
                    {
                       Scan_Cnt = 0u;
                       status = Read_si4703();
                       if(Get_Fm_Tune_Status() & 0x0100U)
                       {
                         fm_stereo = 1u;
                       }
                       else
                       {
                         fm_stereo = 0u;
                       }
                       if((si4703_read_reg.powercfg & 0x0001u) == 0u)
                       {
                          radio_state = RADIO_RESET;
                       } 
                       if(prev_fm_stereo != fm_stereo)
                       {
                          prev_fm_stereo = fm_stereo;
                       }
                    }
                 }
              }
              if(disp_timer != 0u)
              {
                 disp_timer--;
              }

              if(prev_fm_freq != fm_freq)
              {
                 prev_fm_freq = fm_freq;
              }
           }
           break;   
      case RADIO_INIT:
           if(power_state == ON_STATE)
           {
              if(App_Si_EE_Check_Pending)
              {
                 App_Si_EE_Check_Pending = 0u;
                 Validate_FM_Data();
              }
              si_vol = K_Default_Si_Vol_Level;
              if(!first_time_power_up)
              {
                 first_time_power_up = 1u;
                 fm_freq = K_Radio_Data_Read[preset_index];
                 Log_Printf(SEND_WELCOME);
              }
              Set_Fm_Tune_Freq(fm_freq);
              preset_mode = 0u;               
              radio_state = RADIO_RUNNING;
              Is_Vol_Changed = 1u;
              monitor_mode = 1u;
              Set_GPIO2(TRUE);//Blue Led Power ON
              Amp_Power(OFF);
              Amp_Mute(OFF); 
              Log_Printf(SEND_PWR_ON);
              Log_Printf(SEND_FREQ);
              disp_timer = DISP_RELOAD;
           }
           break;   
      case RADIO_RESET:
           Init_Radio();
           radio_state = RADIO_INIT;
           break;
      case RADIO_OFF:
           if(power_state == ON_STATE)
           {
              radio_state = RADIO_INIT;
              Log_Printf(SEND_PWR_ON);
              disp_timer = DISP_RELOAD;
              monitor_mode = 0u;
              Power_Radio(1);
           }
           else
           {
              Amp_Mute(ON); 
              Amp_Power(OFF);
              //si in reset state
              si4703_Rsen(0u);
              si4703_Rst(0u);
              ShutI2C();
              ShutGPIO();
              /*  sleep here
              ZZZZZZzzzzzzzz,  ---
              ZZZZZzzzzz.... ( . . )
              ZZZZZzzzzz.... .  O
              ZZZZzzzz.......  ---
              ZZZzzz
              ZZzz
              Zz
              */
              Enter_Sleep();

              /* Wakeup restart from here*/
              unsigned int delay_stabilize = 1000u;
              // Configure Clock: Set DCO to 16 MHz
              DCOCTL = CALDCO_16MHZ;     // DCO to 16 MHz
              BCSCTL1 = CALBC1_16MHZ;    // Set BCS to 16 MHz
              while(delay_stabilize--)
              {
                  __no_operation();
              }
              ee_state = INIT_EE;
              App_Si_EE_Check_Pending = 1u;
              first_time_power_up = 0u;
              Service_Watchdog();
              Init_Radio();
           
           }
           break;   
    default:
           break;   
   }
}
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Process_Commands(void)
{
   unsigned char val=0;
   if(UARTFlag == 1)
   {
      UARTFlag = 0;
      if((UARTData[0] == 0x55u) && (UARTData[2] == 0xAAu))
      {
         val = UARTData[1];
      }
      Execute_Commands(val);
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Execute_Commands(unsigned char val)
{
   switch(val)
   {
      case '^'://mute
              if(power_state == ON_STATE)
              {
                 Set_Vol(K_Min_Si_Vol_Level);
                 Log_Printf(SEND_MUTE);
              }
              break;
      case ']'://power OFF
              if(power_state == ON_STATE)
              {
                 power_state = OFF_STATE;
              }
              break;
      case '[':	//power ON
              if(power_state == OFF_STATE)
              {
                 power_state = ON_STATE;
              }
              break;
      case '<'://volume down
              if(power_state == ON_STATE)
              {
                 Radio_Enc2_Operation(LEFT_TURN);
              }
              break;
      case '>'://volume up
              if(power_state == ON_STATE)
              {
                 Radio_Enc2_Operation(RIGHT_TURN);
              }
              break;
      case '+':	//right rotate in radio mode
              if(power_state == ON_STATE)
              {
                 Radio_Enc1_Operation(RIGHT_TURN);
              }
              break;
      case '-'://left rotate in radio mode
              if(power_state == ON_STATE)
              {
                 Radio_Enc1_Operation(LEFT_TURN);
              }
              break;
      case 'S'://store preset
              if(power_state == ON_STATE)
              {
                 Radio_Enc1_Button(LONG_PRESS);
              }
              break;
      case 'M'://mode change
              if(power_state == ON_STATE)
              {
                 Radio_Enc1_Button(SHORT_PRESS);
              }
      case 'R'://monitor change
              if(power_state == ON_STATE)
              {
                 monitor_mode = !monitor_mode;
              }
              break;
       default :
              break;
   }    
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
unsigned char Write_Si_I2C(unsigned char *dataPtr,unsigned char size)
{
   volatile unsigned char status=1u;
   volatile unsigned int counterSize=0u;
   volatile unsigned char *memoryData;

   memoryData = dataPtr;
   
   while(UCB0STAT & UCBUSY)                // wait until I2C module has
   {
      if(exitLoop(7))
      {
         status = 0;
         return status;
      }
   }
   I2CWriteInit();
   UCB0CTL1 |= UCTXSTT;                      // start condition generation
   while(UCB0CTL1 & UCTXSTT)
   {
      if(exitLoop(8))
      {
         status = 0;
         return status;
      }
      if(!(UCNACKIFG & UCB0STAT))            // Break out if ACK received
      {
         break;
      }         
   }
   for(counterSize=0;counterSize<size;counterSize++)   
   {
      UCB0TXBUF = *memoryData;       // Load TX buffer
      while(!(UCB0TXIFG & IFG2))
      {
         if(exitLoop(9))
         {
            status = 0;
            break;
         }
      }
      *memoryData++;     
      if(UCB0STAT & UCNACKIFG)       // nack received this should not happen if address is correct
      {
         UCB0STAT &= ~UCNACKIFG;
         UCB0CTL1 |= UCTXSTP;        //stop...
         while(UCB0CTL1 & UCTXSTP)
         {
            if(exitLoop(10))
            {
               status = 0;
               break;
            }
         }
         while(UCB0STAT & UCBUSY)  // wait until I2C module is idle
         {
            if(exitLoop(11))
            {
               status = 0;
               break;
            }
         }
         return status;              //... and exit
      }
   }
   UCB0CTL1 |= UCTXSTP;              //stop...
   while(UCB0CTL1 & UCTXSTP)
   {
      if(exitLoop(12))
      {
         status = 0;
         break;
      }
   }
   while(UCB0STAT & UCBUSY)        // wait until I2C module is idle
   {
      if(exitLoop(13))
      {
         status = 0;
         break;
      }
   }
   return status;
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
unsigned char Read_Si_I2C(unsigned char *dataPtr,unsigned char size)
{
   volatile unsigned char status=1u;
   volatile unsigned int counterSize=0u;
   volatile unsigned char *memoryData;

   memoryData = dataPtr;

   while(UCB0STAT & UCBUSY)                // wait until I2C module is idle
   {
      if(exitLoop(1))
      {
         status = 0;
         break;
      }
   }
   // Read Data byte
   I2CReadInit();
   UCB0CTL1 |= UCTXSTT;                     // I2C start condition
   while(UCB0CTL1 & UCTXSTT)                // Start condition sent?
   {
      if(exitLoop(2))
      {
         status = 0;
         break;
      }
      if(!(UCNACKIFG & UCB0STAT))           // Break out if ACK received
      {
         break;
      }
   }
   if(UCB0STAT & UCNACKIFG)                 // nack received this should not happen if address is correct
   {
      UCB0STAT &= ~UCNACKIFG;
      UCB0CTL1 |= UCTXSTP;                  //stop...
      while(UCB0CTL1 & UCTXSTP)
      {
         if(exitLoop(3))
         {
            status = 0;
            break;
         }
      }
      while(UCB0STAT & UCBUSY)                // wait until I2C module is idle
      {
         if(exitLoop(4))
         {
            status = 0;
            break;
         }
      }
   }

    while(1)
    {
      if(UCB0RXIFG & IFG2)
      {
         if(counterSize >= size)
         {
            IFG2 &= ~UCB0RXIFG;
            //one more  byte to read send stop
            UCB0CTL1 |= UCTXSTP; //I2C stop condition
            while(UCB0CTL1 & UCTXSTP)
            {
               if(exitLoop(5))
               {
                  status = 0;
                  break;
               }
            }
            while(UCB0STAT & UCBUSY)                // wait until I2C module is idle
            {
               if(exitLoop(6))
               {
                  status = 0;
                  break;
               }
            }
            break;            
         }
         else 
         {
            *memoryData= UCB0RXBUF;
            *memoryData++;
            counterSize++;
         }         
      }
      if(exitLoop(6))
      {
         status = 0;
         break;
      }         
   }
   return status;                              //... and exit
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Amp_Mute(unsigned char state) 
{
   // Set the corresponding GPIO pin to HIGH (on)
   if(state)
   {
      Set_GPIO1(TRUE);
   }
   else
   {
      Set_GPIO1(FALSE);
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Amp_Power(unsigned char state) 
{
   // Set the corresponding GPIO pin to HIGH (on)
   if(state)
   {
      Amp_Enable();
   }
   else
   {
      Amp_Disable();
   }
}



