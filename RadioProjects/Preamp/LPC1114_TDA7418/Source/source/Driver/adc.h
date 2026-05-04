#ifndef ADC_H
#define ADC_H
/*filename:-adc.h*/

/*-----------------------------------------------------------------------------
 * ADC Driver - LPC1114
 *
 * Provides single-shot software-triggered reads on any ADC channel.
 * Pin/channel mapping (LPC1114FN28) - ADx name == channel number:
 *   AD0 = PIO0_11  (JTAG_TDI)   channel 0   <- button ladder uses this
 *   AD1 = PIO1_0   (JTAG_TMS)   channel 1
 *   AD2 = PIO1_1   (JTAG_TDO)   channel 2
 *   AD3 = PIO1_2   (JTAG_nTRST) channel 3
 *   AD4 = PIO1_3   (ARM_SWDIO)  channel 4
 *   AD5 = PIO1_4                channel 5
 *   AD6 = PIO1_10               channel 6
 *   AD7 = PIO1_11               channel 7
 *
 * Currently only AD0 (channel 0, PIO0_11) is configured for the button
 * resistor ladder. Extend ADC_Init() if other channels are needed.
 ---------------------------------------------------------------------------*/

#include "type_sys.h"

/* ADC channel number used for the button resistor ladder */
#define ADC_BTN_CHANNEL    0u   /* AD0 = PIO0_11 = channel 0 */

void           ADC_Init(void);
unsigned short ADC_Read(unsigned char channel);

#endif /* ADC_H */
