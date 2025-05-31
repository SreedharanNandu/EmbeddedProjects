
// Define to prevent recursive inclusion
#ifndef INTF_UTL_H
#define INTF_UTL_H


#include <io78f1827_48.h>
#include <io78f1827_48_ext.h>
#include "system_def.h"

#include "HAL_Init.h"
#include "HAL_Timer_API.h"
#include "HAL_ADC_API.h"
#include "HAL_USART.h"
#include "INTF_VTMR_API.h"
#include "APP_Serial.h"
#include "string.h"  



// START GLOBAL DEFINITION
///////////////////////////////////////////////////////////////////////////////

// global variables


// global function defines
extern void UTL_Powerup_Init_A(void);
extern void UTL_Wakeup_Init_A(void);
extern void UTL_Sleep_A(void);
extern void UTL_Emergency_Shutdown_A(void);

// Signed version
extern s32 UTL_HW_Multiply(s16 Multiplicand, s16 Multiplier, bool InIsrContext);
extern s32 UTL_HW_Divide(s32 Dividend, s32 Divisor);
// Unsigned version
extern u32 UTL_HW_Multiply_unsigned(u16 Multiplicand, u16 Multiplier);
extern u32 UTL_HW_Divide_unsigned(u32 Dividend, u32 Divisor);

extern void UTL_Interpolate_U8(const u8 X1, const u8 Y1, const u8 X2, const u8 Y2, const u8 X_Value, u8 * const Y_Value);
extern u16 UTL_U8_Percentage_of_U16(const u16 value, const u8 percentage);
extern u8 UTL_Average_of_2_U8_Values(const u8 value_1, const u8 value_2);

// macro defines

///////////////////////////////////////////////////////////////////////////////
// END GLOBAL DEFINITION


// START LOCAL DEFINITION
///////////////////////////////////////////////////////////////////////////////
//local variable defines

//local function defines
u8 utl_check_sign_and_convert_16bit(s16 *value);
u8 utl_check_sign_and_convert_32bit(s32 *value);

///////////////////////////////////////////////////////////////////////////////
// END LOCAL DEFINITION

#endif

