;------------------------------------------------------------------------------
; fdl_call.asm  -  assembler glue between CA78K0R C code and FDL dl_drv
; NOT yet assembled or tested. If RA78K0R rejects a directive, fix the syntax only.
;
; What it does
;  - FAL_Open / FAL_Close / FAL_Handler take no arguments: plain C-callable wrappers.
;  - FAL_Init, FAL_Execute, FAL_GetVersionString pass their data through the
;    fdlg_* variables below, so this file does not depend on how CA78K0R passes
;    arguments. fdl_wrap.c fills / reads those variables.
;  - HL is saved around every call into the driver ,CA78K0R callers use HL as a frame pointer.
;
;  FAL_Init:   far pointer pushed on the stack, HIGH word first, then LOW word;
;              the blob removes the 4 bytes itself; status returned in A.
;  FAL_Execute: near pointer in AX.
;  FAL_GetVersionString: result is A (bank byte) : HL (low word).
;------------------------------------------------------------------------------
        NAME    FDLGLUE

        EXTRN   FDLRAW_Init
        EXTRN   FDLRAW_Execute
        EXTRN   FDLRAW_Handler
        EXTRN   FDLRAW_Open
        EXTRN   FDLRAW_Close
        EXTRN   FDLRAW_GetVersionString

        PUBLIC  _FAL_Open
        PUBLIC  _FAL_Close
        PUBLIC  _FAL_Handler
        PUBLIC  _fdlg_init
        PUBLIC  _fdlg_execute
        PUBLIC  _fdlg_getver
        PUBLIC  _fdlg_ptr_lo
        PUBLIC  _fdlg_ptr_hi
        PUBLIC  _fdlg_req
        PUBLIC  _fdlg_ver_lo
        PUBLIC  _fdlg_ver_hi
        PUBLIC  _fdlg_status

;--- exchange variables (RAM) ---
FDLGDAT DSEG    BASEP
_fdlg_ptr_lo:   DS      2
_fdlg_ptr_hi:   DS      2
_fdlg_req:      DS      2
_fdlg_ver_lo:   DS      2
_fdlg_ver_hi:   DS      1
_fdlg_status:   DS      1

;--- code ---
FDLGCOD CSEG

; void FAL_Open(void)
_FAL_Open:
        PUSH    HL
        CALL    !!FDLRAW_Open
        POP     HL
        RET

; void FAL_Close(void)
_FAL_Close:
        PUSH    HL
        CALL    !!FDLRAW_Close
        POP     HL
        RET

; void FAL_Handler(void)
_FAL_Handler:
        PUSH    HL
        CALL    !!FDLRAW_Handler
        POP     HL
        RET

; void fdlg_init(void)    in: fdlg_ptr_hi:fdlg_ptr_lo   out: fdlg_status
_fdlg_init:
        PUSH    HL
        MOVW    AX,!_fdlg_ptr_hi
        PUSH    AX
        MOVW    AX,!_fdlg_ptr_lo
        PUSH    AX
        CALL    !!FDLRAW_Init
        MOV     !_fdlg_status,A
        POP     HL
        RET

; void fdlg_execute(void)    in: fdlg_req (16-bit near address of fal_request_t)
_fdlg_execute:
        PUSH    HL
        MOVW    AX,!_fdlg_req
        CALL    !!FDLRAW_Execute
        POP     HL
        RET

; void fdlg_getver(void)    out: fdlg_ver_hi:fdlg_ver_lo
_fdlg_getver:
        PUSH    HL
        CALL    !!FDLRAW_GetVersionString
        MOV     !_fdlg_ver_hi,A
        MOVW    AX,HL
        MOVW    !_fdlg_ver_lo,AX
        POP     HL
        RET

        END
