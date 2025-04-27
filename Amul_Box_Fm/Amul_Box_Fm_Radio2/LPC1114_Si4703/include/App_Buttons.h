#ifndef __APP_BUTTONS_H__
#define __APP_BUTTONS_H__

#include <stdio.h>

#define SHORT_PRESS   1u
#define LONG_PRESS    2u

#define BREAK_TIMEOUT           (1000u/10u) /*10msec sample*/
#define BREAK_TIMEOUT1          (1000u/10u) /*10msec sample*/


extern void Scan_Buttons(void);


#endif  /* __APP_BUTTONS_H__ */
