#pragma section @@DATA FBL_NOIN

/* BootFlag.c : the same file, at the same address, in the boot AND in the application project  */
#include "system_def.h"

/* Fixed-address RAM that the start-up code must not clear. Check the exact pragma syntax in    */
/* the CA78K0R manual. Alternative: carve a MEMORY area out of RAM in the .dr file and MERGE    */
/* the BOOT_NOINIT segment into it.   

*/


volatile u32 gBootReqMagic;
volatile u32 gBootReqInv;






