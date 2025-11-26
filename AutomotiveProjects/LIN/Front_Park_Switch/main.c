#include "pic12f1822.h"
#include "types.h"
#include "hal_lin.h"
#include "btn.h"

#define DEBUG_LED               0u
#define BAUD_9600               1u
#define DELAY_9600              13u
#define TIMER2_USED             

/**********************************************************************************
Global definition
***********************************************************************************/

/**********************************************************************************
Global variables
***********************************************************************************/
volatile unsigned char timer_10ms_count;
volatile unsigned char timer_100ms_count;

/***********************************************************************************
Function Prototypes
***********************************************************************************/


static void InitializeSystem(void);
static void Init_UART(void);


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
   
   InitializeSystem();
   Init_UART();
   Init_Btn();
   
   while(1)
   {
      if(timer_10ms_count == 9u)
      {
         App_Btn_Process();
         timer_10ms_count = 0u;
      }
      if(timer_100ms_count == 99u)
      {
         timer_100ms_count = 0u;
      }
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
static void InitializeSystem(void) 
{
   //write main config 
   APFCON = 0x8Cu;
   OSCCON = 0x78u;   //clock uses 16Mhz HFINTOSC
   OPTION_REG = 0x03u; //1:16 for TMR0 i.e 250Khz
   CM1CON0 = 7u;    //comparators off
   ANSELA = 0x04u;  //RA2 analog,rest all digital GPIO pins
   ADCON0 = 0x01u;  //ADC enabled
   ADCON1 = 0xD0u;  //Right justified,fosc/16 = 1Mhz
   
   //set port
   //VDD  pin1
   //1 - input , 0 - output
   TRISA5 = 1u;//gpio pin2 rx
   TRISA4 = 0u;//gpio pin3 tx
   TRISA3 = 1u;//mclr pin4
   TRISA2 = 1u;//analog gpio pin5 sw
   TRISA1 = 0u;//gpio pin6 led
   TRISA0 = 0u;//gpio pin7 slp_pin
   //VSS  pin8


   #if defined(TIMER2_USED)
   //timer2
   T2CON = 0x7Cu; //(fosc/4 input) 16Mhz/4 = 4Mhz/ 16(pre) = 0.25Mhz i.e 4us so that 256 * 4 gives 1024us which is 1ms
   TMR2 = 0u;//1ms timer 
   TMR2IF = 0u;
   TMR2IE = 1u;
   #endif
   
   //enable interrupts
   PEIE = 1u;
   GIE = 1u;

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
   unsigned char data;
   #if defined(TIMER2_USED)
   //timer 2 interrupt
   if(TMR2IE &&  TMR2IF)
   {
      TMR2IF = 0u;
      timer_10ms_count++;
      timer_100ms_count++;
   }
   #endif
  
   if(PIR1bits.RCIF)
   {
      RCIF = 0;
      data = RCREG;
      LIN_Task_USART_Interrupt(data);
   }
}


/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
static void Init_UART(void)
{
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
}


