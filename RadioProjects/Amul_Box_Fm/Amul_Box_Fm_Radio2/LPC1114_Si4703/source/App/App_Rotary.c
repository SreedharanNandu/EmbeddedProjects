#include <stdio.h>
#include "App_Rotary.h"
#include "App_Si.h"
#include "gpio.h"
#include "App_Led.h"


static volatile unsigned char Edge_Timer[MAX_ENCODERS] ;
static volatile unsigned char Encoder_Count[MAX_ENCODERS];
static volatile unsigned char Current_Encoder_A[MAX_ENCODERS] ;
static volatile unsigned char Current_Encoder_B[MAX_ENCODERS] ;
static volatile unsigned char Prior_Encoder_A[MAX_ENCODERS] ;
static volatile unsigned char Prior_Encoder_B[MAX_ENCODERS] ;
static volatile unsigned char Encoder_Missed_Edge[MAX_ENCODERS] ;
static volatile unsigned char Edge_Faster[MAX_ENCODERS] ;
static signed char Encoder_State[MAX_ENCODERS];
static volatile unsigned char Debounced_Signal_A_Changed[MAX_ENCODERS] ;
static volatile unsigned char Debounced_Signal_B_Changed[MAX_ENCODERS] ;
static signed char Prior_Encoder_State[MAX_ENCODERS] ;
static volatile unsigned char Encoder_Debounce_Timer_A[MAX_ENCODERS] ;
static volatile unsigned char Encoder_Debounce_Timer_B[MAX_ENCODERS] ;


void Decode_Encoder( unsigned char i );
void Clear_Encoder_Direction(void);

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Init_Encoder(void) 
{  
   volatile unsigned char i;
	 volatile unsigned char Signal_A[MAX_ENCODERS] ;
   volatile unsigned char Signal_B[MAX_ENCODERS] ;

   /*-----------------------------------------------------------------
                Initialize Radio Encoder Rotary signals
    -----------------------------------------------------------------*/
   for(i=0u;i<MAX_ENCODERS;i++)
   {
      if(i == ENC1)
      {
         Signal_A[i] = GPIOGetValue(1u,4u);
         Signal_B[i] = GPIOGetValue(1u,5u);
      }
      else if(i == ENC2)
      {
         Signal_A[i] = GPIOGetValue(1u,0u);
         Signal_B[i] = GPIOGetValue(1u,1u);
      }
            
      Current_Encoder_A[i] = Signal_A[i] ;
      Prior_Encoder_A[i] = Signal_A[i] ;
      Current_Encoder_B[i] = Signal_B[i] ;
      Prior_Encoder_B[i] = Signal_B[i];
      Edge_Faster[i] = 0u ;
      Edge_Timer[i] = RADIO_ENC_EDGE_DELAY ;
      Prior_Encoder_State[i] = RADIO_ENC_IDLE_ENCODER_STATE ;
      Encoder_State[i] = RADIO_ENC_IDLE_ENCODER_STATE ;
      Encoder_Debounce_Timer_A[i] = RADIO_ENC_ZERO_SAMPLES ;
      Encoder_Debounce_Timer_B[i] = RADIO_ENC_ZERO_SAMPLES ;
      Encoder_Missed_Edge[i] = 0u;
      Encoder_Count[i] = RADIO_ENC_ZERO_ENCODER_COUNT ;
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  : 0.5ms or 1ms  
*******************************************************************************/
void Fast_Periodic_Encoder( void )
{
   volatile unsigned char i;
	 volatile unsigned char Signal_A[MAX_ENCODERS] ;
   volatile unsigned char Signal_B[MAX_ENCODERS] ;
    
   for(i=0u;i<MAX_ENCODERS;i++)
   {
      
      Debounced_Signal_A_Changed[i] = 0u ;
      Debounced_Signal_B_Changed[i] = 0u ;
      /*-------------------------------------------------------------------------
                             Encoder Signal A Debouncing
      -------------------------------------------------------------------------*/
      if(i == ENC1)
      {
         Signal_A[i] = GPIOGetValue(1u,4u);
      }
      else if(i == ENC2)
      {
         Signal_A[i] = GPIOGetValue(1u,0u);
      }
      if ( Current_Encoder_A[i] != Signal_A[i] )
      {
          Current_Encoder_A[i] = Signal_A[i] ;
          Encoder_Debounce_Timer_A [i]= RADIO_ENC_DEBOUNCE_PERIOD_NORMAL ;
      }
      else if ( Encoder_Debounce_Timer_A[i] > 0u )
      {
          Encoder_Debounce_Timer_A[i]-- ;
          if ( Encoder_Debounce_Timer_A[i] == 0u )
          {
              if ( Prior_Encoder_A[i] != Current_Encoder_A[i] )
              {
                  Debounced_Signal_A_Changed[i] = 1u ;
              }
          }
      }

      /*-------------------------------------------------------------------------
                        Encoder Signal B Debouncing
       ------------------------------------------------------------------------*/

      if(i == ENC1)
      {
         Signal_B[i] = GPIOGetValue(1u,5u);
      }
      else if(i == ENC2)
      {
         Signal_B[i] = GPIOGetValue(1u,1u);
      }
      if ( Current_Encoder_B[i] != Signal_B[i] )
      {
          Current_Encoder_B[i] = Signal_B[i] ;
          Encoder_Debounce_Timer_B[i] = RADIO_ENC_DEBOUNCE_PERIOD_NORMAL ;
      }
      else if ( Encoder_Debounce_Timer_B[i] > 0u )
      {
          Encoder_Debounce_Timer_B[i]-- ;
          if ( Encoder_Debounce_Timer_B[i] == 0u )
          {
              if ( Prior_Encoder_B[i] != Current_Encoder_B[i] )
              {
                  Debounced_Signal_B_Changed[i] = 1u ;
              }
          }
      }

         /*-------------------------------------------------------------------------
                                      Encoder Decoding
          ------------------------------------------------------------------------*/
      /*if ICP is normal communication and wake up recvd*/

      if ( Debounced_Signal_A_Changed[i] || Debounced_Signal_B_Changed[i] )
      {
          /* Start or re-start edge timer at any edge*/
          Edge_Timer[i] = RADIO_ENC_EDGE_DELAY ;
          Decode_Encoder(i) ;
          /* Save the current signal states as the next prior signal states*/
          if ( Debounced_Signal_A_Changed[i] )
          {
              Prior_Encoder_A[i] = Current_Encoder_A[i] ;
          }
          if ( Debounced_Signal_B_Changed[i] )
          {
              Prior_Encoder_B[i] = Current_Encoder_B[i] ;
          }
      }
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Decode_Encoder( unsigned char i )
{
   /* Perform binary-tree logic decoding of the signal changes*/
   if(Prior_Encoder_A[i])
   {
      if(Current_Encoder_A[i])
      {
         /*-----------------------------------------------------------------
           3 > 3       11 > 11     No change                      15   1111
           3 > 2       11 > 10     Counterclockwise one increment 14   1110
           2 > 3       10 > 11     Clockwise one increment        13   1101
           2 > 2       10 > 10     No change                      12   1100
          -----------------------------------------------------------------*/
         if(Prior_Encoder_B[i])
         {
            if(Current_Encoder_B[i])
            {
               /* 1111     15  No change*/
            }
            else
            {
               /* 1110     14  A remained high, B High > Low, Counterclockwise*/
               Encoder_State[i]-- ;
            }
         }
         else
         {
            if(Current_Encoder_B[i])
            {
               /* 1101     13  A remained high, B Low > High, Clockwise*/
               Encoder_State[i]++ ;
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
         if ( Prior_Encoder_B[i] )
         {
            if ( Current_Encoder_B[i] )
            {
               /* 1011     11  A High > Low, B remained high, Clockwise*/
               Encoder_State[i]++ ;
            }
            else
            {
               /* 1010     10  A High > Low, B High > Low, Invalid (missed edge)*/
               Encoder_Missed_Edge[i] = 1u ;
            }
         }
         else
         {
            if( Current_Encoder_B[i] )
            {
               /* 1001     9   A High > Low, B Low > High, Invalid (missed edge)*/
               Encoder_Missed_Edge[i] = 1u ;
            }
            else
            {
               /* 1000     8   A High > Low, B remained low, Counterclockwise*/
               Encoder_State[i]-- ;
            }
         }
      }
   }
   else
   {
      if( Current_Encoder_A[i] )
      {
         /*-----------------------------------------------------------------
           1 > 3       01 > 11     Counterclockwise one increment  7   0111
           1 > 2       01 > 10     Invalid (missed edge)           6   0110
           0 > 3       00 > 11     Invalid (missed edge)           5   0101
           0 > 2       00 > 10     Clockwise one increment         4   0100
          -----------------------------------------------------------------*/
         if ( Prior_Encoder_B[i] )
         {
            if ( Current_Encoder_B[i] )
            {
               /* 0111     7   A Low > High, B remained high, Counterclockwise*/
               Encoder_State[i]-- ;
            }
            else
            {
               /* 0110     6   A Low > High, B High > Low, Invalid (missed edge)*/
               Encoder_Missed_Edge[i] = 1u ;
            }
         }
         else
         {
            if ( Current_Encoder_B[i] )
            {
               /* 0101     5   A Low > High, B Low > High, Invalid (missed edge)*/
               Encoder_Missed_Edge[i] = 1u ;
            }
            else
            {
               /* 0100     4   A Low > High, B remained low, Clockwise*/
               Encoder_State[i]++ ;
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
         if ( Prior_Encoder_B[i] )
         {
            if ( Current_Encoder_B[i] )
            {
               /* 0011     3   No change*/
            }
            else
            {
               /* 0010     2   A remained low, B High > Low, Clockwise*/
               Encoder_State[i]++ ;
            }
         }
         else
         {
            if ( Current_Encoder_B[i] )
            {
               /* 0001     1   A remained low, B Low > High, Counterclockwise*/
               Encoder_State[i]-- ;
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
   if ( Encoder_Missed_Edge[i] )
   {
      Encoder_Missed_Edge[i] = 0 ;
      /* Make edge timeout quicker after a missed edge*/
      Edge_Faster[i] = 1u ;
      /* Check prior encoder state*/
      if ( Prior_Encoder_State[i] > 0 )
      {
          Encoder_State[i]++ ;
      }
      else if ( Prior_Encoder_State[i] < 0 )
      {
          Encoder_State[i]-- ;
      }
      else
      {
         /*do nothing*/
      }
   }
   /* Remember prior encoder state*/
   Prior_Encoder_State[i] = Encoder_State[i] ;
   /*-------------------------------------------------------------------------
          Check encoder state and increment or decrement encoder count
   -------------------------------------------------------------------------*/
   if ( Encoder_State[i] == RADIO_ENC_MINIMUM_ENCODER_STATE )
   {
      Encoder_State[i] = RADIO_ENC_IDLE_ENCODER_STATE ;
      Encoder_Count[i]++ ;
      //RIGHT_RADIO_TURN;
      if(i == ENC1)
      {
         Radio_Enc_Tune_Operation(RIGHT_TURN);
      }
      else if(i == ENC2)
      {
         Radio_Enc_Volume_Operation(RIGHT_TURN);
      }
   }
   else if ( Encoder_State[i] == RADIO_ENC_MAXIMUM_ENCODER_STATE )
   {
      Encoder_State[i] = RADIO_ENC_IDLE_ENCODER_STATE ;
      Encoder_Count[i]--;
      //LEFT_RADIO_TURN;
      if(i == ENC1)
      {
         Radio_Enc_Tune_Operation(LEFT_TURN);
      }
      else if(i == ENC2)
      {
         Radio_Enc_Volume_Operation(LEFT_TURN);
      }
   }
   else if( Encoder_State[i] == RADIO_ENC_IDLE_ENCODER_STATE )
   {
      //NO_RADIO_TURN;
   }
   else if( Encoder_State[i] == -1 )
   {
      //NO_RADIO_TURN;
   }
   else if( Encoder_State[i] == 1 )
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
 Description  :100ms   
*******************************************************************************/
void Slow_Periodic_Encoder( void )
{
   volatile unsigned char i;
	 volatile unsigned char l_faster_time[MAX_ENCODERS] ;
   
   for(i=0u;i<MAX_ENCODERS;i++)
   {
      if(Edge_Timer[i])
      {
         Edge_Timer[i]-- ;
         if(Edge_Faster[i])
         {
            l_faster_time[i] = RADIO_ENC_EDGE_FASTER ;
            while(l_faster_time[i]--)
            {
               if(Edge_Timer[i])
               {
                  Edge_Timer[i]-- ;
               }
            }
         }
         if(Edge_Timer[i] == RADIO_ENC_EDGE_DELAY_EXPIRED)
         {
            Edge_Faster[i] = 0 ;
            Encoder_State[i] = RADIO_ENC_IDLE_ENCODER_STATE;
         }
      }
   }
}




