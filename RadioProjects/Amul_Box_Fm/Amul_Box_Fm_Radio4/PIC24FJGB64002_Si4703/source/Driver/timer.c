/*****************************************************************************
* Filename   : timer.c
* Created By : Nandu
* Date       :    
*****************************************************************************/ 
#include "hardware.h"
#include "timer.h"

/*****************************************************************************
* Function    : void Init_Timer1(void)
* Created By  : Nandu
* Date        :    
* Overview    : 
* Argument    : None 
* Output      : None
* Note        : 
*****************************************************************************/ 
void Init_Timer1(void)
{
   T1CON = 0x0000;         //timer config
   TMR1 = 0;               //Clr TMRq
   PR1 = TMR1_VALUE;       //Load Period register
   
   #ifdef TIMER_INTERRUPT
      _T1IP = 0x01;    //Intr priority level
      _T1IF = 0;       //Clear the Timer1 interrupt status flag
      _T1IE = 1;       //Enable Timer1 interrupts
   #endif 

}
/*****************************************************************************
* Function   : Init_Timer2(void)
* Created By : Nandu
* Date       :    
* Overview   : 
* Argument   : None 
* Output     : None
* Note       : 
*****************************************************************************/ 
void Init_Timer2(void)
{
   T2CON = 0x0000; // Clr timer vaue
   PR2 = TMR1_VALUE; //same as TMR1 1ms
   TMR2 = 0;       // Clr TMRq

}
/*****************************************************************************
* Function   : Turn_On_Timer1(void)
* Created By : Nandu
* Date       :    
* Overview   : 
* Argument   : None 
* Output     : None
* Note       : 
*****************************************************************************/ 
void Turn_On_Timer2(void)

{
   TMR2 = 0;           //Clr TMRq   
   T2CONbits.TON = 1;  //Start Timer2 with prescaler settings at 1:1 and
}
/*****************************************************************************
* Function   : void Turn_Off_Timer1(void)
* Created By : Nandu
* Date       :    
* Overview   : 
* Argument   : None 
* Output     : None
* Note       : 
*****************************************************************************/ 
void Turn_Off_Timer2(void)
{
   T2CONbits.TON = 0;       //Turn off Timer 
   _T2IF = 0;       //Clear the Timer1 interrupt status flag
   _T2IE = 0;       //Turn OFF Timer 1
}
/*****************************************************************************
* Function   : Turn_On_Timer1(void)
* Created By : Nandu
* Date       :    
* Overview   : 
* Argument   : None 
* Output     : None
* Note       : 
*****************************************************************************/ 
void Turn_On_Timer1(void)

{
   TMR1 = TMR1_VALUE;          //Clr TMRq   
   _TON = 1;       //Start Timer1 with prescaler settings at 1:1 and
}
/*****************************************************************************
* Function   : void Turn_Off_Timer1(void)
* Created By : Nandu
* Date       :    
* Overview   : 
* Argument   : None 
* Output     : None
* Note       : 
*****************************************************************************/ 
void Turn_Off_Timer1(void)
{
   _TON = 0;       //Turn off Timer 
   _T1IF = 0;       //Clear the Timer1 interrupt status flag
   _T1IE = 0;       //Turn OFF Timer 1
}


/*****************************************************************************
* Function   : void Delay_Ms(unsigned long)
* Created By : Nandu
* Date       :    
* Overview   : 
* Argument   : None 
* Output     : None
* Note       : 
*****************************************************************************/ 
void Delay_Ms(unsigned long timer_val)
{
   unsigned long t_cnt = 0;
   _T2IF = 0;
   _T2IE = 0;
   TMR2 = 0;
   Turn_On_Timer2();
   do
   {
     t_cnt++;
     while(!_T2IF);
     _T2IF = 0;
   }while(t_cnt < timer_val);
   Turn_Off_Timer2();
}

/*****************************************************************************
* Function   : void InsertDelay(unsigned int)
* Created By : Nandu
* Date       :    
* Overview   : 
* Argument   : None 
* Output     : None
* Note       : 
*****************************************************************************/ 
void InsertDelay(unsigned int x)
{                         
   volatile unsigned int idelay = x;   
   while(idelay--);
}

//EOF
