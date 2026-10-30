; ---------------------------------------------------------------
; App_Vect.asm - app vector/branch table, placed by the LINKER.
; No absolute addresses here: the .dr file merges
;     APP_MAGIC -> ROM_MAGIC_FLAG   (4000H)
;     APP_VECT  -> ROM_INTVEC_REMAP (4001H)
;
; Layout of APP_VECT (contiguous):
;     _AppResetVec : DW  reset address
;     AVT_E01      : DB 0EDH / DW isr   (BR !addr16)   <- 3 bytes each
;     AVT_E02 ...  AVT_E63
; AVT_En is the slot for the boot vector at address 2*n.
;
; BOOT build: use this file as is (every slot -> _@cstart).
; APP  build: replace _@cstart with your ISR (plus an EXTRN) per slot.
; ---------------------------------------------------------------
        NAME    APPL_VECT

        EXTRN   _@cstart
        PUBLIC  _AppFblId, _AppResetVec 
        PUBLIC  AVT_E01, AVT_E02, AVT_E03, AVT_E04, AVT_E05, AVT_E06, AVT_E07, AVT_E08, AVT_E09
        PUBLIC  AVT_E10, AVT_E11, AVT_E12, AVT_E13, AVT_E14, AVT_E15, AVT_E16, AVT_E17, AVT_E18
        PUBLIC  AVT_E19, AVT_E20, AVT_E21, AVT_E22, AVT_E23, AVT_E24, AVT_E25, AVT_E26, AVT_E27
        PUBLIC  AVT_E28, AVT_E29, AVT_E30, AVT_E31, AVT_E32, AVT_E33, AVT_E34, AVT_E35, AVT_E36
        PUBLIC  AVT_E37, AVT_E38, AVT_E39, AVT_E40, AVT_E41, AVT_E42, AVT_E43, AVT_E44, AVT_E45
        PUBLIC  AVT_E46, AVT_E47, AVT_E48, AVT_E49, AVT_E50, AVT_E51, AVT_E52, AVT_E53, AVT_E54
        PUBLIC  AVT_E55, AVT_E56, AVT_E57, AVT_E58, AVT_E59, AVT_E60, AVT_E61, AVT_E62, AVT_E63

BR_VALUE EQU    0EDH

APPFBLID  CSEG 
_AppFblId:
        DB      0A5H                ; key

APPVECT CSEG
_AppResetVec:
        DW      _@cstart            ; reset vector (app: its own startup)

AVT_E01:  DB      BR_VALUE            ; vector 0002H
        DW      _@cstart
AVT_E02:  DB      BR_VALUE            ; vector 0004H
        DW      _@cstart
AVT_E03:  DB      BR_VALUE            ; vector 0006H
        DW      _@cstart
AVT_E04:  DB      BR_VALUE            ; vector 0008H
        DW      _@cstart
AVT_E05:  DB      BR_VALUE            ; vector 000AH
        DW      _@cstart
AVT_E06:  DB      BR_VALUE            ; vector 000CH
        DW      _@cstart
AVT_E07:  DB      BR_VALUE            ; vector 000EH
        DW      _@cstart
AVT_E08:  DB      BR_VALUE            ; vector 0010H
        DW      _@cstart
AVT_E09:  DB      BR_VALUE            ; vector 0012H
        DW      _@cstart
AVT_E10:  DB      BR_VALUE            ; vector 0014H
        DW      _@cstart
AVT_E11:  DB      BR_VALUE            ; vector 0016H
        DW      _@cstart
AVT_E12:  DB      BR_VALUE            ; vector 0018H
        DW      _@cstart
AVT_E13:  DB      BR_VALUE            ; vector 001AH
        DW      _@cstart
AVT_E14:  DB      BR_VALUE            ; vector 001CH
        DW      _@cstart
AVT_E15:  DB      BR_VALUE            ; vector 001EH
        DW      _@cstart
AVT_E16:  DB      BR_VALUE            ; vector 0020H
        DW      _@cstart
AVT_E17:  DB      BR_VALUE            ; vector 0022H
        DW      _@cstart
AVT_E18:  DB      BR_VALUE            ; vector 0024H
        DW      _@cstart
AVT_E19:  DB      BR_VALUE            ; vector 0026H
        DW      _@cstart
AVT_E20:  DB      BR_VALUE            ; vector 0028H
        DW      _@cstart
AVT_E21:  DB      BR_VALUE            ; vector 002AH
        DW      _@cstart
AVT_E22:  DB      BR_VALUE            ; vector 002CH
        DW      _@cstart
AVT_E23:  DB      BR_VALUE            ; vector 002EH
        DW      _@cstart
AVT_E24:  DB      BR_VALUE            ; vector 0030H
        DW      _@cstart
AVT_E25:  DB      BR_VALUE            ; vector 0032H
        DW      _@cstart
AVT_E26:  DB      BR_VALUE            ; vector 0034H
        DW      _@cstart
AVT_E27:  DB      BR_VALUE            ; vector 0036H
        DW      _@cstart
AVT_E28:  DB      BR_VALUE            ; vector 0038H
        DW      _@cstart
AVT_E29:  DB      BR_VALUE            ; vector 003AH
        DW      _@cstart
AVT_E30:  DB      BR_VALUE            ; vector 003CH
        DW      _@cstart
AVT_E31:  DB      BR_VALUE            ; vector 003EH
        DW      _@cstart
AVT_E32:  DB      BR_VALUE            ; vector 0040H
        DW      _@cstart
AVT_E33:  DB      BR_VALUE            ; vector 0042H
        DW      _@cstart
AVT_E34:  DB      BR_VALUE            ; vector 0044H
        DW      _@cstart
AVT_E35:  DB      BR_VALUE            ; vector 0046H
        DW      _@cstart
AVT_E36:  DB      BR_VALUE            ; vector 0048H
        DW      _@cstart
AVT_E37:  DB      BR_VALUE            ; vector 004AH
        DW      _@cstart
AVT_E38:  DB      BR_VALUE            ; vector 004CH
        DW      _@cstart
AVT_E39:  DB      BR_VALUE            ; vector 004EH
        DW      _@cstart
AVT_E40:  DB      BR_VALUE            ; vector 0050H
        DW      _@cstart
AVT_E41:  DB      BR_VALUE            ; vector 0052H
        DW      _@cstart
AVT_E42:  DB      BR_VALUE            ; vector 0054H
        DW      _@cstart
AVT_E43:  DB      BR_VALUE            ; vector 0056H
        DW      _@cstart
AVT_E44:  DB      BR_VALUE            ; vector 0058H
        DW      _@cstart
AVT_E45:  DB      BR_VALUE            ; vector 005AH
        DW      _@cstart
AVT_E46:  DB      BR_VALUE            ; vector 005CH
        DW      _@cstart
AVT_E47:  DB      BR_VALUE            ; vector 005EH
        DW      _@cstart
AVT_E48:  DB      BR_VALUE            ; vector 0060H
        DW      _@cstart
AVT_E49:  DB      BR_VALUE            ; vector 0062H
        DW      _@cstart
AVT_E50:  DB      BR_VALUE            ; vector 0064H
        DW      _@cstart
AVT_E51:  DB      BR_VALUE            ; vector 0066H
        DW      _@cstart
AVT_E52:  DB      BR_VALUE            ; vector 0068H
        DW      _@cstart
AVT_E53:  DB      BR_VALUE            ; vector 006AH
        DW      _@cstart
AVT_E54:  DB      BR_VALUE            ; vector 006CH
        DW      _@cstart
AVT_E55:  DB      BR_VALUE            ; vector 006EH
        DW      _@cstart
AVT_E56:  DB      BR_VALUE            ; vector 0070H
        DW      _@cstart
AVT_E57:  DB      BR_VALUE            ; vector 0072H
        DW      _@cstart
AVT_E58:  DB      BR_VALUE            ; vector 0074H
        DW      _@cstart
AVT_E59:  DB      BR_VALUE            ; vector 0076H
        DW      _@cstart
AVT_E60:  DB      BR_VALUE            ; vector 0078H
        DW      _@cstart
AVT_E61:  DB      BR_VALUE            ; vector 007AH
        DW      _@cstart
AVT_E62:  DB      BR_VALUE            ; vector 007CH
        DW      _@cstart
AVT_E63:  DB      BR_VALUE            ; vector 007EH
        DW      _@cstart

        END
