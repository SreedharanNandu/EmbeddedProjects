// Define to include local variables in the header file
#include <io78f1827_48.h>
#include <io78f1827_48_ext.h>
#include "system_def.h"

// common includes
#include "INTF_UTL.h"


// Function Prototypes


///////////////////////////////////////////////////////////////////////////////
 // Function                : u8 utl_check_sign_and_convert_16bit(s16 *value)
 //
 // Description             : Routine determines sign and removes sign (if negative)
 //
///////////////////////////////////////////////////////////////////////////////

u8 utl_check_sign_and_convert_16bit(s16 *value)
{
  u8 utl_sign = 0u;
  if(*value&0x8000)
  {
    utl_sign = 1u; // negative
    *value = ~(*value-1);
  }
  return(utl_sign);
} // end of function utl_check_sign_and_convert_16bit

///////////////////////////////////////////////////////////////////////////////
 // Function                : u8 utl_check_sign_and_convert_32bit(s32 *value)
 //
 // Description             : Routine determines sign and removes sign (if negative)
 //
///////////////////////////////////////////////////////////////////////////////
u8 utl_check_sign_and_convert_32bit(s32 *value)
{
  u8 utl_sign = 0u;
  if(*value&0x80000000)
  {
    utl_sign = 1u;
    *value = ~(*value-1);
  }
  return(utl_sign);
} // end of function utl_check_sign_and_convert_32bit

    

///////////////////////////////////////////////////////////////////////////////
 // Function                : s32 UTL_HW_Multiply(s16 Multiplicand, s16 Multiplier)
 //
 // Description             : Routine uses hardware capability to multiply two numbers
 //
///////////////////////////////////////////////////////////////////////////////
s32 UTL_HW_Multiply(s16 Multiplicand, s16 Multiplier, bool InIsrContext)
{
  s32 uti_multiplied_value;
  u8 utl_sign_multiplier;
  
  utl_sign_multiplier = utl_check_sign_and_convert_16bit(&Multiplicand);
  utl_sign_multiplier ^= utl_check_sign_and_convert_16bit(&Multiplier);
  
  if (InIsrContext == FALSE)
  {
     // enter critical section
     BSP_DISABLE_INTERRUPTS();
  }
  MDUC = 0x00u; // Clear DIVMODE (BIT7) to setup for multiply
  MDAL = Multiplicand;
  MDAH = Multiplier;
  NO_OPERATION(); // wait at least one clock
  uti_multiplied_value = MDBH; // take result and shove it into 32 value // note if the highest bit is set this will problematic
   uti_multiplied_value = (uti_multiplied_value<<16); // shift bits into position
  uti_multiplied_value += MDBL; 
  
  if (InIsrContext == FALSE)
  {
     // exit critical section
     BSP_ENABLE_INTERRUPTS();
  }

  if(utl_sign_multiplier) // return the sign to the result
  {
    return (-uti_multiplied_value);
  } // end if

  return (uti_multiplied_value);  
} // end of function UTL_HW_Multiply

///////////////////////////////////////////////////////////////////////////////
 // Function                : s32 UTL_HW_Divide(s32 Dividend, s32 Divisor)
 //
 // Description             : Routine uses hardware capability to divide two numbers
 //
///////////////////////////////////////////////////////////////////////////////
s32 UTL_HW_Divide(s32 Dividend, s32 Divisor)
{
  
  s32 uti_quotient_value;
  u8 utl_sign_divisor;
  
  utl_sign_divisor = utl_check_sign_and_convert_32bit(&Dividend);
  utl_sign_divisor ^= utl_check_sign_and_convert_32bit(&Divisor);  
    
  // enter critical section
  BSP_DISABLE_INTERRUPTS();

  MDUC |= 0x80u; // Set DIVMODE (BIT7) to setup for Divide 
  MDAH  = (Dividend>>16);
  MDAL = Dividend;
  
  MDBH = (Divisor>>16);
  MDBL = Divisor;
  MDUC |= 0x01u; // Enable division, set DIVST (Bit 0)
  
  while(MDUC&0x01u) // at least 16 clocks...
   { // shouldn't be needed but debugger likes to crash without it...
     NO_OPERATION();
   } // end while

  uti_quotient_value = MDAH; // take result and shove it into 32 value // note if the highest bit is set this will problematic
   uti_quotient_value = (uti_quotient_value<<16);  // shift bits into position
  uti_quotient_value += MDAL; // add lower bits
  
  // exit critical section
  BSP_ENABLE_INTERRUPTS();

  if(utl_sign_divisor)
  {
    return(-uti_quotient_value);
  } // end if

  return(uti_quotient_value);
  
} // end of function UTL_HW_Divide

///////////////////////////////////////////////////////////////////////////////
 // Function                : u32 UTL_HW_Multiply_unsigned(u16 Multiplicand, u16 Multiplier)
 //
 // Description             : Routine uses hardware capability to multiply two numbers
 //
///////////////////////////////////////////////////////////////////////////////
u32 UTL_HW_Multiply_unsigned(u16 Multiplicand, u16 Multiplier)
{
  u32 uti_multiplied_value;
  
  // enter critical section
  BSP_DISABLE_INTERRUPTS();

//  while(MDUC&0x01); // make sure divide isn't in process // optional
  MDUC = 0x00u; // Clear DIVMODE (BIT7) to setup for multiply
  MDAL = Multiplicand;
  MDAH = Multiplier;
  NO_OPERATION(); // wait at least one clock
  uti_multiplied_value = MDBH; // take result and shove it into 32 value 
   uti_multiplied_value = (uti_multiplied_value<<16); // shift bits into position
  uti_multiplied_value += MDBL; 

  // exit critical section
  BSP_ENABLE_INTERRUPTS();
  
  return (uti_multiplied_value);  
} // end of function UTL_HW_Multiply_unsigned

///////////////////////////////////////////////////////////////////////////////
 // Function                : u32 UTL_HW_Divide_unsigned(u32 Dividend, u32 Divisor)
 //
 // Description             : Routine uses hardware capability to divide two numbers
 //
///////////////////////////////////////////////////////////////////////////////
u32 UTL_HW_Divide_unsigned(u32 Dividend, u32 Divisor)
{
  
  u32 uti_quotient_value;
    
  // enter critical section
  BSP_DISABLE_INTERRUPTS();

  MDUC |= 0x80u; // Set DIVMODE (BIT7) to setup for Divide 
  MDAH  = (Dividend>>16);
  MDAL = Dividend;
  
  MDBH = (Divisor>>16);
  MDBL = Divisor;
  MDUC |= 0x01u; // Enable division, set DIVST (Bit 0)
  
  while(MDUC&0x01u) // at least 16 clocks...
   { // shouldn't be needed but debugger likes to crash without it...
     NO_OPERATION();
   } // end while

  uti_quotient_value = MDAH; // take result and shove it into 32 value 
   uti_quotient_value = (uti_quotient_value<<16);  // shift bits into position
  uti_quotient_value += MDAL; // add lower bits

  // exit critical section
  BSP_ENABLE_INTERRUPTS();
  
  return(uti_quotient_value);
  
} // end of function UTL_HW_Divide_unsigned



///////////////////////////////////////////////////////////////////////////////
 // Function                : UTL_Interpolate_U8
 //
 // Description             : Implements linear interpolation between u8 values
 //
///////////////////////////////////////////////////////////////////////////////
void UTL_Interpolate_U8(const u8 X1, const u8 Y1, const u8 X2, const u8 Y2, const u8 X_Value, u8 * const Y_Value)
{
  u32 result, dividend, divisor; 
  u16 multiplicand, multiplier; 
  
   if ((X_Value > X1) && (X_Value < X2))
   {
                  multiplier = (Y1 > Y2) ? (u16)(Y1 - Y2) : (u16)(Y2 - Y1);
                  multiplicand = (u16)(X_Value - X1);
                  dividend = UTL_HW_Multiply_unsigned(multiplicand, multiplier);
                  divisor = (u32)(X2 - X1);
                  result = UTL_HW_Divide_unsigned(dividend, divisor);
                  *Y_Value = (Y1 > Y2) ? (Y1 - (u8)result) : Y1 + (u8)result;                
   }
   else
   {
      // conditions not correct
   }
}

//
// input:  u16 value, u8 percentage
// output: u16 result
//
// percentage 0 to 100: 1% standard resolution for full range
//   result = value * percentage / 100
//
// percentage 101 to 200: 0.1% fine resolution for first 10%
//   result = value * (percentage - 100) / 1000
//
// pernctage 201 to 250: 0.01% finer resultion for first 0.5%
//   result = value * (percentage - 200) / 10000
//
// percentage 251 to 255: no change
//   result = value
//
u16 UTL_U8_Percentage_of_U16(const u16 value, const u8 percentage)
{
   u16 multiplicand;
   u32 divisor;
   u32 result;

   if (percentage <= 100u)
   {
      multiplicand = (u16)percentage;
      divisor = 100u;
   }
   else if (percentage <= 200u)
   {
      multiplicand = (u16)(percentage - 100u);
      divisor = 1000u;
   }
   else if (percentage <= 250u)
   {
      multiplicand = (u16)(percentage - 200u);
      divisor = 10000u;
   }
   else
   {
      return value;
   }

   result = UTL_HW_Multiply_unsigned(value, multiplicand);

   result = UTL_HW_Divide_unsigned(result, divisor);

   return (u16)result;
}

u8 UTL_Average_of_2_U8_Values(const u8 value_1, const u8 value_2)
{
   u16 temp_value;

   temp_value = (u16)(value_1 + value_2);
   temp_value = temp_value >> 1;

   return ((u8)temp_value);
}
