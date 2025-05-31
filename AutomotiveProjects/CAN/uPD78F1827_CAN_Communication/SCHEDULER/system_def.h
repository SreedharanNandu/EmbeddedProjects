/******************************************************************************
NAME:         sysdef.h

DESCRIPTION:  This file contains system data types         
              
Date:  
Revision No:
****************************************************************************/

#include <io78f1827_48.h>


/* Define to prevent recursive inclusion*/
#ifndef SYSTEM_DEF_H
#define SYSTEM_DEF_H

#define _IAR

/* type defs*/
typedef unsigned char bool;

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef signed long  s32;
typedef signed short s16;
typedef signed char  s8;

typedef float f32;
typedef double d64;

typedef volatile unsigned long  vu32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char  vu8;

typedef volatile signed long  vs32;
typedef volatile signed short vs16;
typedef volatile signed char  vs8;

typedef volatile float vf32;
typedef volatile double vd64;

typedef struct
{
  u8 x;
  u8 y;
} u8_datum;

/* global macro defines*/

#define U8_MAX     ((u8)255)
#define U16_MAX    ((u16)65535)
#define U32_MAX    ((u32)4294967295)

#define Abs( x )  (( (x) < 0) ? -(x) : (x) )

/* The types of these must be the same or the result is nonsense. */
#define Min( x,y )  (( (x) < (y)) ? (x) : (y) )
#define Max( x,y )  (( (x) > (y)) ? (x) : (y) )

/******************************
* Defines for Portability
*******************************/
#define PAGEZERO  @dir
#define INTERRUPT @interrupt
#define FAR       @far
#define EEPROM    @eeprom
#define TASK      @far void
#define CAL_CONST const

/*********************************************
* Object Locating Directives
**********************************************/
/* This will cause the object to be placed in 16 bit address space */
#define NEAR_ADDR @near

/* This will cause the object to be placed in 8 bit address space */
#define SHORT_MEMORY @tiny



/*Commonly Used 8-bit Bits*/
#define BIT0_MASK            ( (u8)0x01 )
#define BIT1_MASK            ( (u8)0x02 )
#define BIT2_MASK            ( (u8)0x04 )
#define BIT3_MASK            ( (u8)0x08 )
#define BIT4_MASK            ( (u8)0x10 )
#define BIT5_MASK            ( (u8)0x20 )
#define BIT6_MASK            ( (u8)0x40 )
#define BIT7_MASK            ( (u8)0x80 )

#ifndef BIT0
#define BIT0            ( (unsigned char )0x01 )
#define BIT1            ( (unsigned char )0x02 )
#define BIT2            ( (unsigned char )0x04 )
#define BIT3            ( (unsigned char )0x08 )
#define BIT4            ( (unsigned char )0x10 )
#define BIT5            ( (unsigned char )0x20 )
#define BIT6            ( (unsigned char )0x40 )
#define BIT7            ( (unsigned char )0x80 )
#endif

/* Commonly Used 16-bit Bits*/
#define BIT0_U16            ( (u16)0x0001 )
#define BIT1_U16            ( (u16)0x0002 )
#define BIT2_U16            ( (u16)0x0004 )
#define BIT3_U16            ( (u16)0x0008 )
#define BIT4_U16            ( (u16)0x0010 )
#define BIT5_U16            ( (u16)0x0020 )
#define BIT6_U16            ( (u16)0x0040 )
#define BIT7_U16            ( (u16)0x0080 )
#define BIT8_U16            ( (u16)0x0100 )
#define BIT9_U16            ( (u16)0x0200 )
#define BIT10_U16           ( (u16)0x0400 )
#define BIT11_U16           ( (u16)0x0800 )
#define BIT12_U16           ( (u16)0x1000 )
#define BIT13_U16           ( (u16)0x2000 )
#define BIT14_U16           ( (u16)0x4000 )
#define BIT15_U16           ( (u16)0x8000 )

/*Commonly Used Bytes*/
#define BYTE0           ( (u8)0 )
#define BYTE1           ( (u8)1 )
#define BYTE2           ( (u8)2 )
#define BYTE3           ( (u8)3 )
#define BYTE4           ( (u8)4 )
#define BYTE5           ( (u8)5 )
#define BYTE6           ( (u8)6 )
#define BYTE7           ( (u8)7 )
#define BYTE8           ( (u8)8 )
#define BYTE9           ( (u8)9 )
#define BYTE10           ( (u8)10 )

#define WORD0 ( (u16)0 )
#define WORD1 ( (u16)1 )
#define WORD2 ( (u16)2 )
#define WORD3 ( (u16)3 )
#define WORD4 ( (u16)4 )
#define WORD5 ( (u16)5 )
#define WORD6 ( (u16)6 )
#define WORD7 ( (u16)7 )
#define WORD8 ( (u16)8 )
#define WORD9 ( (u16)9 )

/*Boolean Data type values*/
#ifndef ON
#define ON            ( 1 )
#endif


#ifndef OFF
#define OFF           ( 0 )
#endif

#ifndef TRUE
#define TRUE            ( 1 )
#endif

#ifndef FALSE
#define FALSE           ( 0 )
#endif

#ifndef SET
#define SET ( 1 )
#endif

#ifndef RESET
#define RESET ( 0 )
#endif

#ifndef CLEAR
#define CLEAR ( 0 )
#endif

#ifndef ENABLE
#define ENABLE ( 1 )
#endif

#ifndef DISABLE
#define DISABLE ( 0 )
#endif

#ifndef HIGH
#define HIGH (1)
#endif

#ifndef LOW
#define LOW (0)
#endif

#ifndef START
#define START            ( 1 )
#endif

/*I/O Port Mode (PM) Register defines*/
#define Output_Port                     0       /*Setting port mode register bit to 0 makes I/O port an output*/
#define Input_Port                      1       /*Setting port mode register bit to 1 makes I/O port an input*/


/*Hardware logic levels*/
#define Logic_High                      1
#define Logic_Low                       0

/*Null definition*/
#ifndef NULL
#define NULL            ((void *)0)
#endif

#define SetRegBit(reg, bit)                                     (reg |= reg##_##bit##_##MASK)
#define ClrRegBit(reg, bit)                                     (reg &= ~reg##_##bit##_##MASK)
#define GetRegBit(reg, bit)                                     (reg & reg##_##bit##_##MASK)
#define SetReg(reg, val)                                        (reg = (word)(val))
#define GetReg(reg)                                             (reg)
#define SetRegBits(reg, mask)                                   (reg |= (word)(mask))
#define GetRegBits(reg, mask)                                   (reg & (word)(mask))
#define ClrRegBits(reg, mask)                                   (reg &= (word)(~(mask)))
#define SetRegBitGroup(reg, bits, val)                          (reg = (word)((reg & ~reg##_##bits##_##MASK) | ((val) << reg##_##bits##_##BITNUM)))
#define GetRegBitGroup(reg, bits)                               ((reg & reg##_##bits##_##MASK) >> reg##_##bits##_##BITNUM)
#define SetRegMask(reg, maskAnd, maskOr)                        (reg = (word)((getReg(reg) & ~(maskAnd)) | (maskOr)))
#define SetRegBitVal(reg, bit, val)                             ((val) == 0 ? (reg &= ~reg##_##bit##_##MASK) : (reg |= reg##_##bit##_##MASK))
#define ChangeRegBits(reg, mask)                                (reg ^= (mask))
#define ChangeRegBit(reg, bit)                                  (reg ^= reg##_##bit##_##MASK)

/******************************************************************/
/* Uniform multiplatform peripheral access macros - 16 bit access */
/******************************************************************/
#define SetReg16Bit(RegName, BitName)                           (RegName |= RegName##_##BitName##_##MASK)
#define ClrReg16Bit(RegName, BitName)                           (RegName &= ~(word)RegName##_##BitName##_##MASK)
#define InvertReg16Bit(RegName, BitName)                        (RegName ^= RegName##_##BitName##_##MASK)
#define TestReg16Bit(RegName, BitName)                          (RegName & RegName##_##BitName##_##MASK)

/* Whole peripheral register access macros */
#define SetReg16(RegName, val)                                  (RegName = (word)(val))
#define GetReg16(RegName)                                       (RegName)

/* Bits peripheral register access macros */
#define TestReg16Bits(RegName, GetMask)                         (RegName & (GetMask))
#define ClrReg16Bits(RegName, ClrMask)                          (RegName &= (word)(~(word)(ClrMask)))
#define SetReg16Bits(RegName, SetMask)                          (RegName |= (word)(SetMask))
#define InvertReg16Bits(RegName, InvMask)                       (RegName ^= (word)(InvMask))
#define ClrSetReg16Bits(RegName, ClrMask, SetMask)              (RegName = (RegName & (~(word)(ClrMask))) | (word)(SetMask))
#define SeqClrSetReg16Bits(RegName, BitsMask, BitsVal)          ((RegName &= ~(~(word)(BitsVal) & (word)(BitsMask))),\
                                                                (RegName |= (word)(BitsVal) & (word)(BitsMask)) )
#define SeqSetClrReg16Bits(RegName, BitsMask, BitsVal)          ((RegName |= (word)(BitsVal) & (word)(BitsMask)),\
                                                                (RegName &= ~(~(word)(BitsVal) & (word)(BitsMask))) )
#define SeqResetSetReg16Bits(RegName, BitsMask, BitsVal)        ((RegName &= ~(word)(BitsMask)),\
                                                                (RegName |= (word)(BitsVal) & (word)(BitsMask)) )
#define ClrReg16BitsByOne(RegName, ClrMask, BitsMask)           (RegName &= (word)(ClrMask) & (word)(BitsMask))

/* Bit group peripheral register access macros */
#define TestReg16BitGroup(RegName, GroupName)                   (RegName & RegName##_##GroupName##_##MASK)
#define GetReg16BitGroupVal(RegName, GroupName)                 ((RegName & RegName##_##GroupName##_##MASK) >> RegName##_##GroupName##_##BITNUM)
#define SetReg16BitGroupVal(RegName, GroupName, GroupVal)       (RegName = (RegName & ~(word)RegName##_##GroupName##_##MASK) | (((word)GroupVal) << RegName##_##GroupName##_##BITNUM))
#define SeqClrSetReg16BitGroupVal(RegName,GroupName,GroupVal)   ((RegName &= ~(~(((word)GroupVal) << RegName##_##GroupName##_##BITNUM) & RegName##_##GroupName##_##MASK)),\
                                                                (RegName |= (((word)GroupVal) << RegName##_##GroupName##_##BITNUM) & RegName##_##GroupName##_##MASK) )
#define SeqSetClrReg16BitGroupVal(RegName,GroupName,GroupVal)   ((RegName |= (((word)GroupVal) << RegName##_##GroupName##_##BITNUM) & RegName##_##GroupName##_##MASK),\
                                                                (RegName &= ~(~(((word)GroupVal) << RegName##_##GroupName##_##BITNUM) & RegName##_##GroupName##_##MASK)) )
#define SeqResetSetReg16BitGroupVal(RegName,GroupName,GroupVal) ((RegName &= ~(word)RegName##_##GroupName##_##MASK),\
                                                                (RegName |= (((word)GroupVal) << RegName##_##GroupName##_##BITNUM) & RegName##_##GroupName##_##MASK) )

/*****************************************************************/
/* Uniform multiplatform peripheral access macros - 8 bit access */
/*****************************************************************/
#define SetReg8Bit(RegName, BitName)                             (RegName |= RegName##_##BitName##_##MASK)
#define ClrReg8Bit(RegName, BitName)                             (RegName &= (byte)~(byte)RegName##_##BitName##_##MASK)
#define InvertReg8Bit(RegName, BitName)                          (RegName ^= RegName##_##BitName##_##MASK)
#define TestReg8Bit(RegName, BitName)                            (RegName & RegName##_##BitName##_##MASK)

/* Whole peripheral register access macros */
#define SetReg8(RegName, val)                                    (RegName = (byte)(val))
#define GetReg8(RegName)                                         (RegName)

/* Bits peripheral register access macros */
#define TestReg8Bits(RegName, GetMask)                           (RegName & (GetMask))
#define ClrReg8Bits(RegName, ClrMask)                            (RegName &= (byte)(~(byte)(ClrMask)))
#define SetReg8Bits(RegName, SetMask)                            (RegName |= (byte)(SetMask))
#define InvertReg8Bits(RegName, InvMask)                         (RegName ^= (byte)(InvMask))
#define ClrSetReg8Bits(RegName, ClrMask, SetMask)                (RegName = (RegName & ((byte)(~(byte)(ClrMask)))) | (byte)(SetMask))
#define SeqClrSetReg8Bits(RegName, BitsMask, BitsVal)            ((RegName &= (byte)~((byte)~((byte)BitsVal) & ((byte)BitsMask))),\
                                                                 (RegName |= ((byte)BitsVal) & ((byte)BitsMask)) )
#define SeqSetClrReg8Bits(RegName, BitsMask, BitsVal)            ((RegName |= (byte)(BitsVal) & (byte)(BitsMask)),\
                                                                 (RegName &= (byte)~((byte)~((byte)BitsVal) & (byte)(BitsMask))) )
#define SeqResetSetReg8Bits(RegName, BitsMask, BitsVal)          ((RegName &= (byte)~((byte)BitsMask)),\
                                                                 (RegName |= (byte)(BitsVal) & (byte)(BitsMask)) )
#define ClrReg8BitsByOne(RegName, ClrMask, BitsMask)             (RegName &= (byte)(ClrMask) & (byte)(BitsMask))

/* Bit group peripheral register access macros */
#define TestReg8BitGroup(RegName, GroupName)                     (RegName & RegName##_##GroupName##_##MASK)
#define GetReg8BitGroupVal(RegName, GroupName)                   ((RegName & RegName##_##GroupName##_##MASK) >> RegName##_##GroupName##_##BITNUM)
#define SetReg8BitGroupVal(RegName, GroupName, GroupVal)         (RegName = (RegName & (byte)~(byte)RegName##_##GroupName##_##MASK) | (byte)(((byte)GroupVal) << RegName##_##GroupName##_##BITNUM))
#define SeqClrSetReg8BitGroupVal(RegName,GroupName,GroupVal)     ((RegName &= (byte)~((byte)~(byte)(((byte)GroupVal) << RegName##_##GroupName##_##BITNUM) & (byte)RegName##_##GroupName##_##MASK)),\
                                                                 (RegName |= (byte)(((byte)GroupVal) << RegName##_##GroupName##_##BITNUM) & (byte)RegName##_##GroupName##_##MASK) )
#define SeqSetClrReg8BitGroupVal(RegName,GroupName,GroupVal)     ((RegName |= (byte)(((byte)GroupVal) << RegName##_##GroupName##_##BITNUM) & (byte)RegName##_##GroupName##_##MASK),\
                                                                 (RegName &= (byte)~((byte)~(byte)(((byte)GroupVal) << RegName##_##GroupName##_##BITNUM) & (byte)RegName##_##GroupName##_##MASK)) )
#define SeqResetSetReg8BitGroupVal(RegName,GroupName,GroupVal)   ((RegName &= (byte)~(byte)RegName##_##GroupName##_##MASK),\
                                                                 (RegName |= (byte)(((byte)GroupVal) << RegName##_##GroupName##_##BITNUM) & RegName##_##GroupName##_##MASK) )
#define TESTBIT( operand, bit_mask )        (((operand) &  (bit_mask)) != ((bit_mask) - (bit_mask)))
#define SETBIT( operand, bit_mask )         ((operand) |= (bit_mask))
#define CLEARBIT( operand, bit_mask )       ((operand) &= ((u32)~(bit_mask)))

#endif/*TYPES_H*/


/******************************************************************************
**                  R E V I S I O N     H I S T O R Y  
*******************************************************************************
** Please update in the following format:
**
** Revision No :     Date:              Author:
** Description of change:Initial Release
**
**
*******************************************************************************/


