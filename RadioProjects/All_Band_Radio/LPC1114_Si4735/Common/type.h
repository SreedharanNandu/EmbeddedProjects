/*****************************************************************************
 *   type.h:  Type definition Header file for NXP Family 
******************************************************************************/
#ifndef TYPE_H
#define TYPE_H

/* exact-width signed integer types */
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed       __int64 int64_t;

    /* exact-width unsigned integer types */
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       __int64 uint64_t;

typedef   unsigned          char u8;
typedef   unsigned short     int u16;
typedef   unsigned           int u32;

typedef   unsigned char           bool;

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif


#define ON (1u)
#define OFF (0u)

#endif  /* __TYPE_H__ */
