 /*********************************************************************
 *                  Generic Typedef's
 *********************************************************************
 * FileName:        Generic.h
 * Dependencies:
 * Processor:       PIC24F
 *
 * Complier:        MPLAB C30 v1.31.00 or higher
 *                  MPLAB IDE v7.22.00 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ********************************************************************/

#ifndef __GENERIC_TYPE_DEFS_H_
#define __GENERIC_TYPE_DEFS_H_

/* By default, configuration constants are assembled from macros */
/* using the bit-wise AND operator (&). An alternate mode is     */
/* available in which constants are assembled using the bit-wise */
/* OR operator (|) to set values and bit-wise AND (&) masks to   */
/* clear values. To enable the alternate mode, define the        */
/* USE_AND_OR symbol.                                            */
/*                                                               */
/* To define this macro as a compiler option:                    */
/*                                                               */
/* -DUSE_AND_OR                                                  */
/*                                                               */
/* To define this macro in source code:                          */
/*                                                               */
/* #define USE_AND_OR                                            */

#define PUBLIC
#define PROTECTED
#define PRIVATE                 static

#ifndef _WINDOWS
typedef unsigned char      BYTE;            // 8-bit unsigned
typedef unsigned short int   WORD;            // 16-bit unsigned
typedef unsigned long      DWORD;            // 32-bit unsigned
typedef unsigned long long   QWORD;            // 64-bit unsigned
typedef signed char         CHAR;            // 8-bit signed
typedef signed short int   SHORT;            // 16-bit signed
typedef signed long         LONG;            // 32-bit signed
typedef signed long long   LONGLONG;         // 64-bit signed

/* unsigned integer */
typedef unsigned int        UINT;
typedef unsigned char       UINT8;              // other name for 8-bit integer
typedef unsigned short      UINT16;             // other name for 16-bit integer
typedef unsigned long       UINT32;             // other name for 32-bit integer

typedef enum _BOOL { FALSE = 0, TRUE } BOOL;   // Undefined size
typedef enum _bool { false = 0, true } bool;

//#define false   0
//#define true    1
//#define INVALID 0XFF


#endif

typedef struct
{
    BYTE    b0:     1;
    BYTE    b1:     1;
    BYTE    b2:     1;
    BYTE    b3:     1;
    BYTE    b4:     1;
    BYTE    b5:     1;
    BYTE    b6:     1;
    BYTE    b7:     1;

}BYTE_BITS;
typedef struct
{
    WORD    b0:     1;
    WORD    b1:     1;
    WORD    b2:     1;
    WORD    b3:     1;
    WORD    b4:     1;
    WORD    b5:     1;
    WORD    b6:     1;
    WORD    b7:     1;
    WORD    b8:     1;
    WORD    b9:     1;
    WORD    b10:    1;
    WORD    b11:    1;
    WORD    b12:    1;
    WORD    b13:    1;
    WORD    b14:    1;
    WORD    b15:    1;
}WORD_BITS;

typedef union _BYTE_VAL
{
    BYTE Val;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
    } bits;
} BYTE_VAL;

typedef union _CHAR_VAL
{
    CHAR Val;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
    } bits;
} CHAR_VAL;

typedef union _WORD_VAL
{
    WORD Val;
    BYTE v[2];
    struct
    {
        BYTE LB;
        BYTE HB;
    } byte;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
        unsigned char b8:1;
        unsigned char b9:1;
        unsigned char b10:1;
        unsigned char b11:1;
        unsigned char b12:1;
        unsigned char b13:1;
        unsigned char b14:1;
        unsigned char b15:1;
    } bits;
} WORD_VAL;

typedef union _DWORD_VAL
{
    DWORD Val;
   WORD w[2];
    BYTE v[4];
    struct
    {
        WORD LW;
        WORD HW;
    } word;
    struct
    {
        BYTE LB;
        BYTE HB;
        BYTE UB;
        BYTE MB;
    } byte;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
        unsigned char b8:1;
        unsigned char b9:1;
        unsigned char b10:1;
        unsigned char b11:1;
        unsigned char b12:1;
        unsigned char b13:1;
        unsigned char b14:1;
        unsigned char b15:1;
        unsigned char b16:1;
        unsigned char b17:1;
        unsigned char b18:1;
        unsigned char b19:1;
        unsigned char b20:1;
        unsigned char b21:1;
        unsigned char b22:1;
        unsigned char b23:1;
        unsigned char b24:1;
        unsigned char b25:1;
        unsigned char b26:1;
        unsigned char b27:1;
        unsigned char b28:1;
        unsigned char b29:1;
        unsigned char b30:1;
        unsigned char b31:1;
    } bits;
} DWORD_VAL;


typedef union _SHORT_VAL
{
    SHORT Val;
    BYTE  v[2];
    struct
    {
        BYTE LB;
        BYTE HB;
    } byte;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
        unsigned char b8:1;
        unsigned char b9:1;
        unsigned char b10:1;
        unsigned char b11:1;
        unsigned char b12:1;
        unsigned char b13:1;
        unsigned char b14:1;
        unsigned char b15:1;
    } bits;
} SHORT_VAL;


typedef union _LONG_VAL
{
    LONG Val;
   WORD w[2];
    BYTE v[4];
    struct
    {
        WORD LW;
        WORD HW;
    } word;
    struct
    {
        BYTE LB;
        BYTE HB;
        BYTE UB;
        BYTE MB;
    } byte;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
        unsigned char b8:1;
        unsigned char b9:1;
        unsigned char b10:1;
        unsigned char b11:1;
        unsigned char b12:1;
        unsigned char b13:1;
        unsigned char b14:1;
        unsigned char b15:1;
        unsigned char b16:1;
        unsigned char b17:1;
        unsigned char b18:1;
        unsigned char b19:1;
        unsigned char b20:1;
        unsigned char b21:1;
        unsigned char b22:1;
        unsigned char b23:1;
        unsigned char b24:1;
        unsigned char b25:1;
        unsigned char b26:1;
        unsigned char b27:1;
        unsigned char b28:1;
        unsigned char b29:1;
        unsigned char b30:1;
        unsigned char b31:1;
    } bits;
} LONG_VAL;

typedef union _QWORD_VAL
{
    QWORD Val;
   DWORD d[2];
   WORD w[4];
    BYTE v[8];
    struct
    {
        DWORD LD;
        DWORD HD;
    } dword;
    struct
    {
        WORD LW;
        WORD HW;
        WORD UW;
        WORD MW;
    } word;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
        unsigned char b8:1;
        unsigned char b9:1;
        unsigned char b10:1;
        unsigned char b11:1;
        unsigned char b12:1;
        unsigned char b13:1;
        unsigned char b14:1;
        unsigned char b15:1;
        unsigned char b16:1;
        unsigned char b17:1;
        unsigned char b18:1;
        unsigned char b19:1;
        unsigned char b20:1;
        unsigned char b21:1;
        unsigned char b22:1;
        unsigned char b23:1;
        unsigned char b24:1;
        unsigned char b25:1;
        unsigned char b26:1;
        unsigned char b27:1;
        unsigned char b28:1;
        unsigned char b29:1;
        unsigned char b30:1;
        unsigned char b31:1;
        unsigned char b32:1;
        unsigned char b33:1;
        unsigned char b34:1;
        unsigned char b35:1;
        unsigned char b36:1;
        unsigned char b37:1;
        unsigned char b38:1;
        unsigned char b39:1;
        unsigned char b40:1;
        unsigned char b41:1;
        unsigned char b42:1;
        unsigned char b43:1;
        unsigned char b44:1;
        unsigned char b45:1;
        unsigned char b46:1;
        unsigned char b47:1;
        unsigned char b48:1;
        unsigned char b49:1;
        unsigned char b50:1;
        unsigned char b51:1;
        unsigned char b52:1;
        unsigned char b53:1;
        unsigned char b54:1;
        unsigned char b55:1;
        unsigned char b56:1;
        unsigned char b57:1;
        unsigned char b58:1;
        unsigned char b59:1;
        unsigned char b60:1;
        unsigned char b61:1;
        unsigned char b62:1;
        unsigned char b63:1;
    } bits;
} QWORD_VAL;

#define LSB(a)          ((a).v[0])
#define MSB(a)          ((a).v[1])

#define LOWER_LSB(a)    ((a).v[0])
#define LOWER_MSB(a)    ((a).v[1])
#define UPPER_LSB(a)    ((a).v[2])
#define UPPER_MSB(a)    ((a).v[3])

#define    BIT0         0X0001
#define      BIT1         0X0002
#define    BIT2         0X0004
#define    BIT3         0X0008
#define   BIT4         0X0010
#define    BIT5         0X0020
#define   BIT6         0X0040
#define   BIT7         0X0080  
#define   BIT8         0X0100
#define   BIT9         0X0200
#define   BIT10         0X0400
#define   BIT11     0X0800
#define   BIT12     0X1000
#define   BIT13     0X2000
#define   BIT14         0X4000
#define   BIT15     0X8000  

/* The following macros can be used to modify the current CPU */
/* IPL. The definition of the macro may vary from device to   */
/* device.                                                    */
/*                                                            */
/* To safely set the CPU IPL, use mSET_CPU_IP(ipl); the       */
/* valid range of ipl is 0-7, it may be any expression.       */
/*                                                            */
/* mSET_CPU_IP(7);                                            */
/*                                                            */
/* To preserve the current IPL and save it use                */
/* mSET_AND_SAVE_CPU_IP(save_to, ipl); the valid range of ipl */
/* is 0-7 and may be any expression, save_to should denote    */
/* some temporary storage.                                    */
/*                                                            */
/* int old_ipl;                                               */
/*                                                            */
/* mSET_AND_SAVE_CPU_IP(old_ipl, 7);                          */
/*                                                            */
/* The IPL can be restored with mRESTORE_CPU_IP(saved_to)     */
/*                                                            */
/* mRESTORE_CPU_IP(old_ipl);                                  */

#define mSET_CPU_IP(ipl)  SET_CPU_IPL(ipl)

#define mSET_AND_SAVE_CPU_IP(save_to, ipl) SET_AND_SAVE_CPU_IPL(save_to, ipl)

#define mRESTORE_CPU_IP(saved_to) mSET_CPU_IP(saved_to)

#endif
