#include <stdio.h>
#include "rotary.h"
#include "App_Btn.h"



static volatile unsigned char Edge_Timer1 ;
static volatile unsigned char Encoder_Count1;
static volatile unsigned char Current_Encoder_A1 ;
static volatile unsigned char Current_Encoder_B1 ;
static volatile unsigned char Prior_Encoder_A1 ;
static volatile unsigned char Prior_Encoder_B1 ;
static volatile unsigned char Encoder_Missed_Edge1 ;
static volatile unsigned char Edge_Faster1 ;
static signed char Encoder_State1;
static volatile unsigned char Debounced_Signal_A_Changed1 ;
static volatile unsigned char Debounced_Signal_B_Changed1 ;
static signed char Prior_Encoder_State1 ;
static volatile unsigned char Encoder_Debounce_Timer_A1 ;
static volatile unsigned char Encoder_Debounce_Timer_B1 ;


void Decode_Encoder1( void );
void Clear_Encoder_Counts1(void);
void Clear_Encoder_Direction1(void);

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Init_Encoder1(void) 
{  
   unsigned char Signal_A1 ;
   unsigned char Signal_B1 ;

   /*-----------------------------------------------------------------
                Initialize Radio Encoder Rotary signals
    -----------------------------------------------------------------*/
   Signal_A1 = EncA1_Pin();
   Signal_B1 = EncB1_Pin();
   Current_Encoder_A1 = Signal_A1 ;
   Prior_Encoder_A1 = Signal_A1 ;
   Current_Encoder_B1 = Signal_B1;
   Prior_Encoder_B1 = Signal_B1;
   Edge_Faster1 = 0 ;
   Edge_Timer1 = RADIO_ENC_EDGE_DELAY ;
   Prior_Encoder_State1 = RADIO_ENC_IDLE_ENCODER_STATE ;
   Encoder_State1 = RADIO_ENC_IDLE_ENCODER_STATE ;
   Encoder_Debounce_Timer_A1 = RADIO_ENC_ZERO_SAMPLES ;
   Encoder_Debounce_Timer_B1 = RADIO_ENC_ZERO_SAMPLES ;
   Encoder_Missed_Edge1 = 0 ;
   Clear_Encoder_Counts1();
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  : 0.5ms or 1ms  
*******************************************************************************/
void Fast_Periodic_Encoder1( void )
{
   unsigned char Signal_A1 ;
   unsigned char Signal_B1 ;
    
   Debounced_Signal_A_Changed1 = 0 ;
   Debounced_Signal_B_Changed1 = 0 ;
   /*-------------------------------------------------------------------------
                          Encoder Signal A Debouncing
   -------------------------------------------------------------------------*/
   Signal_A1 = EncA1_Pin();
   if ( Current_Encoder_A1 != Signal_A1 )
   {
       Current_Encoder_A1 = Signal_A1 ;
       Encoder_Debounce_Timer_A1 = RADIO_ENC_DEBOUNCE_PERIOD_NORMAL ;
   }
   else if ( Encoder_Debounce_Timer_A1 > 0u )
   {
       Encoder_Debounce_Timer_A1-- ;
       if ( Encoder_Debounce_Timer_A1 == 0u )
       {
           if ( Prior_Encoder_A1 != Current_Encoder_A1 )
           {
               Debounced_Signal_A_Changed1 = 1 ;
           }
       }
   }

   /*-------------------------------------------------------------------------
                     Encoder Signal B Debouncing
    ------------------------------------------------------------------------*/

   Signal_B1 = EncB1_Pin();
   if ( Current_Encoder_B1 != Signal_B1 )
   {
       Current_Encoder_B1 = Signal_B1 ;
       Encoder_Debounce_Timer_B1 = RADIO_ENC_DEBOUNCE_PERIOD_NORMAL ;
   }
   else if ( Encoder_Debounce_Timer_B1 > 0u )
   {
       Encoder_Debounce_Timer_B1-- ;
       if ( Encoder_Debounce_Timer_B1 == 0u )
       {
           if ( Prior_Encoder_B1 != Current_Encoder_B1 )
           {
               Debounced_Signal_B_Changed1 = 1 ;
           }
       }
   }

      /*-------------------------------------------------------------------------
                                   Encoder Decoding
       ------------------------------------------------------------------------*/
   /*if ICP is normal communication and wake up recvd*/

   if ( Debounced_Signal_A_Changed1 || Debounced_Signal_B_Changed1 )
   {
       /* Start or re-start edge timer at any edge*/
       Edge_Timer1 = RADIO_ENC_EDGE_DELAY ;
       Decode_Encoder1() ;
       /* Save the current signal states as the next prior signal states*/
       if ( Debounced_Signal_A_Changed1 )
       {
           Prior_Encoder_A1 = Current_Encoder_A1 ;
       }
       if ( Debounced_Signal_B_Changed1 )
       {
           Prior_Encoder_B1 = Current_Encoder_B1 ;
       }
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Decode_Encoder1( void )
{
   /* Perform binary-tree logic decoding of the signal changes*/
   if(Prior_Encoder_A1)
   {
      if(Current_Encoder_A1)
      {
         /*-----------------------------------------------------------------
           3 > 3       11 > 11     No change                      15   1111
           3 > 2       11 > 10     Counterclockwise one increment 14   1110
           2 > 3       10 > 11     Clockwise one increment        13   1101
           2 > 2       10 > 10     No change                      12   1100
          -----------------------------------------------------------------*/
         if(Prior_Encoder_B1)
         {
            if(Current_Encoder_B1)
            {
               /* 1111     15  No change*/
            }
            else
            {
               /* 1110     14  A remained high, B High > Low, Counterclockwise*/
               Encoder_State1-- ;
            }
         }
         else
         {
            if(Current_Encoder_B1)
            {
               /* 1101     13  A remained high, B Low > High, Clockwise*/
               Encoder_State1++ ;
            }
            else
            {
               /* 1100     12  No change*/
            }
         }
      }
      else
      {
         /*-----------------------------------------------------------------
          3 > 1       11 > 01     Clockwise one increment        11   1011
          3 > 0       11 > 00     Invalid (missed edge)          10   1010
          2 > 1       10 > 01     Invalid (missed edge)           9   1001
          2 > 0       10 > 00     Counterclockwise one increment  8   1000
         -----------------------------------------------------------------*/
         if ( Prior_Encoder_B1 )
         {
            if ( Current_Encoder_B1 )
            {
               /* 1011     11  A High > Low, B remained high, Clockwise*/
               Encoder_State1++ ;
            }
            else
            {
               /* 1010     10  A High > Low, B High > Low, Invalid (missed edge)*/
               Encoder_Missed_Edge1 = 1 ;
            }
         }
         else
         {
            if( Current_Encoder_B1 )
            {
               /* 1001     9   A High > Low, B Low > High, Invalid (missed edge)*/
               Encoder_Missed_Edge1 = 1 ;
            }
            else
            {
               /* 1000     8   A High > Low, B remained low, Counterclockwise*/
               Encoder_State1-- ;
            }
         }
      }
   }
   else
   {
      if( Current_Encoder_A1 )
      {
         /*-----------------------------------------------------------------
           1 > 3       01 > 11     Counterclockwise one increment  7   0111
           1 > 2       01 > 10     Invalid (missed edge)           6   0110
           0 > 3       00 > 11     Invalid (missed edge)           5   0101
           0 > 2       00 > 10     Clockwise one increment         4   0100
          -----------------------------------------------------------------*/
         if ( Prior_Encoder_B1 )
         {
            if ( Current_Encoder_B1 )
            {
               /* 0111     7   A Low > High, B remained high, Counterclockwise*/
               Encoder_State1-- ;
            }
            else
            {
               /* 0110     6   A Low > High, B High > Low, Invalid (missed edge)*/
               Encoder_Missed_Edge1 = 1 ;
            }
         }
         else
         {
            if ( Current_Encoder_B1 )
            {
               /* 0101     5   A Low > High, B Low > High, Invalid (missed edge)*/
               Encoder_Missed_Edge1 = 1 ;
            }
            else
            {
               /* 0100     4   A Low > High, B remained low, Clockwise*/
               Encoder_State1++ ;
            }
         }
      }
      else
      {
         /*-----------------------------------------------------------------
           1 > 1       01 > 01     No change                       3   0011
           1 > 0       01 > 00     Clockwise one increment         2   0010
           0 > 1       00 > 01     Counterclockwise one increment  1   0001
           0 > 0       00 > 00     No change                       0   0000
          -----------------------------------------------------------------*/
         if ( Prior_Encoder_B1 )
         {
            if ( Current_Encoder_B1 )
            {
               /* 0011     3   No change*/
            }
            else
            {
               /* 0010     2   A remained low, B High > Low, Clockwise*/
               Encoder_State1++ ;
            }
         }
         else
         {
            if ( Current_Encoder_B1 )
            {
               /* 0001     1   A remained low, B Low > High, Counterclockwise*/
               Encoder_State1-- ;
            }
            else
            {
               /* 0000     0   No change*/
            }
         }
      }
   }
   /*-------------------------------------------------------------------------
    On a missed edge, assume that the encoder is still moving in the same
    direction and continue adjusting the encoder state in the same direction
    -------------------------------------------------------------------------*/
   if ( Encoder_Missed_Edge1 )
   {
      Encoder_Missed_Edge1 = 0 ;
      /* Make edge timeout quicker after a missed edge*/
      Edge_Faster1 = 1 ;
      /* Check prior encoder state*/
      if ( Prior_Encoder_State1 > 0 )
      {
          Encoder_State1++ ;
      }
      else if ( Prior_Encoder_State1 < 0 )
      {
          Encoder_State1-- ;
      }
      else
      {
         /*do nothing*/
      }
   }
   /* Remember prior encoder state*/
   Prior_Encoder_State1 = Encoder_State1 ;
   /*-------------------------------------------------------------------------
          Check encoder state and increment or decrement encoder count
   -------------------------------------------------------------------------*/
   if ( Encoder_State1 == RADIO_ENC_MINIMUM_ENCODER_STATE )
   {
      Encoder_State1 = RADIO_ENC_IDLE_ENCODER_STATE ;
      Encoder_Count1++ ;
      //RIGHT_RADIO_TURN;
      key_pressed[VOL_UP] = 1;
   }
   else if ( Encoder_State1 == RADIO_ENC_MAXIMUM_ENCODER_STATE )
   {
      Encoder_State1 = RADIO_ENC_IDLE_ENCODER_STATE ;
      Encoder_Count1--;
      //LEFT_RADIO_TURN;
      key_pressed[VOL_DWN] = 1;
   }
   else if( Encoder_State1 == RADIO_ENC_IDLE_ENCODER_STATE )
   {
      //NO_RADIO_TURN;
   }
   else if( Encoder_State1 == -1 )
   {
      //NO_RADIO_TURN;
   }
   else if( Encoder_State1 == 1 )
   {
      //NO_RADIO_TURN;
   }
   else
   {
      /*do nothing*/
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Clear_Encoder_Counts1(void)
{
   Encoder_Count1 = RADIO_ENC_ZERO_ENCODER_COUNT ;
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Clear_Encoder_Direction1(void)
{
    //NO_RADIO_TURN ;
}



/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :100ms   
*******************************************************************************/
void Slow_Periodic_Encoder1( void )
{
   unsigned char l_faster_time1 ;
   if ( Edge_Timer1 )
   {
       Edge_Timer1-- ;
       if ( Edge_Faster1 )
       {
           l_faster_time1 = RADIO_ENC_EDGE_FASTER ;
           while ( l_faster_time1-- )
           {
               if ( Edge_Timer1 )
               {
                   Edge_Timer1-- ;
               }
           }
       }
       if ( Edge_Timer1 == RADIO_ENC_EDGE_DELAY_EXPIRED )
       {
           Edge_Faster1 = 0 ;
           Encoder_State1 = RADIO_ENC_IDLE_ENCODER_STATE ;
       }
   }
}




