/*filename :- main.c*/                                                            

#include "p30f3010.h"
#include "ports.h"
#include "timer.h"
#include <libpic30.h>
#include <string.h>
#include <stdio.h>
#include "system.h"
#include "rotary.h"
#include "btn.h"
#include "bm.h"
#include "led.h"


#define CONSTANT1 10u
#define CONSTANT2 20u

#define SYS_FREQ        32000000u      /*XT+XT_PLL8 for 4Mhz XTAL*/
#define FCY             SYS_FREQ/4u   /*8Mhz*/

_FOSC(CSW_FSCM_OFF & XT_PLL8);   //XTAL mode with x 8 PLL
_FWDT(WDT_OFF);                 //Turn off the Watch-Dog Timer.
_FBORPOR(MCLR_EN & PWRT_OFF);   //Enable MCLR reset pin and turn off the power-up timers.
_FGS(CODE_PROT_OFF); 




volatile unsigned char Task_Activate;
volatile Task_T Task_Now;
volatile unsigned char Power_Mode;



/*Declare constants/coefficients/calibration data to be stored in DataEEPROM*/
volatile unsigned int _EEDATA(32) Data_EE[16]={0x0000,0x1b6a,0x0000};
volatile unsigned int Data_RAM[16];
_prog_addressT EE_addr;


static void Init_UART(void);
static void Process_Audio(void);
static void Read_EEPROM(void);
static void Write_EEPROM(void);
static void Init_Amp(void);
static void Init_UART(void);
static void OS(void);
static void Init_HWIO(void);
static void Turn_Relay(unsigned char rly1 , unsigned char rly2);




/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
int main(void)
{
   //power and crystal stabilization
   unsigned int stabilize = 9000u;
   while(stabilize--);
    
   Init_HWIO();
   
   
   /* initialize a variable to represent the Data EEPROM address */
   _init_prog_address(EE_addr, Data_EE);
   Read_EEPROM();
   Init_Amp();   
   Led_Init();

   while(1)
   {
     OS();
   };   
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Init_Amp(void)
{
   Power_Mode = PWR_OFF;
   Turn_Relay(OFF,OFF);
   Bm83_Init();
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Shut_PowerAmp(void)
{
   Turn_Relay(ON,OFF);
   Delay_Ms(800);
   Turn_Relay(OFF,OFF);
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Start_PowerAmp(void)
{
   Turn_Relay(ON,OFF);
   Delay_Ms(800);
   Turn_Relay(ON,ON);
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Init_UART(void)
{
   U1BRG  = 25u; //for Fcy = 8Mhz and 19.2K (refer dspic30 FRM UART section)
   U1MODE = 0x8400;  /* operation settings */
   U1STA = 0x0040;   /* TX & RX interrupt modes */
   U1STAbits.UTXEN =1;
   IFS0bits.U1RXIF = 0;
   IEC0bits.U1RXIE = 1;

}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Init_HWIO(void)
{
   ADCON1= 0x00;
   ADPCFG = 0xFF;
   
   //Port_Init
   CBI(RE0);
   LATE0 = 0;
   TRISE0 = 0;
   
   CBI(RE1);
   LATE1 = 0;
   TRISE1 = 0;

   CBI(RE2);
   LATE2 = 0;
   TRISE2 = 0;

   CBI(RE3);
   LATE3 = 0;
   TRISE3 = 0;

   CBI(RE4);
   LATE4 = 0;
   TRISE4 = 0;

   CBI(RE5);
   LATE5 = 0;
   TRISE5 = 0;

   CBI(RE8);
   LATE8 = 0;//
   TRISE8 = 0;//

   
   CBI(RD0);
   LATD0 = 0;
   TRISD0 = 0;//
  
   CBI(RD1);
   LATD1 = 0;
   TRISD1 = 0;//
   
   CBI(RC13);
   TRISC13 = 0;//U1ATX

   CBI(RC14);
   TRISC14 = 1;//U1ARX

   
   CBI(RB4);
   LATBbits.LATB4 = 0;
   TRISB4 = 1;//
   
   CBI(RB5);
   LATBbits.LATB5 = 0;
   TRISB5 = 1;//

   CBI(RB3);
   LATBbits.LATB3 = 0;
   TRISB3 = 0;//

   CBI(RB2);
   LATBbits.LATB2 = 0;
   TRISB2 = 0;//

   CBI(RB0);
   LATBbits.LATB0 = 0;
   TRISB0 = 1;//

   CBI(RB1);
   LATBbits.LATB1 = 0;
   TRISB1 = 1;//
 
   CBI(RF2);
   LATFbits.LATF2 = 0;
   TRISF2 = 0;//sda

   CBI(RF3);
   LATFbits.LATF3 = 0;
   TRISF3 = 0;//scl

   //Timer1 (A) for Os init 
   ConfigIntTimer1(T1_INT_PRIOR_7 & T1_INT_ON );
   OpenTimer1(0x8000,0x1F40);//TMR Enabled , for 1ms interrupt i.e SYS_FREQ/1000

   //UART for Rx/Tx
   Init_UART();


   Init_Encoder();
   Init_Btn();
   
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void OS(void)
{
   unsigned char test_data[1] = {0xAA};
   if(Task_Activate == ON)
   {
      Task_Activate = OFF;
      Bm83_Task();
      if(Task_Now == TASK_10MS)
      {
         App_Btn_Process();
         Led_Task();
      }
      
      if(Task_Now == TASK_50MS)
      {
         Process_Audio();
      }
      
      if(Task_Now == TASK_100MS)
      {
         Slow_Periodic_Encoder();
      }
      
      if (Task_Now == TASK_1S)
      {
         if(PWR_ON == Power_Mode)
         {
         }
      }
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Read_EEPROM(void)
{
   /*DataEEPROM to RAM*/
   _memcpy_p2d16(Data_RAM, EE_addr, _EE_ROW);
   if(Data_RAM[0]!=0xffff)
   {
   }
   else
   {
   }
   
   if(Data_RAM[1]!=0xffff)
   {
   }
   else
   {
   }
   
   if(Data_RAM[2]!=0xffff)
   {
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
static void Process_Audio(void)
{
   static unsigned char prev_key_pressed;
   
   if(key_pressed[PWR_BTN] != prev_key_pressed)
   {
      prev_key_pressed = key_pressed[PWR_BTN];
      if(PWR_OFF == Power_Mode)//now turn on the system
      {
         if((key_pressed[PWR_BTN]) && (key_pressed_type[PWR_BTN] == LONG_PRESS_TYPE))
         {
            if(Power_Mode == PWR_OFF)
            {
               Power_Mode = PWR_ON;
               Led(blink_PwnOn);
               Bm83_Power_On_Device(&Bm83);
               Delay_Ms(2000);
               Bm83_Enter_Pairing_Mode(&Bm83);
               Delay_Ms(300);
               Bm83_Clear_Events(&Bm83);
               Start_PowerAmp();
            }
         }
      }
      else//turn off the system
      { 
         if((!key_pressed[PWR_BTN]) && (key_pressed_type[PWR_BTN] == LONG_PRESS_TYPE))
         {
            if(Power_Mode == PWR_ON)
            {
               Power_Mode = PWR_OFF;
               Shut_PowerAmp();
               Bm83_Power_Off_Device(&Bm83);
               Delay_Ms(200);
               Led(blink_PwnOff);
            }
         }
      } 
   }

   if((key_pressed[INFO_BTN] ) && (Power_Mode == PWR_ON))
   {
      key_pressed[INFO_BTN] = FALSE;
   }

}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Write_EEPROM(void)
{
   _INT0IE = 0;
    /*Erase a row in Data EEPROM at array "fooArrayinDataEE" */
   _erase_eedata(EE_addr, _EE_ROW);
   _wait_eedata();
   Data_RAM[0] = 0x11;
   Data_RAM[1] = 0x22;
   Data_RAM[2] = 0x33;
   /*Write a row to Data EEPROM from RAM" */
   _write_eedata_row(EE_addr, Data_RAM);
   _wait_eedata();
   _INT0IE = 1;
   



}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void UARTSend(unsigned char *data,unsigned char len)
{
   _INT0IE = 0;
   while(len)
   {
      while(U1STAbits.UTXBF); /* wait if the buffer is full */
      U1TXREG = *data;    /* transfer data word to TX reg */
      *data++;
      len--;
   }
   _INT0IE = 1;
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Turn_Relay(unsigned char rly1 , unsigned char rly2)
{
   RD0 = (rly1) ? 1 : 0;
   RD1 = (rly2) ? 1 : 0;
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Delay_Ms(unsigned long timer_val)
{
   unsigned long t_cnt = 0;
   _T2IF = 0;
   _T2IE = 0;
   TMR2 = 0;
   OpenTimer2(0x8000,0x1F40);
   do
   {
     t_cnt++;
     while(!_T2IF);
     _T2IF = 0;
   }while(t_cnt < timer_val);
   CloseTimer2();
}
