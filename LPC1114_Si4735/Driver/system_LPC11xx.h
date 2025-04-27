/******************************************************************************
 * @file:    system_LPC11xx.h
 * @purpose: CMSIS Cortex-M0 Device Peripheral Access Layer Header File
 *           for the NXP LPC11xx Device Series 
 ******************************************************************************/


#ifndef __SYSTEM_LPC11xx_H
#define __SYSTEM_LPC11xx_H

/* Vector Table Base ---------------------------------------------------------*/
#define NVIC_VectTab_RAM             (0x10000000)
#define NVIC_VectTab_FLASH           (0x00000000)

extern uint32_t ClockSource;
extern uint32_t SystemFrequency;    /*!< System Clock Frequency (Core Clock)  */
extern uint32_t SystemAHBFrequency;

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemFrequency variable.
 */
extern void SystemInit (void);
#endif
