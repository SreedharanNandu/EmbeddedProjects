/******************************************************************************
* File Name : adc.c
* Description : ADC driver for LPC1114.
*               Configures AD0 (channel 4, PIO0_11) for the 4-switch
*               resistor ladder on the button input.
*               Single-shot, software-triggered, polled (no interrupt).
******************************************************************************/
#include "LPC11xx.h"
#include "adc.h"

/*-----------------------------------------------------------------------------
                            PRIVATE DEFINES
  ---------------------------------------------------------------------------*/

/* SYSCON: ADC power and clock control */
#define SYSCON_PDRUNCFG_ADC_BIT        (1u << 4)    /* 0 = powered up       */
#define SYSCON_SYSAHBCLKCTRL_ADC_BIT   (1u << 13)   /* 1 = clock enabled    */

/* IOCON: PIO0_11 function select
   Bits[2:0] = 010 -> AD0 analogue input; bit 7 (ADMODE) = 0 -> analogue    */
#define IOCON_PIO0_11_FUNC_AD0         0x02u

/* ADC CR register bit fields */
#define ADC_CR_CLKDIV_SHIFT            8u
#define ADC_CR_BURST_BIT               (1u << 16)
#define ADC_CR_CLKS_SHIFT              17u
#define ADC_CR_PDN_BIT                 (1u << 21)
#define ADC_CR_START_SHIFT             24u
#define ADC_CR_START_NOW               (1u << ADC_CR_START_SHIFT)  /* 001   */
#define ADC_CR_START_MASK              (7u << ADC_CR_START_SHIFT)

/* ADC result extraction from GDR / DRx */
#define ADC_RESULT_MASK                0x0000FFC0u  /* bits [15:6]           */
#define ADC_RESULT_SHIFT               6u
#define ADC_DONE_BIT                   (1u << 31)

/* ADC clock divider.
   ADC clock = AHB_CLK / (CLKDIV + 1) must not exceed 4.5 MHz.
   With a 48 MHz system clock: 48 / (10+1) = 4.36 MHz  -> OK
   With a 72 MHz system clock: 72 / (16+1) = 4.24 MHz  -> OK
   Adjust ADC_CLKDIV to suit your SystemCoreClock if different.             */
#define ADC_CLKDIV                     10u

/*-----------------------------------------------------------------------------
                            FUNCTION DEFINITIONS
  ---------------------------------------------------------------------------*/

/*****************************************************************************
** Function name : ADC_Init
** Descriptions  : One-time initialisation of the ADC peripheral.
**                 Configures PIO0_11 as AD0 (channel 4) analogue input for
**                 the button resistor ladder.  Call once from hardware init
**                 before the first ADC_Read().
** Parameters    : None
** Returned value: None
*****************************************************************************/
void ADC_Init(void)
{
   /* 1. Power up the ADC (clear power-down bit in PDRUNCFG)                */
   LPC_SYSCON->PDRUNCFG &= ~SYSCON_PDRUNCFG_ADC_BIT;

   /* 2. Enable AHB clock to the ADC block                                  */
   LPC_SYSCON->SYSAHBCLKCTRL |= SYSCON_SYSAHBCLKCTRL_ADC_BIT;

   /* 3. Set PIO0_11 (JTAG_TDI) to analogue AD0 function via IOCON         */
   LPC_IOCON->JTAG_TDI_PIO0_11 = IOCON_PIO0_11_FUNC_AD0;

   /* 4. Configure ADC CR:
         - Channel 0 selected (SEL bit 0, AD0 = PIO0_11)
         - Clock divider
         - No burst (software triggered)
         - 11 clocks / 10-bit resolution (CLKS = 000)
         - Power on (PDN = 1)
         - START = 000 (no conversion yet)                                  */
   LPC_ADC->CR = (1u               << ADC_BTN_CHANNEL)    |   /* SEL bit 0   */
                 (ADC_CLKDIV       << ADC_CR_CLKDIV_SHIFT)|   /* CLKDIV      */
                 (0u               << 16u)                |   /* BURST off   */
                 (0u               << ADC_CR_CLKS_SHIFT)  |   /* 10-bit      */
                 ADC_CR_PDN_BIT;                              /* power on    */
}

/*****************************************************************************
** Function name : ADC_Read
** Descriptions  : Performs a single software-triggered conversion on the
**                 requested ADC channel and returns the 10-bit result.
**                 Blocking poll - completes in ~2.5 us at 4.4 MHz ADC clock,
**                 well within the 10 ms button-scan period.
** Parameters    : channel - ADC channel number (0..7). Must match a channel
**                           whose pin has been configured in ADC_Init().
** Returned value: 10-bit ADC result (0 = 0 V, 1023 = VDD)
*****************************************************************************/
unsigned short ADC_Read(unsigned char channel)
{
   unsigned long  gdr;
   unsigned short result;

   /* Update SEL to the requested channel, keep all other CR fields intact  */
   LPC_ADC->CR = (LPC_ADC->CR & ~0xFFu) | (1u << channel);

   /* Trigger a conversion (START = 001)                                    */
   LPC_ADC->CR |= ADC_CR_START_NOW;

   /* Poll the Global Data Register until DONE is asserted                  */
   do
   {
      gdr = LPC_ADC->GDR;
   }
   while ((gdr & ADC_DONE_BIT) == 0u);

   /* Clear START bits so conversion does not re-trigger                    */
   LPC_ADC->CR &= ~ADC_CR_START_MASK;

   /* Extract the 10-bit result from bits [15:6]                            */
   result = (unsigned short)((gdr & ADC_RESULT_MASK) >> ADC_RESULT_SHIFT);

   return result;
}
