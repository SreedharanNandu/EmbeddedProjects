/* fal_api.c */
#include "fal.h"

extern unsigned int  fdlg_ptr_lo;
extern unsigned int  fdlg_ptr_hi;
extern unsigned int  fdlg_req;
extern unsigned int  fdlg_ver_lo;
extern unsigned char fdlg_ver_hi;
extern unsigned char fdlg_status;

extern void fdlg_init(void);
extern void fdlg_execute(void);
extern void fdlg_getver(void);

fal_status_t FAL_Init(const fal_descriptor_t __far *descriptor_pstr)
{
    unsigned long addr = (unsigned long)descriptor_pstr;   /* 20-bit address */

    fdlg_ptr_lo = (unsigned int)(addr & 0xFFFFUL);
    fdlg_ptr_hi = (unsigned int)(addr >> 16);
    fdlg_init();
    return fdlg_status;
}

void FAL_Execute(fal_request_t *request_pstr)
{
    /* RAM objects: the library wants the low 16 bits of the address (e.g. 0xE700) */
    fdlg_req = (unsigned int)((unsigned long)request_pstr & 0xFFFFUL);
    fdlg_execute();
}

u8 __far *FAL_GetVersionString(void)
{
    fdlg_getver();
    return (u8 __far *)(((unsigned long)fdlg_ver_hi << 16) | (unsigned long)fdlg_ver_lo);
}
