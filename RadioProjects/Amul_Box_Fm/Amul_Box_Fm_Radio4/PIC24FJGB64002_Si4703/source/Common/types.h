#ifndef TYPES_H
#define TYPES_H

#define DBG_MSG 1
#define DBG_RDR_MSG 1

#define FUNCTION_NOT_USED   0xFF

#define NOP               asm("nop")

// These are the standard variable types that should be ALWAYS used in the project
typedef unsigned char           uint8_t;              // 8-bit unsigned
typedef unsigned short int      uint16_t;             // 16-bit unsigned
typedef unsigned long           uint32_t;             // 32-bit unsigned
typedef unsigned  long long     uint64_t;             // 64-bit signed
typedef char                    int8_t;               // 8-bit signed
typedef signed short int        int16_t;              // 16-bit signed
typedef signed long             int32_t;              // 32-bit signed
typedef signed long long        int64_t;              // 64-bit signed

// these types should be strictly avoided unless the microprocessor has a floating point unit
// double can be same as float or long double based on compiler parameter
typedef float                  real32_t;      // 32-bit floating point
typedef long double            real64_t;      // 64-bit floating point

enum status                           // ENUMERATION CONTANTS TO INDICATE FUNCTION'S 
{                                 // RETURN STATUS
   FAILURE,                           // 1 FOR SUCCESS 0 FOR FAILURE
   SUCCESS
};



#endif 

//EOF 
