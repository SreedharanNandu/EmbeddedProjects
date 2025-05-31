/*********************************************************************/
/*              Header for IO ports module library functions         */
/*********************************************************************/

#ifndef __PORTS_H
#define __PORTS_H

/* List of SFRs for IO ports */
/* This list contains the SFRs with default (POR) values to be used for configuring IOports */
/* The user can modify this based on the requirement */

#define CBI(x) (x=0)
#define SBI(x) (x=1)

/*#######################*/
    #define LATA0               LATAbits.LATA0
    #define LATA1               LATAbits.LATA1
    #define LATA2               LATAbits.LATA2
    #define LATA3               LATAbits.LATA3
    #define LATA4               LATAbits.LATA4
    #define LATA5               LATAbits.LATA5
    #define LATA6               LATAbits.LATA6
    #define LATA7               LATAbits.LATA7

    #define RB0                 PORTBbits.RB0
    #define RB1                 PORTBbits.RB1
    #define RB2                 PORTBbits.RB2
    #define RB3                 PORTBbits.RB3
    #define RB4                 PORTBbits.RB4
    #define RB5                 PORTBbits.RB5
    #define RB6                 PORTBbits.RB6
    #define RB7                 PORTBbits.RB7

    #define TRISB0              TRISBbits.TRISB0
    #define TRISB1              TRISBbits.TRISB1
    #define TRISB2              TRISBbits.TRISB2
    #define TRISB3              TRISBbits.TRISB3
    #define TRISB4              TRISBbits.TRISB4
    #define TRISB5              TRISBbits.TRISB5
    #define TRISB6              TRISBbits.TRISB6
    #define TRISB7              TRISBbits.TRISB7

    #define RA0                 PORTAbits.RA0
    #define RA1                 PORTAbits.RA1
    #define RA2                 PORTAbits.RA2
    #define RA3                 PORTAbits.RA3
    #define RA4                 PORTAbits.RA4
    #define RA5                 PORTAbits.RA5
    #define RA6                 PORTAbits.RA6
    #define RA7                 PORTAbits.RA7

    #define TRISA0              TRISAbits.TRISA0
    #define TRISA1              TRISAbits.TRISA1
    #define TRISA2              TRISAbits.TRISA2
    #define TRISA3              TRISAbits.TRISA3
    #define TRISA4              TRISAbits.TRISA4
    #define TRISA5              TRISAbits.TRISA5
    #define TRISA6              TRISAbits.TRISA6
    #define TRISA7              TRISAbits.TRISA7

    #define TRISC0              TRISCbits.TRISC0
    #define TRISC1              TRISCbits.TRISC1
    #define TRISC2              TRISCbits.TRISC2
    #define TRISC3              TRISCbits.TRISC3
    #define TRISC4              TRISCbits.TRISC4
    #define TRISC5              TRISCbits.TRISC5
    #define TRISC6              TRISCbits.TRISC6
    #define TRISC7              TRISCbits.TRISC7
    #define TRISC13             TRISCbits.TRISC13
    #define TRISC14             TRISCbits.TRISC14

    #define RC0                 PORTCbits.RC0
    #define RC1                 PORTCbits.RC1
    #define RC2                 PORTCbits.RC2
    #define RC3                 PORTCbits.RC3
    #define RC4                 PORTCbits.RC4
    #define RC5                 PORTCbits.RC5
    #define RC6                 PORTCbits.RC6
    #define RC7                 PORTCbits.RC7
    #define RC13                PORTCbits.RC13
    #define RC14                PORTCbits.RC14

    #define RF3                PORTFbits.RF3
    #define RF2                PORTFbits.RF2

    #define TRISF2              TRISFbits.TRISF2
    #define TRISF3              TRISFbits.TRISF3



    #define TRISD7              TRISDbits.TRISD7
    #define TRISD6              TRISDbits.TRISD6
    #define TRISD5              TRISDbits.TRISD5
    #define TRISD4              TRISDbits.TRISD4
    #define TRISD3              TRISDbits.TRISD3
    #define TRISD2              TRISDbits.TRISD2
    #define TRISD1              TRISDbits.TRISD1
    #define TRISD0              TRISDbits.TRISD0

    #define TRISE8              TRISEbits.TRISE8
    #define TRISE7              TRISEbits.TRISE7
    #define TRISE6              TRISEbits.TRISE6
    #define TRISE5              TRISEbits.TRISE5
    #define TRISE4              TRISEbits.TRISE4
    #define TRISE3              TRISEbits.TRISE3
    #define TRISE2              TRISEbits.TRISE2
    #define TRISE1              TRISEbits.TRISE1
    #define TRISE0              TRISEbits.TRISE0

    #define LATD0               LATDbits.LATD0
    #define LATD1               LATDbits.LATD1
    #define LATD2               LATDbits.LATD2
    #define LATD3               LATDbits.LATD3
    #define LATD4               LATDbits.LATD4
    #define LATD5               LATDbits.LATD5
    #define LATD6               LATDbits.LATD6
    #define LATD7               LATDbits.LATD7

    #define LATC0               LATCbits.LATC0
    #define LATC1               LATCbits.LATC1
    #define LATC2               LATCbits.LATC2
    #define LATC3               LATCbits.LATC3
    #define LATC4               LATCbits.LATC4
    #define LATC5               LATCbits.LATC5
    #define LATC6               LATCbits.LATC6
    #define LATC7               LATCbits.LATC7
    #define LATC13              LATCbits.LATC13
    #define LATC14              LATCbits.LATC14


    #define LATE0               LATEbits.LATE0
    #define LATE1               LATEbits.LATE1
    #define LATE2               LATEbits.LATE2
    #define LATE3               LATEbits.LATE3
    #define LATE4               LATEbits.LATE4
    #define LATE5               LATEbits.LATE5
    #define LATE6               LATEbits.LATE6
    #define LATE7               LATEbits.LATE7
    #define LATE8               LATEbits.LATE8

    #define RD7                 PORTDbits.RD7
    #define RD6                 PORTDbits.RD6
    #define RD5                 PORTDbits.RD5
    #define RD4                 PORTDbits.RD4
    #define RD3                 PORTDbits.RD3
    #define RD2                 PORTDbits.RD2
    #define RD1                 PORTDbits.RD1
    #define RD0                 PORTDbits.RD0

    #define RE0                 PORTEbits.RE0
    #define RE1                 PORTEbits.RE1
    #define RE2                 PORTEbits.RE2
    #define RE3                 PORTEbits.RE3
    #define RE4                 PORTEbits.RE4
    #define RE5                 PORTEbits.RE5
    #define RE6                 PORTEbits.RE6
    #define RE7                 PORTEbits.RE7
    #define RE8                 PORTEbits.RE8




/*$$$$$$$$$$$$$$$$$$$$$$$*/


#define CNEN1_VALUE             0x0000
#define CNPU1_VALUE             0x0000

#define CNEN2_VALUE             0x0000
#define CNPU2_VALUE             0x0000

#define IEC0_VALUE              0x0000
#define IEC1_VALUE              0x0000
#define IEC2_VALUE              0x0000

#define IFS0_VALUE              0x0000
#define IFS1_VALUE              0x0000
#define IFS2_VALUE              0x0000

#define IPC0_VALUE              0x4444
#define IPC1_VALUE              0x4444
#define IPC2_VALUE              0x4444
#define IPC3_VALUE              0x4444
#define IPC4_VALUE              0x4444
#define IPC5_VALUE              0x4444
#define IPC6_VALUE              0x4444
#define IPC7_VALUE              0x4444
#define IPC8_VALUE              0x4444
#define IPC9_VALUE              0x4444
#define IPC10_VALUE             0x4444
#define IPC11_VALUE             0x4444

/* Config Bit Defines to be used for CN interrupt configuration */
#define CHANGE_INT_ON           0xFFFFFFFF    /* interrupts on  */
#define CHANGE_INT_OFF          0xF7FFFFFF    /* interrupts off */

/* Setting the priority of CN interrupt */
#define CHANGE_INT_PRI_0        0xF8FFFFFF
#define CHANGE_INT_PRI_1        0xF9FFFFFF
#define CHANGE_INT_PRI_2        0xFAFFFFFF
#define CHANGE_INT_PRI_3        0xFBFFFFFF
#define CHANGE_INT_PRI_4        0xFCFFFFFF
#define CHANGE_INT_PRI_5        0xFDFFFFFF
#define CHANGE_INT_PRI_6        0xFEFFFFFF
#define CHANGE_INT_PRI_7        0xFFFFFFFF


/* Setting the priority of External interrupt */
#define EXT_INT_PRI_0           0xFFF8
#define EXT_INT_PRI_1           0xFFF9
#define EXT_INT_PRI_2           0xFFFA
#define EXT_INT_PRI_3           0xFFFB
#define EXT_INT_PRI_4           0xFFFC
#define EXT_INT_PRI_5           0xFFFD
#define EXT_INT_PRI_6           0xFFFE
#define EXT_INT_PRI_7           0xFFFF

/* enable / disable External interrupt */
#define EXT_INT_ENABLE          0xFFFF
#define EXT_INT_DISABLE         0xFFF7

/* External interrupts edge selection defines */
#define RISING_EDGE_INT         0xFFEF    /*Interrupt is set by a 
                                                    rising edge  */
#define FALLING_EDGE_INT        0xFFFF    /*Interrupt is set by a
                                                     falling edge */

/* Macros to Enable CN interrupts */
#define EnableCN0               asm ("BSET CNEN1,#0")
#define EnableCN1               asm ("BSET CNEN1,#1")
#define EnableCN2               asm ("BSET CNEN1,#2")
#define EnableCN3               asm ("BSET CNEN1,#3")
#define EnableCN4               asm ("BSET CNEN1,#4")
#define EnableCN5               asm ("BSET CNEN1,#5")
#define EnableCN6               asm ("BSET CNEN1,#6")
#define EnableCN7               asm ("BSET CNEN1,#7")

#if defined(__dsPIC30F5011__) || defined(__dsPIC30F6011__) || defined(__dsPIC30F6012__) || \
    defined(__dsPIC30F6010__) || defined(__dsPIC30F5013__) || defined(__dsPIC30F6013__) || \
    defined(__dsPIC30F6014__) || defined(__dsPIC30F5015__) || defined(__dsPIC30F6010A__) || \
    defined(__dsPIC30F6011A__) || defined(__dsPIC30F6012A__) || defined(__dsPIC30F6013A__) || \
    defined(__dsPIC30F6014A__) || defined(__dsPIC30F5016__) || defined(__dsPIC30F6015__)
#define EnableCN8               asm ("BSET CNEN1,#8")
#define EnableCN9               asm ("BSET CNEN1,#9")
#define EnableCN10              asm ("BSET CNEN1,#10")
#define EnableCN11              asm ("BSET CNEN1,#11")
#define EnableCN12              asm ("BSET CNEN1,#12")
#define EnableCN13              asm ("BSET CNEN1,#13")
#define EnableCN14              asm ("BSET CNEN1,#14")
#define EnableCN15              asm ("BSET CNEN1,#15")
#define EnableCN16              asm ("BSET CNEN2,#0")
#define EnableCN17              asm ("BSET CNEN2,#1")
#define EnableCN18              asm ("BSET CNEN2,#2")
#endif

#if defined(__dsPIC30F6010__) || defined(__dsPIC30F5013__) || defined(__dsPIC30F6013__) || \
    defined(__dsPIC30F6014__) || defined(__dsPIC30F6010A__) || defined(__dsPIC30F6013A__) || \
    defined(__dsPIC30F6014A__) || defined(__dsPIC30F5016__)
#define EnableCN19              asm ("BSET CNEN2,#3")
#define EnableCN20              asm ("BSET CNEN2,#4")
#define EnableCN21              asm ("BSET CNEN2,#5")
#endif

#if defined(__dsPIC30F5013__) || defined(__dsPIC30F6013__) || defined(__dsPIC30F6014__) || \
    defined(__dsPIC30F6013A__) || defined(__dsPIC30F6014A__)
#define EnableCN22              asm ("BSET CNEN2,#6")
#define EnableCN23              asm ("BSET CNEN2,#7")
#endif

/* Macros to Disable CN interrupts */
#define DisableCN0              asm ("BCLR CNEN1,#0")
#define DisableCN1              asm ("BCLR CNEN1,#1")
#define DisableCN2              asm ("BCLR CNEN1,#2")
#define DisableCN3              asm ("BCLR CNEN1,#3")
#define DisableCN4              asm ("BCLR CNEN1,#4")
#define DisableCN5              asm ("BCLR CNEN1,#5")
#define DisableCN6              asm ("BCLR CNEN1,#6")
#define DisableCN7              asm ("BCLR CNEN1,#7")

#if defined(__dsPIC30F5011__) || defined(__dsPIC30F6011__) || defined(__dsPIC30F6012__) || \
    defined(__dsPIC30F6010__) || defined(__dsPIC30F5013__) || defined(__dsPIC30F6013__) || \
    defined(__dsPIC30F6014__) || defined(__dsPIC30F5015__) || defined(__dsPIC30F6010A__) || \
    defined(__dsPIC30F6011A__) || defined(__dsPIC30F6012A__) || defined(__dsPIC30F6013A__) || \
    defined(__dsPIC30F6014A__) || defined(__dsPIC30F5016__) || defined(__dsPIC30F6015__)
#define DisableCN8              asm ("BCLR CNEN1,#8")
#define DisableCN9              asm ("BCLR CNEN1,#9")
#define DisableCN10             asm ("BCLR CNEN1,#10")
#define DisableCN11             asm ("BCLR CNEN1,#11")
#define DisableCN12             asm ("BCLR CNEN1,#12")
#define DisableCN13             asm ("BCLR CNEN1,#13")
#define DisableCN14             asm ("BCLR CNEN1,#14")
#define DisableCN15             asm ("BCLR CNEN1,#15")
#define DisableCN16             asm ("BCLR CNEN2,#0")
#define DisableCN17             asm ("BCLR CNEN2,#1")
#define DisableCN18             asm ("BCLR CNEN2,#2")
#endif

#if defined(__dsPIC30F6010__) || defined(__dsPIC30F5013__) || defined(__dsPIC30F6013__) || \
    defined(__dsPIC30F6014__) || defined(__dsPIC30F6010A__) || defined(__dsPIC30F6013A__) || \
    defined(__dsPIC30F6014A__) || defined(__dsPIC30F5016__)
#define DisableCN19             asm ("BCLR CNEN2,#3")
#define DisableCN20             asm ("BCLR CNEN2,#4")
#define DisableCN21             asm ("BCLR CNEN2,#5")
#endif

#if defined(__dsPIC30F5013__) || defined(__dsPIC30F6013__) || defined(__dsPIC30F6014__) || \
    defined(__dsPIC30F6013A__) || defined(__dsPIC30F6014A__)
#define DisableCN22             asm ("BCLR CNEN2,#6")
#define DisableCN23             asm ("BCLR CNEN2,#7")
#endif

/* Macros to Enable External interrupts */
#define EnableINT0              asm ("BSET IEC0, #0")
#define EnableINT1              asm ("BSET IEC1, #0")
#define EnableINT2              asm ("BSET IEC1, #7")
#define EnableINT3              asm ("BSET IEC2, #4")
#define EnableINT4              asm ("BSET IEC2, #5")

/* Macros to Disable External interrupts */
#define DisableINT0             asm ("BCLR IEC0, #0")
#define DisableINT1             asm ("BCLR IEC1, #0")
#define DisableINT2             asm ("BCLR IEC1, #7")
#define DisableINT3             asm ("BCLR IEC2, #4")
#define DisableINT4             asm ("BCLR IEC2, #5")

/* Macros to set priority for External interrupts */
#define SetPriorityInt0(priority)     (IPC0bits.INT0IP = priority)
#define SetPriorityInt1(priority)     (IPC4bits.INT1IP = priority)
#define SetPriorityInt2(priority)     (IPC5bits.INT2IP = priority)
#define SetPriorityInt3(priority)     (IPC9bits.INT3IP = priority)
#define SetPriorityInt4(priority)     (IPC9bits.INT4IP = priority)


/* Function Prototypes */

/* ConfigCNPullups
 * Enable/ disable pull up registers
 */
void ConfigCNPullups(long int) __attribute__ ((section (".libperi")));

/* ConfigIntCN
 * Enable/Disable CN interrupt and set priority
 */
void ConfigIntCN(long int) __attribute__ ((section (".libperi")));

/* ConfigINT0
 * Enable/disable INT0 interrupts,set priority and falling edge/rising edge
 */
void ConfigINT0(unsigned int) __attribute__ ((section (".libperi")));

/* CloseINT0
 * Disable INT0 interrupts, registers
 */
void CloseINT0() __attribute__ ((section (".libperi")));

/* ConfigINT1
 * Enable/disable INT1 interrupt,set priority and falling edge/rising edge
 */
void ConfigINT1(unsigned int) __attribute__ ((section (".libperi")));

/* CloseINT1
 * Disable INT1 interrupts, registers
 */
void CloseINT1() __attribute__ ((section (".libperi")));

/* ConfigINT2
 * Enable/disable INT2 interrupt,set priority and falling edge/rising edge
 */
void ConfigINT2(unsigned int) __attribute__ ((section (".libperi")));

/* CloseINT2
 * Disable INT2 interrupts, registers
 */
void CloseINT2() __attribute__ ((section (".libperi")));

#if defined(__dsPIC30F6010__) || defined(__dsPIC30F5011__) || defined(__dsPIC30F6011__)|| \
    defined(__dsPIC30F6012__) || defined(__dsPIC30F5013__) || defined(__dsPIC30F6013__)|| \
    defined(__dsPIC30F6014__) || defined(__dsPIC30F6010A__) || defined(__dsPIC30F6011A__) || \
    defined(__dsPIC30F6012A__) || defined(__dsPIC30F6013A__) || defined(__dsPIC30F6014A__) || \
    defined(__dsPIC30F5016__) || defined(__dsPIC30F6015__)

/* ConfigINT3
 * Enable/disable INT3 interrupt,set priority and falling edge/rising edge
 */
void ConfigINT3(unsigned int) __attribute__ ((section (".libperi")));

/* CloseINT3
 * Disable INT3 interrupts, registers
 */
void CloseINT3() __attribute__ ((section (".libperi")));

/* ConfigINT4
 * Enable/disable INT4 interrupt,set priority and falling edge/rising edge
 */
void ConfigINT4(unsigned int) __attribute__ ((section (".libperi")));

/* CloseINT4
 * Disable INT4 interrupts, registers
 */
void CloseINT4() __attribute__ ((section (".libperi")));

#endif

#endif
