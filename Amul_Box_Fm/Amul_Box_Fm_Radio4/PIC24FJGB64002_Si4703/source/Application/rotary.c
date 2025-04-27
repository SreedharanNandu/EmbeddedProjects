#include <stdio.h>
#include "rotary.h"
#include "types.h"
#include "io.h"
#include "radio.h"



#define RADIO_ROT_ENC_INACTIVE_RESP_TIME_MS    (100u)
#define RADIO_ENC_IDLE_ENCODER_STATE  0        ///* Idle encoder state*/
#define RADIO_ENC_MAXIMUM_ENCODER_STATE  (2 )       ///* Maximum 2 edge per detent encoder state*/
#define RADIO_ENC_MINIMUM_ENCODER_STATE  (-2 )      ///* Minimum 2 edge per detent encoder state*/
#define RADIO_ENC_ZERO_ENCODER_COUNT     (0x00)    ///* Zero*/ 
#define RADIO_ENC_MAXIMUM_ENCODER_COUNT  (0x0E)    
#define RADIO_ENC_MINIMUM_ENCODER_COUNT  (0x01)    ///* Minimum encoder value is the same for Radio*/ 
#define RADIO_ENC_ZERO_SAMPLES   (0u)
#define RADIO_ENC_TIMER_EXPIRED  (0u)
#define RADIO_ENC_DEBOUNCE_PERIOD_NORMAL  (1u)
#define RADIO_ENC_DEBOUNCE_PERIOD_SHORT  (1u)
#define RADIO_ENC_EDGE_DELAY         (20u)
#define RADIO_ENC_EDGE_FASTER        (20u)  /* 2 seconds / period (100ms)  = 20 */
#define RADIO_ENC_EDGE_DELAY_EXPIRED (0u)
#define RADIO_ENC_USE_ENCODER_B_TO_COUNT (0)/* Use only encoder Gray code phase A to count the detents*/

typedef enum
{
   NO_RADIO_TURN=0,
   LEFT_RADIO_TURN,
   RIGHT_RADIO_TURN

}Radio_Dir_T;

static volatile unsigned char Edge_Timer ;
static volatile unsigned char Encoder_Count;
static volatile unsigned char Current_Encoder_A ;
static volatile unsigned char Current_Encoder_B ;
static volatile unsigned char Prior_Encoder_A ;
static volatile unsigned char Prior_Encoder_B ;
static volatile unsigned char Encoder_Missed_Edge ;
static volatile unsigned char Edge_Faster ;
static signed char Encoder_State;
static volatile unsigned char Debounced_Signal_A_Changed ;
static volatile unsigned char Debounced_Signal_B_Changed ;
static signed char Prior_Encoder_State ;
static volatile unsigned char Encoder_Debounce_Timer_A ;
static volatile unsigned char Encoder_Debounce_Timer_B ;


void Decode_Encoder( void );
void Clear_Encoder_Counts(void);
void Clear_Encoder_Direction(void);

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Init_Encoder(void) 
{  
   unsigned char Signal_A ;
   unsigned char Signal_B ;

   /*-----------------------------------------------------------------
                Initialize Radio Encoder Rotary signals
    -----------------------------------------------------------------*/
   Signal_A = EncA_Pin();
   Signal_B = EncB_Pin();
   Current_Encoder_A = Signal_A ;
   Prior_Encoder_A = Signal_A ;
   Current_Encoder_B = Signal_B ;
   Prior_Encoder_B = Signal_B;
   Edge_Faster = 0 ;
   Edge_Timer = RADIO_ENC_EDGE_DELAY ;
   Prior_Encoder_State = RADIO_ENC_IDLE_ENCODER_STATE ;
   Encoder_State = RADIO_ENC_IDLE_ENCODER_STATE ;
   Encoder_Debounce_Timer_A = RADIO_ENC_ZERO_SAMPLES ;
   Encoder_Debounce_Timer_B = RADIO_ENC_ZERO_SAMPLES ;
   Encoder_Missed_Edge = 0 ;
   Clear_Encoder_Counts();
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  : 0.5ms or 1ms  
*******************************************************************************/
void Fast_Periodic_Encoder( void )
{
   unsigned char Signal_A ;
   unsigned char Signal_B ;
    
   Debounced_Signal_A_Changed = 0 ;
   Debounced_Signal_B_Changed = 0 ;
   /*-------------------------------------------------------------------------
                          Encoder Signal A Debouncing
   -------------------------------------------------------------------------*/
   Signal_A = EncA_Pin();
   if ( Current_Encoder_A != Signal_A )
   {
       Current_Encoder_A = Signal_A ;
       Encoder_Debounce_Timer_A = RADIO_ENC_DEBOUNCE_PERIOD_NORMAL ;
   }
   else if ( Encoder_Debounce_Timer_A > 0u )
   {
       Encoder_Debounce_Timer_A-- ;
       if ( Encoder_Debounce_Timer_A == 0u )
       {
           if ( Prior_Encoder_A != Current_Encoder_A )
           {
               Debounced_Signal_A_Changed = 1 ;
           }
       }
   }

   /*-------------------------------------------------------------------------
                     Encoder Signal B Debouncing
    ------------------------------------------------------------------------*/

   Signal_B = EncB_Pin();
   if ( Current_Encoder_B != Signal_B )
   {
       Current_Encoder_B = Signal_B ;
       Encoder_Debounce_Timer_B = RADIO_ENC_DEBOUNCE_PERIOD_NORMAL ;
   }
   else if ( Encoder_Debounce_Timer_B > 0u )
   {
       Encoder_Debounce_Timer_B-- ;
       if ( Encoder_Debounce_Timer_B == 0u )
       {
           if ( Prior_Encoder_B != Current_Encoder_B )
           {
               Debounced_Signal_B_Changed = 1 ;
           }
       }
   }

      /*-------------------------------------------------------------------------
                                   Encoder Decoding
       ------------------------------------------------------------------------*/
   /*if ICP is normal communication and wake up recvd*/

   if ( Debounced_Signal_A_Changed || Debounced_Signal_B_Changed )
   {
       /* Start or re-start edge timer at any edge*/
       Edge_Timer = RADIO_ENC_EDGE_DELAY ;
       Decode_Encoder() ;
       /* Save the current signal states as the next prior signal states*/
       if ( Debounced_Signal_A_Changed )
       {
           Prior_Encoder_A = Current_Encoder_A ;
       }
       if ( Debounced_Signal_B_Changed )
       {
           Prior_Encoder_B = Current_Encoder_B ;
       }
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Decode_Encoder( void )
{
   /* Perform binary-tree logic decoding of the signal changes*/
   if(Prior_Encoder_A)
   {
      if(Current_Encoder_A)
      {
         /*-----------------------------------------------------------------
           3 > 3       11 > 11     No change                      15   1111
           3 > 2       11 > 10     Counterclockwise one increment 14   1110
           2 > 3       10 > 11     Clockwise one increment        13   1101
           2 > 2       10 > 10     No change                      12   1100
          -----------------------------------------------------------------*/
         if(Prior_Encoder_B)
         {
            if(Current_Encoder_B)
            {
               /* 1111     15  No change*/
            }
            else
            {
               /* 1110     14  A remained high, B High > Low, Counterclockwise*/
               Encoder_State-- ;
            }
         }
         else
         {
            if(Current_Encoder_B)
            {
               /* 1101     13  A remained high, B Low > High, Clockwise*/
               Encoder_State++ ;
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
         if ( Prior_Encoder_B )
         {
            if ( Current_Encoder_B )
            {
               /* 1011     11  A High > Low, B remained high, Clockwise*/
               Encoder_State++ ;
            }
            else
            {
               /* 1010     10  A High > Low, B High > Low, Invalid (missed edge)*/
               Encoder_Missed_Edge = 1 ;
            }
         }
         else
         {
            if( Current_Encoder_B )
            {
               /* 1001     9   A High > Low, B Low > High, Invalid (missed edge)*/
               Encoder_Missed_Edge = 1 ;
            }
            else
            {
               /* 1000     8   A High > Low, B remained low, Counterclockwise*/
               Encoder_State-- ;
            }
         }
      }
   }
   else
   {
      if( Current_Encoder_A )
      {
         /*-----------------------------------------------------------------
           1 > 3       01 > 11     Counterclockwise one increment  7   0111
           1 > 2       01 > 10     Invalid (missed edge)           6   0110
           0 > 3       00 > 11     Invalid (missed edge)           5   0101
           0 > 2       00 > 10     Clockwise one increment         4   0100
          -----------------------------------------------------------------*/
         if ( Prior_Encoder_B )
         {
            if ( Current_Encoder_B )
            {
               /* 0111     7   A Low > High, B remained high, Counterclockwise*/
               Encoder_State-- ;
            }
            else
            {
               /* 0110     6   A Low > High, B High > Low, Invalid (missed edge)*/
               Encoder_Missed_Edge = 1 ;
            }
         }
         else
         {
            if ( Current_Encoder_B )
            {
               /* 0101     5   A Low > High, B Low > High, Invalid (missed edge)*/
               Encoder_Missed_Edge = 1 ;
            }
            else
            {
               /* 0100     4   A Low > High, B remained low, Clockwise*/
               Encoder_State++ ;
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
         if ( Prior_Encoder_B )
         {
            if ( Current_Encoder_B )
            {
               /* 0011     3   No change*/
            }
            else
            {
               /* 0010     2   A remained low, B High > Low, Clockwise*/
               Encoder_State++ ;
            }
         }
         else
         {
            if ( Current_Encoder_B )
            {
               /* 0001     1   A remained low, B Low > High, Counterclockwise*/
               Encoder_State-- ;
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
   if ( Encoder_Missed_Edge )
   {
      Encoder_Missed_Edge = 0 ;
      /* Make edge timeout quicker after a missed edge*/
      Edge_Faster = 1 ;
      /* Check prior encoder state*/
      if ( Prior_Encoder_State > 0 )
      {
          Encoder_State++ ;
      }
      else if ( Prior_Encoder_State < 0 )
      {
          Encoder_State-- ;
      }
      else
      {
         /*do nothing*/
      }
   }
   /* Remember prior encoder state*/
   Prior_Encoder_State = Encoder_State ;
   /*-------------------------------------------------------------------------
          Check encoder state and increment or decrement encoder count
   -------------------------------------------------------------------------*/
   if ( Encoder_State == RADIO_ENC_MINIMUM_ENCODER_STATE )
   {
      Encoder_State = RADIO_ENC_IDLE_ENCODER_STATE ;
      Encoder_Count++ ;
      //RIGHT_RADIO_TURN;
      Radio_Enc_Operation(RIGHT_TURN);
   }
   else if ( Encoder_State == RADIO_ENC_MAXIMUM_ENCODER_STATE )
   {
      Encoder_State = RADIO_ENC_IDLE_ENCODER_STATE ;
      Encoder_Count--;
      //LEFT_RADIO_TURN;
      Radio_Enc_Operation(LEFT_TURN);
   }
   else if( Encoder_State == RADIO_ENC_IDLE_ENCODER_STATE )
   {
      //NO_RADIO_TURN;
   }
   else if( Encoder_State == -1 )
   {
      //NO_RADIO_TURN;
   }
   else if( Encoder_State == 1 )
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
void Clear_Encoder_Counts(void)
{
   Encoder_Count = RADIO_ENC_ZERO_ENCODER_COUNT ;
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Clear_Encoder_Direction(void)
{
    //NO_RADIO_TURN ;
}



/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :100ms   
*******************************************************************************/
void Slow_Periodic_Encoder( void )
{
   unsigned char l_faster_time ;
   if ( Edge_Timer )
   {
       Edge_Timer-- ;
       if ( Edge_Faster )
       {
           l_faster_time = RADIO_ENC_EDGE_FASTER ;
           while ( l_faster_time-- )
           {
               if ( Edge_Timer )
               {
                   Edge_Timer-- ;
               }
           }
       }
       if ( Edge_Timer == RADIO_ENC_EDGE_DELAY_EXPIRED )
       {
           Edge_Faster = 0 ;
           Encoder_State = RADIO_ENC_IDLE_ENCODER_STATE ;
       }
   }
}




