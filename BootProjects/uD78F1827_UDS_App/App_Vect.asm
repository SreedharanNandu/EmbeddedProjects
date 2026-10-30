; ---------------------------------------------------------------------------
; App_Vect.asm  -  application interrupt branch table for the UDS bootloader (CS+ / CC78K0R)
;
; Same layout as the IAR reference (applvect.s26), fixed at 4000H-40BFH (192 bytes):
;   4000H      magic flag 0AAH (application table; the IAR bootloader placeholder used 0A5H)
;   4001H-4002H reset address  -> _@cstart (application start-up)
;   4003H-4005H slot for hardware vector 0002H
;   4006H ...   one 3-byte slot (BR !addr16 = ED lo hi) per hardware vector 0004H..007EH
;               slot address of vector v = 4000H + 6 + 3*((v-4)/2)
;
; The bootloader owns the hardware vectors at 0000H-007FH; each of them holds the
; address of the matching slot below. Unused vectors reset the application.
; The ISRs must be NEAR code (below 10000H): BR !addr16 cannot reach further.
; ---------------------------------------------------------------------------
        NAME    APPL_VECT

        EXTRN   _@cstart
        EXTRN   _MD_INTWDTI
        EXTRN   _MD_INTLVI
        EXTRN   _MD_INTP0
        EXTRN   _MD_INTP1
        EXTRN   _MD_INTP2
        EXTRN   _MD_INTP3
        EXTRN   _MD_INTP4
        EXTRN   _MD_INTP5
        EXTRN   _MD_INTCLM
        EXTRN   _MD_INTCSI00
        EXTRN   _MD_INTDMA0
        EXTRN   _MD_INTDMA1
        EXTRN   _MD_INTWUTM
        EXTRN   _MD_INTFL
        EXTRN   _MD_INTLT0
        EXTRN   _MD_INTLR0
        EXTRN   _MD_INTLS0
        EXTRN   _MD_INTPLR0
        EXTRN   _MD_INTTM00
        EXTRN   _MD_INTTM01
        EXTRN   _MD_INTTM02
        EXTRN   _MD_INTTM03
        EXTRN   _MD_INTAD
        EXTRN   _MD_INTLT1
        EXTRN   _MD_INTLR1
        EXTRN   _MD_INTLS1
        EXTRN   _MD_INTPLR1
        EXTRN   _MD_INTCSI10
        EXTRN   _MD_INTCSI11
        EXTRN   _MD_INTIIC11
        EXTRN   _MD_INTTM04
        EXTRN   _MD_INTTM05
        EXTRN   _MD_INTTM06
        EXTRN   _MD_INTTM07
        EXTRN   _MD_INTP6
        EXTRN   _MD_INTP7
        EXTRN   _MD_INTC0ERR
        EXTRN   _MD_INTC0WUP
        EXTRN   _MD_INTC0REC
        EXTRN   _MD_INTC0TRX
        EXTRN   _MD_INTTM10
        EXTRN   _MD_INTTM11
        EXTRN   _MD_INTTM12
        EXTRN   _MD_INTTM13
        EXTRN   _MD_INTMD
        EXTRN   _MD_INTTM14
        EXTRN   _MD_INTTM15
        EXTRN   _MD_INTTM16
        EXTRN   _MD_INTTM17
        EXTRN   _MD_INTDMA2
        EXTRN   _MD_INTDMA3


        PUBLIC  _AppFblId, _AppResetVec





APPFBLID  CSEG AT 04000H
_AppFblId:
        DB      05AH                    ; magic flag

APPVECT CSEG AT 04001H
_AppResetVec:
        DW      _@cstart                ; reset address read by the bootloader

; vector 0002H (reserved)
        BR      !_@cstart
; vector 0004H  INTWDTI
        BR      !_MD_INTWDTI
; vector 0006H  INTLVI
        BR      !_@cstart
; vector 0008H  INTP0
        BR      !_@cstart
; vector 000AH  INTP1
        BR      !_@cstart
; vector 000CH  INTP2
        BR      !_@cstart
; vector 000EH  INTP3
        BR      !_@cstart
; vector 0010H  INTP4
        BR      !_@cstart
; vector 0012H  INTP5
        BR      !_@cstart
; vector 0014H  INTCLM
        BR      !_@cstart
; vector 0016H  INTCSI00
        BR      !_MD_INTCSI00
; vector 0018H  INTCSI01
        BR      !_@cstart
; vector 001AH  INTDMA0
        BR      !_@cstart
; vector 001CH  INTDMA1
        BR      !_@cstart
; vector 001EH  INTWUTM
        BR      !_MD_INTWUTM
; vector 0020H  INTFL
        BR      !_@cstart
; vector 0022H  INTLT0
        BR      !_MD_INTLT0
; vector 0024H  INTLR0
        BR      !_MD_INTLR0
; vector 0026H  INTLS0
        BR      !_MD_INTLS0
; vector 0028H  INTPLR0
        BR      !_@cstart
; vector 002AH  reserved
        BR      !_@cstart
; vector 002CH  INTTM00
        BR      !_MD_INTTM00
; vector 002EH  INTTM01
        BR      !_@cstart
; vector 0030H  INTTM02
        BR      !_@cstart
; vector 0032H  INTTM03
        BR      !_@cstart
; vector 0034H  INTAD
        BR      !_@cstart
; vector 0036H  INTLT1
        BR      !_MD_INTLT1
; vector 0038H  INTLR1
        BR      !_MD_INTLR1
; vector 003AH  INTLS1
        BR      !_MD_INTLS1
; vector 003CH  INTPLR1
        BR      !_@cstart
; vector 003EH  INTCSI10
        BR      !_@cstart
; vector 0040H  INTIIC11
        BR      !_MD_INTIIC11
; vector 0042H  INTTM04
        BR      !_@cstart
; vector 0044H  INTTM05
        BR      !_@cstart
; vector 0046H  INTTM06
        BR      !_@cstart
; vector 0048H  INTTM07
        BR      !_@cstart
; vector 004AH  INTP6 & INTKR
        BR      !_@cstart
; vector 004CH  INTP7
        BR      !_@cstart
; vector 004EH  INTC0ERR
        BR      !_MD_INTC0ERR
; vector 0050H  INTC0WUP
        BR      !_MD_INTC0WUP
; vector 0052H  INTC0REC
        BR      !_MD_INTC0REC
; vector 0054H  INTC0TRX
        BR      !_MD_INTC0TRX
; vector 0056H  INTTM10
        BR      !_MD_INTTM10
; vector 0058H  INTTM11
        BR      !_MD_INTTM11
; vector 005AH  INTTM12
        BR      !_MD_INTTM12
; vector 005CH  INTTM13
        BR      !_@cstart
; vector 005EH  INTMD
        BR      !_@cstart
; vector 0060H  INTIIC20 & INTST2
        BR      !_@cstart
; vector 0062H  INTSR2
        BR      !_@cstart
; vector 0064H  INTPR2
        BR      !_@cstart
; vector 0066H  INTTM14
        BR      !_@cstart
; vector 0068H  INTTM15
        BR      !_@cstart
; vector 006AH  INTTM16
        BR      !_@cstart
; vector 006CH  INTTM17
        BR      !_@cstart
; vector 006EH  INTTM20
        BR      !_@cstart
; vector 0070H  INTTM21
        BR      !_@cstart
; vector 0072H  INTTM22
        BR      !_@cstart
; vector 0074H  INTTM23
        BR      !_@cstart
; vector 0076H  reserved
        BR      !_@cstart
; vector 0078H  reserved
        BR      !_@cstart
; vector 007AH  INTDMA2
        BR      !_@cstart
; vector 007CH  INTDMA3
        BR      !_@cstart
; vector 007EH  BRK_I
        BR      !_@cstart

        END
