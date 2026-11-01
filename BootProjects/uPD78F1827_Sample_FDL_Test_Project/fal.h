/* fal.h - FAL API for CA78K0R 
 */
#ifndef FAL_H
#define FAL_H

#include "system_def.h"


typedef unsigned char   fal_command_t;
enum {
    FAL_CMD_UNDEFINED       = 0x00,
    FAL_CMD_BLANKCHECK_WORD = 0x01,
    FAL_CMD_IVERIFY_WORD    = 0x02,
    FAL_CMD_READ_WORD       = 0x03,
    FAL_CMD_WRITE_WORD      = 0x04,
    FAL_CMD_ERASE_BLOCK     = 0x05
};

typedef unsigned char   fal_status_t;
enum {
    FAL_OK                  = 0x00,
    FAL_BUSY                = 0x01,
    FAL_ERR_PROTECTION      = 0x10,
    FAL_ERR_BLANKCHECK      = 0x11,
    FAL_ERR_VERIFY          = 0x12,
    FAL_ERR_WRITE           = 0x13,
    FAL_ERR_ERASE           = 0x14,
    FAL_ERR_PARAMETER       = 0x20,
    FAL_ERR_CONFIGURATION   = 0x21,
    FAL_ERR_INITIALIZATION  = 0x22,
    FAL_ERR_COMMAND         = 0x23,
    FAL_ERR_REJECTED        = 0x24
};

/* must be exactly 8 bytes: data_u32 +0, index_u16 +4, command +6, status +7 */
typedef struct {
    u32         data_u32;
    u16         index_u16;
    fal_command_t   command_enu;
    fal_status_t    status_enu;
} fal_request_t;
extern fal_request_t g_req;

typedef char fal_request_size_must_be_8[(sizeof(fal_request_t) == 8) ? 1 : -1];

/* The descriptor is kept as the 62 raw bytes the library was tested with. */
typedef struct { u8 raw[62]; } fal_descriptor_t;
extern const unsigned char fal_descriptor_tbl[62];
#define fal_descriptor_str (*(const fal_descriptor_t *)fal_descriptor_tbl)

void          FAL_Open(void);
void          FAL_Close(void);
fal_status_t  FAL_Init(const fal_descriptor_t __far *descriptor_pstr);
void          FAL_Execute(fal_request_t *request_pstr);
void          FAL_Handler(void);
u8 __far *FAL_GetVersionString(void);

void Fal_Issue(fal_command_t cmd, u16 idx, u32 data);
fal_status_t Fal_Poll(void);
fal_status_t Fal_Run(fal_command_t cmd, u16 idx, u32 data);
u8 Rd(u16 idx, u32 *v);
u16 Word_Base(u8 b, u8 c, u16 slot);
u32 Hdr_Word(u8 b);
u16 Crc16_Word(u16 crc, u32 v);
u32 Crc_Word(u16 crc);
u16 Data_Words(u8 b);
u32 Data_Word(u8 b, u16 i);
u16 Shadow_Crc(u8 b, u32 count);
u32 Slot_Word(u8 b, u16 w);
u8 Slot_Check(u8 b, u8 c, u16 slot, u32 *count);
u8 Slot_Load(u8 b, u8 c, u16 slot);
void Job_Start(u8 b, u32 count, u8 mask);
void Job_Finish(void);
void Job_Abort(void);
void Job_Copy_Fail(void);
void Job_Slot_Fail(void);
u8 Job_Step(void);
void Job_Run_Blocking(void);
void Scan_Copy(u8 b, u8 c);
void Init_Block(u8 b);











#endif
