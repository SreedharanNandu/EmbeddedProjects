#include "pic12f1822.h"
#include "types.h"
#include "hal_lin.h"
#include "btn.h"
#include "led.h"

//#define TIMER2_USED_OS             

/**********************************************************************************
Global definition
***********************************************************************************/

/**********************************************************************************
Global variables
***********************************************************************************/
volatile unsigned char timer_10ms_count;
volatile unsigned char timer_1ms_count;

/***********************************************************************************
Function Prototypes
***********************************************************************************/
static void Init_HWIO(void);
#if defined(TIMER0_USED)
static void Delay_Ms(unsigned long dlyMs);
#endif

/***********************************************************************************
Function Prototypes
***********************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
***********************************************************************************/
void main(void)
{
   //power stabilization
   volatile  unsigned int data = 9000u;
   while(data--);
   
   Init_HWIO();
   LIN_Initialize();
   
   //enable interrupts
   GIE = 1u;

   while(1)
   {
      if(timer_1ms_count == 1u)
      {
         timer_1ms_count = 0u;
         App_Led_Process();
      }
      if(timer_10ms_count == 9u)
      {
         App_Btn_Process();
         timer_10ms_count = 0u;
      }
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
static void Init_HWIO(void) 
{
   //write main config 
   APFCON = 0x8Cu;
   OSCCON = 0x78u;   //clock uses 16Mhz HFINTOSC
   OPTION_REG = 0x03u; //1:16 for TMR0 i.e 250Khz
   CM1CON0 = 7u;    //comparators off
   ANSELA = 0x04u;  //RA2 analog,rest all digital GPIO pins
   ADCON0 = 0x09u;  //RA2 selected, ADC enabled
   ADCON1 = 0xD0u;  //Right justified,fosc/16 = 1Mhz
   WPUA = 0x3Bu;//for RA2 analog pin internal pull up must be disabled,rest enabled

   //set port
   //VDD  pin1
   //1 - input , 0 - output
   TRISA5 = 1u;//gpio pin2 rx
   TRISA4 = 0u;//gpio pin3 tx
   TRISA3 = 1u;//mclr pin4
   TRISA2 = 1u;//analog gpio pin5 sw
   TRISA1 = 0u;//gpio pin6 not used
   TRISA0 = 0u;//gpio pin7 led
   //VSS  pin8

   SPBRGH = 0u; // For 9600 Baud and with 16 Mhz Crystal
   SPBRGL = 25u;
   
   TXSTAbits.TX9= 0; // 8-bit data mode
   TXSTAbits.TXEN = 1; // enable transmitter
   TXSTAbits.SYNC = 0; // enable asynchronous mode
   TXSTAbits.SENDB = 0; // sync break transmission completed
   TXSTAbits.BRGH = 0; // select low baud rate
   
   BAUDCONbits.SCKP = 0; // don't invert polarity
   BAUDCONbits.BRG16 = 0; // 8-bit baudrate generator
   BAUDCONbits.ABDEN = 0; // auto baud rate detect disable
   
   RCSTAbits.RX9 = 0; // enable 8-bit reception
   RCSTAbits.CREN = 1; // enable receiver
   RCSTAbits.FERR = 0;
   RCSTAbits.OERR = 0;
   RCSTAbits.SPEN = 1; // enable serial port 

   RCIE = 1;//enable interrupt

   #if defined(TIMER2_USED_OS)
   //timer2
   T2CON = 0x7Cu; //(fosc/4 input) 16Mhz/4 = 4Mhz/ 16(pre) = 0.25Mhz i.e 4us so that 256 * 4 gives 1024us which is 1ms
   TMR2 = 0u;//1ms timer 
   TMR2IF = 0u;
   TMR2IE = 1u;
   #else
   TMR0 = 0u;//1ms timer i.e 8 bit TMR with fosc/4  = 16mhz/4 = 4mhz with 1:16 prescalar = 250Khz clock,so MAX = 255 * 4us = 1020us
   TMR0IF = 0u;
   TMR0IE = 1u;
   #endif

   PEIE = 1u;//enable peripheral interrupt

}


#if defined(TIMER0_USED)
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
static void Delay_Ms(unsigned long dlyMs)
{
   //1msec X a
   while(dlyMs)
   {
      TMR0 = 0u;//1ms timer i.e 8 bit TMR with fosc/4  = 16mhz/4 = 4mhz with 1:16 prescalar = 250Khz clock,so MAX = 255 * 4us = 1020us
      TMR0IF = 0u;
      TMR0IE = 0u;
      while(!TMR0IF);
      dlyMs--;
   }
}
#endif






/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
static void interrupt isr(void)// Here be interrupt function - the name is unimportant.
{
   volatile unsigned char data;
   if(RCIF)
   {
      RCIF = 0;
      data = RCREG;
      LIN_Task_USART_Interrupt(data);
   }
   if(RCSTAbits.OERR)
   {
      RCSTAbits.CREN = 0;
      RCSTAbits.CREN = 1;
      Lin_State = SYNC_BREAK_WAIT;
   }

   #if defined(TIMER2_USED_OS)
   //timer 2 interrupt
   if(TMR2IE &&  TMR2IF)
   {
      TMR2IF = 0u;
      timer_1ms_count++;
      timer_10ms_count++;
   }
   #else
   //timer 0 interrupt
   if(TMR0IE &&  TMR0IF)
   {
      TMR0IF = 0u;
      timer_1ms_count++;
      timer_10ms_count++;
   }

   #endif
  
}




