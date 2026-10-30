; ---------------------------------------------------------------
; Boot_Vect.asm - boot vector table at 0x00, forwarding to the app
; branch table. Targets are the AVT_En labels from App_Vect.asm, so the
; linker (.dr) decides where the table lives - nothing hardcoded here.
;
;  0x0000 reset  : @cstart (boot)       0x0002 : debugger, linker-made
;  0x0004..0x007E : DW AVT_E02 .. AVT_E63  (segment BOOTVECT, merged by .dr)
; ---------------------------------------------------------------
        NAME    BOOT_VECT

        EXTRN   AVT_E02, AVT_E03, AVT_E04, AVT_E05, AVT_E06, AVT_E07, AVT_E08, AVT_E09, AVT_E10
        EXTRN   AVT_E11, AVT_E12, AVT_E13, AVT_E14, AVT_E15, AVT_E16, AVT_E17, AVT_E18, AVT_E19
        EXTRN   AVT_E20, AVT_E21, AVT_E22, AVT_E23, AVT_E24, AVT_E25, AVT_E26, AVT_E27, AVT_E28
        EXTRN   AVT_E29, AVT_E30, AVT_E31, AVT_E32, AVT_E33, AVT_E34, AVT_E35, AVT_E36, AVT_E37
        EXTRN   AVT_E38, AVT_E39, AVT_E40, AVT_E41, AVT_E42, AVT_E43, AVT_E44, AVT_E45, AVT_E46
        EXTRN   AVT_E47, AVT_E48, AVT_E49, AVT_E50, AVT_E51, AVT_E52, AVT_E53, AVT_E54, AVT_E55
        EXTRN   AVT_E56, AVT_E57, AVT_E58, AVT_E59, AVT_E60, AVT_E61, AVT_E62, AVT_E63

FBLVECT  CSEG AT 0004H                         ; placed by linker (see UDS_ram.dr)
        DW      AVT_E02                 ; vector 0004H
        DW      AVT_E03                 ; vector 0006H
        DW      AVT_E04                 ; vector 0008H
        DW      AVT_E05                 ; vector 000AH
        DW      AVT_E06                 ; vector 000CH
        DW      AVT_E07                 ; vector 000EH
        DW      AVT_E08                 ; vector 0010H
        DW      AVT_E09                 ; vector 0012H
        DW      AVT_E10                 ; vector 0014H
        DW      AVT_E11                 ; vector 0016H
        DW      AVT_E12                 ; vector 0018H
        DW      AVT_E13                 ; vector 001AH
        DW      AVT_E14                 ; vector 001CH
        DW      AVT_E15                 ; vector 001EH
        DW      AVT_E16                 ; vector 0020H
        DW      AVT_E17                 ; vector 0022H
        DW      AVT_E18                 ; vector 0024H
        DW      AVT_E19                 ; vector 0026H
        DW      AVT_E20                 ; vector 0028H
        DW      AVT_E21                 ; vector 002AH
        DW      AVT_E22                 ; vector 002CH
        DW      AVT_E23                 ; vector 002EH
        DW      AVT_E24                 ; vector 0030H
        DW      AVT_E25                 ; vector 0032H
        DW      AVT_E26                 ; vector 0034H
        DW      AVT_E27                 ; vector 0036H
        DW      AVT_E28                 ; vector 0038H
        DW      AVT_E29                 ; vector 003AH
        DW      AVT_E30                 ; vector 003CH
        DW      AVT_E31                 ; vector 003EH
        DW      AVT_E32                 ; vector 0040H
        DW      AVT_E33                 ; vector 0042H
        DW      AVT_E34                 ; vector 0044H
        DW      AVT_E35                 ; vector 0046H
        DW      AVT_E36                 ; vector 0048H
        DW      AVT_E37                 ; vector 004AH
        DW      AVT_E38                 ; vector 004CH
        DW      AVT_E39                 ; vector 004EH
        DW      AVT_E40                 ; vector 0050H
        DW      AVT_E41                 ; vector 0052H
        DW      AVT_E42                 ; vector 0054H
        DW      AVT_E43                 ; vector 0056H
        DW      AVT_E44                 ; vector 0058H
        DW      AVT_E45                 ; vector 005AH
        DW      AVT_E46                 ; vector 005CH
        DW      AVT_E47                 ; vector 005EH
        DW      AVT_E48                 ; vector 0060H
        DW      AVT_E49                 ; vector 0062H
        DW      AVT_E50                 ; vector 0064H
        DW      AVT_E51                 ; vector 0066H
        DW      AVT_E52                 ; vector 0068H
        DW      AVT_E53                 ; vector 006AH
        DW      AVT_E54                 ; vector 006CH
        DW      AVT_E55                 ; vector 006EH
        DW      AVT_E56                 ; vector 0070H
        DW      AVT_E57                 ; vector 0072H
        DW      AVT_E58                 ; vector 0074H
        DW      AVT_E59                 ; vector 0076H
        DW      AVT_E60                 ; vector 0078H
        DW      AVT_E61                 ; vector 007AH
        DW      AVT_E62                 ; vector 007CH
        DW      AVT_E63                 ; vector 007EH

        END
