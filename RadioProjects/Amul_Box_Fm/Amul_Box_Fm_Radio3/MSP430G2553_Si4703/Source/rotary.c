#include <stdio.h>
#include "type.h"
#include <msp430.h>


volatile unsigned char Edge_Timer[NUM_ENCODERS];
volatile unsigned char Encoder_Count[NUM_ENCODERS];
volatile unsigned char Current_Encoder_A[NUM_ENCODERS];
volatile unsigned char Current_Encoder_B[NUM_ENCODERS];
volatile unsigned char Prior_Encoder_A[NUM_ENCODERS];
volatile unsigned char Prior_Encoder_B[NUM_ENCODERS];
volatile unsigned char Encoder_Missed_Edge[NUM_ENCODERS];
volatile unsigned char Edge_Faster[NUM_ENCODERS];
signed char Encoder_State[NUM_ENCODERS];
volatile unsigned char Debounced_Signal_A_Changed[NUM_ENCODERS];
volatile unsigned char Debounced_Signal_B_Changed[NUM_ENCODERS];
signed char Prior_Encoder_State[NUM_ENCODERS];
volatile unsigned char Encoder_Debounce_Timer_A[NUM_ENCODERS];
volatile unsigned char Encoder_Debounce_Timer_B[NUM_ENCODERS];


static void Decode_Encoder(unsigned char enc);

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :
*******************************************************************************/
void Init_Encoder(void)
{
   unsigned char enc;
   unsigned char Signal_A;
   unsigned char Signal_B;

   /*-----------------------------------------------------------------
               Initialize Radio Encoder Rotary signals
    -----------------------------------------------------------------*/
   for(enc=0;enc<NUM_ENCODERS;enc++)
   {
      if(enc == ENC1)
      {
         Signal_A = ((P2IN & BIT1)?1u:0u);
         Signal_B = ((P2IN & BIT0)?1u:0u);
      }
      else if(enc == ENC2) 
      {
         Signal_A = ((P1IN & BIT5)?1u:0u);  
         Signal_B = ((P1IN & BIT4)?1u:0u);  
      }
      Current_Encoder_A[enc] = Signal_A;
      Prior_Encoder_A[enc]   = Signal_A;
      Current_Encoder_B[enc] = Signal_B;
      Prior_Encoder_B[enc]   = Signal_B;
      Edge_Faster[enc]       = 0;
      Edge_Timer[enc]        = RADIO_ENC_EDGE_DELAY;
      Prior_Encoder_State[enc] = RADIO_ENC_IDLE_ENCODER_STATE;
      Encoder_State[enc]       = RADIO_ENC_IDLE_ENCODER_STATE;
      Encoder_Debounce_Timer_A[enc] = RADIO_ENC_ZERO_SAMPLES;
      Encoder_Debounce_Timer_B[enc] = RADIO_ENC_ZERO_SAMPLES;
      Encoder_Missed_Edge[enc] = 0;
      Encoder_Count[enc] = RADIO_ENC_ZERO_ENCODER_COUNT;
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  : 0.5ms or 1ms
*******************************************************************************/
void Fast_Periodic_Encoder(void)
{
   unsigned char enc;
   unsigned char Signal_A;
   unsigned char Signal_B;

   for(enc=0;enc<NUM_ENCODERS;enc++)
   {
      
      Debounced_Signal_A_Changed[enc] = 0u;
      Debounced_Signal_B_Changed[enc] = 0u;

      /*-------------------------------------------------------------------------
                            Encoder Signal A Debouncing
      -------------------------------------------------------------------------*/
      if(enc == ENC1)
      {
          Signal_A = ((P2IN & BIT1)?1u:0u);
      }
      else if(enc == ENC2)
      {
          Signal_A = ((P1IN & BIT5)?1u:0u); 
      }

      if (Current_Encoder_A[enc] != Signal_A)
      {
          Current_Encoder_A[enc] = Signal_A;
          Encoder_Debounce_Timer_A[enc] = RADIO_ENC_DEBOUNCE_PERIOD_NORMAL;
      }
      else if (Encoder_Debounce_Timer_A[enc] > 0u)
      {
          Encoder_Debounce_Timer_A[enc]--;
          if (Encoder_Debounce_Timer_A[enc] == 0u)
          {
              if (Prior_Encoder_A[enc] != Current_Encoder_A[enc])
              {
                  Debounced_Signal_A_Changed[enc] = 1u;
              }
          }
      }

      /*-------------------------------------------------------------------------
                       Encoder Signal B Debouncing
       ------------------------------------------------------------------------*/
      if(enc == ENC1)   
      {
          Signal_B = ((P2IN & BIT0)?1u:0u);
      }
      else if(enc == ENC2)   
      {
          Signal_B = ((P1IN & BIT4)?1u:0u);  
      }

      if (Current_Encoder_B[enc] != Signal_B)
      {
          Current_Encoder_B[enc] = Signal_B;
          Encoder_Debounce_Timer_B[enc] = RADIO_ENC_DEBOUNCE_PERIOD_NORMAL;
      }
      else if (Encoder_Debounce_Timer_B[enc] > 0u)
      {
          Encoder_Debounce_Timer_B[enc]--;
          if (Encoder_Debounce_Timer_B[enc] == 0u)
          {
              if (Prior_Encoder_B[enc] != Current_Encoder_B[enc])
              {
                  Debounced_Signal_B_Changed[enc] = 1u;
              }
          }
      }

      /*-------------------------------------------------------------------------
                                     Encoder Decoding
         ------------------------------------------------------------------------*/
      if (Debounced_Signal_A_Changed[enc] || Debounced_Signal_B_Changed[enc])
      {
          /* Start or re-start edge timer at any edge */
          Edge_Timer[enc] = RADIO_ENC_EDGE_DELAY;
          Decode_Encoder(enc);
          /* Save the current signal states as the next prior signal states */
          if (Debounced_Signal_A_Changed[enc])
          {
              Prior_Encoder_A[enc] = Current_Encoder_A[enc];
          }
          if (Debounced_Signal_B_Changed[enc])
          {
              Prior_Encoder_B[enc] = Current_Encoder_B[enc];
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
static void Decode_Encoder(unsigned char enc)
{
      /* Perform binary-tree logic decoding of the signal changes */
      if(Prior_Encoder_A[enc])
      {
         if(Current_Encoder_A[enc])
         {
            /*-----------------------------------------------------------------
              3 > 3       11 > 11     No change                      15   1111
              3 > 2       11 > 10     Counterclockwise one increment 14   1110
              2 > 3       10 > 11     Clockwise one increment        13   1101
              2 > 2       10 > 10     No change                      12   1100
             -----------------------------------------------------------------*/
            if(Prior_Encoder_B[enc])
            {
               if(Current_Encoder_B[enc])
               {
                  /* 1111     15  No change */
               }
               else
               {
                  /* 1110     14  A remained high, B High > Low, Counterclockwise */
                  Encoder_State[enc]--;
               }
            }
            else
            {
               if(Current_Encoder_B[enc])
               {
                  /* 1101     13  A remained high, B Low > High, Clockwise */
                  Encoder_State[enc]++;
               }
               else
               {
                  /* 1100     12  No change */
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
            if(Prior_Encoder_B[enc])
            {
               if(Current_Encoder_B[enc])
               {
                  /* 1011     11  A High > Low, B remained high, Clockwise */
                  Encoder_State[enc]++;
               }
               else
               {
                  /* 1010     10  A High > Low, B High > Low, Invalid (missed edge) */
                  Encoder_Missed_Edge[enc] = 1;
               }
            }
            else
            {
               if(Current_Encoder_B[enc])
               {
                  /* 1001     9   A High > Low, B Low > High, Invalid (missed edge) */
                  Encoder_Missed_Edge[enc] = 1;
               }
               else
               {
                  /* 1000     8   A High > Low, B remained low, Counterclockwise */
                  Encoder_State[enc]--;
               }
            }
         }
      }
      else
      {
         if(Current_Encoder_A[enc])
         {
            /*-----------------------------------------------------------------
              1 > 3       01 > 11     Counterclockwise one increment  7   0111
              1 > 2       01 > 10     Invalid (missed edge)           6   0110
              0 > 3       00 > 11     Invalid (missed edge)           5   0101
              0 > 2       00 > 10     Clockwise one increment         4   0100
             -----------------------------------------------------------------*/
            if(Prior_Encoder_B[enc])
            {
               if(Current_Encoder_B[enc])
               {
                  /* 0111     7   A Low > High, B remained high, Counterclockwise */
                  Encoder_State[enc]--;
               }
               else
               {
                  /* 0110     6   A Low > High, B High > Low, Invalid (missed edge) */
                  Encoder_Missed_Edge[enc] = 1;
               }
            }
            else
            {
               if(Current_Encoder_B[enc])
               {
                  /* 0101     5   A Low > High, B Low > High, Invalid (missed edge) */
                  Encoder_Missed_Edge[enc] = 1;
               }
               else
               {
                  /* 0100     4   A Low > High, B remained low, Clockwise */
                  Encoder_State[enc]++;
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
            if(Prior_Encoder_B[enc])
            {
               if(Current_Encoder_B[enc])
               {
                  /* 0011     3   No change */
               }
               else
               {
                  /* 0010     2   A remained low, B High > Low, Clockwise */
                  Encoder_State[enc]++;
               }
            }
            else
            {
               if(Current_Encoder_B[enc])
               {
                  /* 0001     1   A remained low, B Low > High, Counterclockwise */
                  Encoder_State[enc]--;
               }
               else
               {
                  /* 0000     0   No change */
               }
            }
         }
      }

      /*-------------------------------------------------------------------------
       On a missed edge, assume that the encoder is still moving in the same
       direction and continue adjusting the encoder state in the same direction
       -------------------------------------------------------------------------*/
      if(Encoder_Missed_Edge[enc])
      {
         Encoder_Missed_Edge[enc] = 0u;
         /* Make edge timeout quicker after a missed edge */
         Edge_Faster[enc] = 1u;
         /* Check prior encoder state */
         if(Prior_Encoder_State[enc] > 0u)
         {
             Encoder_State[enc]++;
         }
         else if(Prior_Encoder_State[enc] < 0u)
         {
             Encoder_State[enc]--;
         }
         else
         {
            /* do nothing */
         }
      }

      /* Remember prior encoder state */
      Prior_Encoder_State[enc] = Encoder_State[enc];

      /*-------------------------------------------------------------------------
            Check encoder state and increment or decrement encoder count
      -------------------------------------------------------------------------*/
      if(Encoder_State[enc] == RADIO_ENC_MINIMUM_ENCODER_STATE)
      {
         Encoder_State[enc] = RADIO_ENC_IDLE_ENCODER_STATE;
         Encoder_Count[enc]++;
         if(enc == ENC1)
         {
            //RIGHT_RADIO_TURN;
            Radio_Enc1_Operation(RIGHT_TURN);
         }
         else if(enc == ENC2)
         {
            //RIGHT_VOL_TURN;
            Radio_Enc2_Operation(RIGHT_TURN);
         }
      }
      else if(Encoder_State[enc] == RADIO_ENC_MAXIMUM_ENCODER_STATE)
      {
         Encoder_State[enc] = RADIO_ENC_IDLE_ENCODER_STATE;
         Encoder_Count[enc]--;
         if(enc == ENC1)
         {
            //LEFT_RADIO_TURN;
            Radio_Enc1_Operation(LEFT_TURN);
         }
         else if(enc == ENC2)
         {
            //LEFT_VOL_TURN;
            Radio_Enc2_Operation(LEFT_TURN);
         }
      }
      else if(Encoder_State[enc] == RADIO_ENC_IDLE_ENCODER_STATE)
      {
         /* NO_RADIO_TURN */
      }
      else if(Encoder_State[enc] == -1)
      {
         /* NO_RADIO_TURN */
      }
      else if(Encoder_State[enc] == 1u)
      {
         /* NO_RADIO_TURN */
      }
      else
      {
         /* do nothing */
      }
}



/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  : 100ms
*******************************************************************************/
void Slow_Periodic_Encoder(void)
{
   unsigned char enc;
   unsigned char l_faster_time;

   for(enc=0;enc<NUM_ENCODERS;enc++)
   {
      if(Edge_Timer[enc])
      {
          Edge_Timer[enc]--;
          if(Edge_Faster[enc])
          {
              l_faster_time = RADIO_ENC_EDGE_FASTER;
              while(l_faster_time--)
              {
                  if(Edge_Timer[enc])
                  {
                      Edge_Timer[enc]--;
                  }
              }
          }
          if(Edge_Timer[enc] == RADIO_ENC_EDGE_DELAY_EXPIRED)
          {
              Edge_Faster[enc]    = 0u;
              Encoder_State[enc]  = RADIO_ENC_IDLE_ENCODER_STATE;
          }
      }
   }
}
