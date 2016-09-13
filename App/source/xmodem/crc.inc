	XMODEM16 equ	1	; compile for XMODEM16CRC
	KERMIT	 equ	0	; compile for KERMIT_F
	CCITT_F	 equ	0	; compile for CRC16_CCITT_F
	CCITT_CF equ	0	; compile for CRC16_CCITT_C_F
	ARC_F    equ	0
	ARC_ZF	 equ	0
;
; 8080/Z80 assembly code to implement the nine major 16-bit algorithms
; in constant time, without the use of lookup tables. 
; One of the routines is for faster calculation on a Z80 only.

;
; Main loop, for 8080/Z80
;
;       LD      HL,0            ; for XMODEM, KERMIT and ARC
;;      LD      HL,FFFFH        ; for false CCITT, X.25 and USB
;       LD      DE,data         ; set pointer to test string
;       LD      B, dataend-data ; set counter to string length
;tloop: LD      A,(DE)          ; get character of string
;       INC     DE              ; increment pointer
;       PUSH    BC              ; save counter
;       CALL    crc16_ccitt_f   ; do CRC on character
;       POP     BC              ; restore counter
;       DJNZ     tloop          ; and loop until string done
;;       LD      A,H             ; complement result
;;       CPL                     ; for false CCITT, X.25 and USB
;;       LD      H,A
;;       LD      A,L
;;       CPL
;;       LD      L,A
;        HALT

;
; CRC-16/CCITT-FALSE for 8080/Z80
; On entry HL = old CRC, A = byte
; On exit HL = new CRC, A,B,C undefined
; 115 T-states, 25 bytes
;
IF CCITT_F                      ; Ts  M/code    8080 assembly
crc16_ccitt_f:
        XOR     H               ;  4  AC        XRA     H
        LD      B,A             ;  4  47        MOV     B,A
        LD      C,L             ;  4  4D        MOV     C,L
        RRCA                    ;  4  0F        RRC
        RRCA                    ;  4  0F        RRC
        RRCA                    ;  4  0F        RRC
        RRCA                    ;  4  0F        RRC
        LD      L,A             ;  4  6F        MOV     L,A
        AND dw  0FH             ;  7  E6 0F     ANI dw  0FH
        LD      H,A             ;  4  67        MOV     H,A
        XOR     B               ;  4  A8        XRA     B
        LD      B,A             ;  4  47        MOV     B,A
        XOR     L               ;  4  AD        XRA     L
        AND     F0H             ;  7  E6 F0     ANI     F0H
        LD      L,A             ;  4  6F        MOV     L,A
        XOR     C               ;  4  A9        XRA     C
        ADD     HL,HL           ; 11  29        DAD     H
        XOR     H               ;  4  AC        XRA     H
        LD      H,A             ;  4  67        MOV     H,A
        LD      A,L             ;  4  7D        MOV     A,L
        XOR     B               ;  4  A8        XRA     B
        LD      L,A             ;  4  6F        MOV     L,A
        RET                     ; 10  C9        RET
ENDIF
;
;
; CRC-16/CCITT-FALSE for 8080/Z80
; On entry HL = old CRC, A = byte
; On exit HL = new CRC, A,B undefined
; 126 T-states, 31 bytes
;
IF CCITT_CF                     ; Ts  M/code    8080 assembly
crc16_ccitt_c_f:
        XOR     H               ;  4  AC        XRA     H
        LD      H,A             ;  4  67        MOV     H,A
        AND     F0H             ;  7  E6 F0     ANI     F0H
        RRCA                    ;  4  0F        RRC
        RRCA                    ;  4  0F        RRC
        RRCA                    ;  4  0F        RRC
        RRCA                    ;  4  0F        RRC
        XOR     H               ;  4  AC        XRA     H
        LD      H,A             ;  4  67        MOV     H,A
        RRCA                    ;  4  0F        RRC
        RRCA                    ;  4  0F        RRC
        RRCA                    ;  4  0F        RRC
        LD      B,A             ;  4  47        MOV     B,A
        AND     E0H             ;  7  E6 E0     ANI     E0H
        XOR     H               ;  4  AC        XRA     H
        LD      H,A             ;  4  67        MOV     H,A
        LD      A,B             ;  4  78        MOV     A,B
        AND     1FH             ;  7  E6 1F     ANI     1FH
        XOR     L               ;  4  AD        XRA     L
        LD      L,A             ;  4  6F        MOV     L,A
        LD      A,B             ;  4  78        MOV     A,B
        RRCA                    ;  4  0F        RRC
        AND     F0H             ;  7  E6 F0     ANI     F0H
        XOR     L               ;  4  AD        XRA     L
        LD      L,H             ;  4  6C        MOV     L,H
        LD      H,A             ;  4  67        MOV     H,A
        RET                     ; 10  C9        RET
ENDIF
;
;
; KERMIT (XMODEM) for 8080/Z80
; On entry HL = old CRC, A = byte
; On exit HL = new CRC, A,C undefined
; 119 T-states, 29 bytes
;
IF KERMIT			; Ts  M/code    8080 assembly
kermit_f:
        XOR     L               ;  4  AD        XRA     L
        LD      L,A             ;  4  6F        MOV     L,A
        ADD     A,A             ;  4  87        ADD     A
        ADD     A,A             ;  4  87        ADD     A
        ADD     A,A             ;  4  87        ADD     A
        ADD     A,A             ;  4  87        ADD     A
        XOR     L               ;  4  AD        XRA     L
        LD      L,A             ;  4  6F        MOV     L,A
        RLCA                    ;  4  07        RLC
        RLCA                    ;  4  07        RLC
        RLCA                    ;  4  07        RLC
        LD      C,A             ;  4  47        MOV     C,A
        AND dw  07H             ;  7  E6 07     ANI dw  07H
        XOR     L               ;  4  AD        XRA     L
        LD      L,A             ;  4  6F        MOV     L,A
        LD      A,C             ;  4  78        MOV     A,C
        AND dw  0F8H            ;  7  E6 F8     ANI     F8H
        XOR     H               ;  4  AC        XRA     H
        LD      H,A             ;  4  67        MOV     H,A
        LD      A,C             ;  4  78        MOV     A,C
        RLCA                    ;  4  07        RLC
        AND dw  0FH             ;  7  E6 0F     ANI dw  0FH
        XOR     H               ;  4  AC        XRA     H
        LD      H,L             ;  4  65        MOV     H,L
        LD      L,A             ;  4  6F        MOV     L,A
        RET                     ; 10  C9        RET
ENDIF
;
;
; CRC-16/ARC for 8080/Z80
; On entry HL = old CRC, A = byte
; On exit HL = new CRC, A,B undefined
; 125 T-states, 31 bytes
IF ARC_F                        ; Ts  M/code    8080 assembly
crc16_arc_f:
        XOR     L               ;  4  AD        XRA     L
        LD      L,A             ;  4  6F        MOV     L,A
        RRCA                    ;  4  0F        RRC
        RRCA                    ;  4  0F        RRC
        ;AND    A               ; needed if running in ZINT / no ballast
        JP      PE,blur         ; 10  EA nn nn  JPE     blur
        SCF                     ;  0  37        STC
blur:   JP      PO,blur1        ; 10  E2 nn nn  JPO     blur1
        AND     A               ;  4  A7        ANA     A
blur1:  RRA                     ;  4  1F        RAR
        AND     E0H             ;  7  E6 E0     ANI     E0H
        RLA                     ;  4  17        RAL
        LD      B,A             ;  4  47        MOV     B,A
        RLA                     ;  4  17        RAL
        XOR     B               ;  4  A8        XRA     B
        XOR     H               ;  4  AC        XRA     H
        LD      B,A             ;  4  47        MOV     B,A
        XOR     H               ;  4  AC        XRA     H
        RRA                     ;  4  1F        RAR
        LD      A,L             ;  4  7D        MOV     A,L
        RRA                     ;  4  1F        RAR
        LD      L,A             ;  4  6F        MOV     L,A
        AND     A               ;  4  A7        ANA     A
        RRA                     ;  4  1F        RAR
        XOR     L               ;  4  AD        XRA     L
        LD      L,B             ;  4  68        MOV     L,B
        LD      H,A             ;  4  67        MOV     H,A
        RET                     ; 10  C9        RET
ENDIF
;
;
; CRC-16/ARC for Z80 only
; On entry HL = old CRC, A = byte
; On exit HL = new CRC, A,B undefined
; 113 T-states, 29 bytes
IF ARC_ZF			; Ts  M/code
crc16_arc_z80_f:
        LD      B,0             ;  7  06 00
        XOR     L               ;  4  AD
        ;AND    A               ; needed if running in ZINT
        JP      PE,blur80       ; 10  EA nn nn
        SCF                     ;  0  37
blur80: JP      PO,blur81       ; 10  E2 nn nn
        NOP                     ;  4  00
blur81: RRA                     ;  4  1F
        RR      B               ;  8  CB 18
        LD      L,A             ;  4  6F
        SRL     A               ;  8  CB 3F
        RR      B               ;  8  CB 18
        XOR     L               ;  4  AD
        LD      L,A             ;  4  6F
        ADD     A,A             ;  4  87
        LD      A,B             ;  4  78
        RLA                     ;  4  17
        XOR     B               ;  4  A8
        XOR     H               ;  4  AC
        LD      H,L             ;  4  65
        LD      L,A             ;  4  6F
        RET                     ; 10  C9
ENDIF


IF XMODEM16
; 
;function CalcXModemCRC16(P: PChar; Length: Word): Word;
;var I: Word;
;begin
;  Result := 0;
;  for I := 1 to Length do begin
;    Result := (Byte(Result) shl 8) xor XModemCRC16Table[Hi(Result)] xor
;Byte(P^);
;    Inc(P);
;  end;
;end;
;
; The polynome is X^16 + X^12 + X^5 + 1
; Input: 	A=next byte (P^)
;		HL = Result (initially HL=0)
;
crc16_xmodem:
	ld	c, l
	push	de
	ld	e, h
	ld	d, 0
	ld	hl, CRC16Table
	add	hl, de		
	add	hl, de		; word wide index: ; (hl)=XModemCRC16Table[Hi(Result)]
	xor	(hl)		; (hl) xor Byte(P^)
	ld	e, a
	inc	hl		
	ld	a, c
	xor	(hl)		; (hl+1) xor (Byte(Result) shl 8)
	ld	h, a
	ld	l, e
	pop	de
	ret
CRC16Table: 
dw  00000h, 01021h, 02042h, 03063h, 04084h, 050A5h, 060C6h, 070E7h
dw  08108h, 09129h, 0A14Ah, 0B16Bh, 0C18Ch, 0D1ADh, 0E1CEh, 0F1EFh
dw  01231h, 00210h, 03273h, 02252h, 052B5h, 04294h, 072F7h, 062D6h
dw  09339h, 08318h, 0B37Bh, 0A35Ah, 0D3BDh, 0C39Ch, 0F3FFh, 0E3DEh
dw  02462h, 03443h, 00420h, 01401h, 064E6h, 074C7h, 044A4h, 05485h
dw  0A56Ah, 0B54Bh, 08528h, 09509h, 0E5EEh, 0F5CFh, 0C5ACh, 0D58Dh
dw  03653h, 02672h, 01611h, 00630h, 076D7h, 066F6h, 05695h, 046B4h
dw  0B75Bh, 0A77Ah, 09719h, 08738h, 0F7DFh, 0E7FEh, 0D79Dh, 0C7BCh
dw  048C4h, 058E5h, 06886h, 078A7h, 00840h, 01861h, 02802h, 03823h
dw  0C9CCh, 0D9EDh, 0E98Eh, 0F9AFh, 08948h, 09969h, 0A90Ah, 0B92Bh
dw  05AF5h, 04AD4h, 07AB7h, 06A96h, 01A71h, 00A50h, 03A33h, 02A12h
dw  0DBFDh, 0CBDCh, 0FBBFh, 0EB9Eh, 09B79h, 08B58h, 0BB3Bh, 0AB1Ah
dw  06CA6h, 07C87h, 04CE4h, 05CC5h, 02C22h, 03C03h, 00C60h, 01C41h
dw  0EDAEh, 0FD8Fh, 0CDECh, 0DDCDh, 0AD2Ah, 0BD0Bh, 08D68h, 09D49h
dw  07E97h, 06EB6h, 05ED5h, 04EF4h, 03E13h, 02E32h, 01E51h, 00E70h
dw  0FF9Fh, 0EFBEh, 0DFDDh, 0CFFCh, 0BF1Bh, 0AF3Ah, 09F59h, 08F78h
dw  09188h, 081A9h, 0B1CAh, 0A1EBh, 0D10Ch, 0C12Dh, 0F14Eh, 0E16Fh
dw  01080h, 000A1h, 030C2h, 020E3h, 05004h, 04025h, 07046h, 06067h
dw  083B9h, 09398h, 0A3FBh, 0B3DAh, 0C33Dh, 0D31Ch, 0E37Fh, 0F35Eh
dw  002B1h, 01290h, 022F3h, 032D2h, 04235h, 05214h, 06277h, 07256h
dw  0B5EAh, 0A5CBh, 095A8h, 08589h, 0F56Eh, 0E54Fh, 0D52Ch, 0C50Dh
dw  034E2h, 024C3h, 014A0h, 00481h, 07466h, 06447h, 05424h, 04405h
dw  0A7DBh, 0B7FAh, 08799h, 097B8h, 0E75Fh, 0F77Eh, 0C71Dh, 0D73Ch
dw  026D3h, 036F2h, 00691h, 016B0h, 06657h, 07676h, 04615h, 05634h
dw  0D94Ch, 0C96Dh, 0F90Eh, 0E92Fh, 099C8h, 089E9h, 0B98Ah, 0A9ABh
dw  05844h, 04865h, 07806h, 06827h, 018C0h, 008E1h, 03882h, 028A3h
dw  0CB7Dh, 0DB5Ch, 0EB3Fh, 0FB1Eh, 08BF9h, 09BD8h, 0ABBBh, 0BB9Ah
dw  04A75h, 05A54h, 06A37h, 07A16h, 00AF1h, 01AD0h, 02AB3h, 03A92h
dw  0FD2Eh, 0ED0Fh, 0DD6Ch, 0CD4Dh, 0BDAAh, 0AD8Bh, 09DE8h, 08DC9h
dw  07C26h, 06C07h, 05C64h, 04C45h, 03CA2h, 02C83h, 01CE0h, 00CC1h
dw  0EF1Fh, 0FF3Eh, 0CF5Dh, 0DF7Ch, 0AF9Bh, 0BFBAh, 08FD9h, 09FF8h
dw  06E17h, 07E36h, 04E55h, 05E74h, 02E93h, 03EB2h, 00ED1h, 01EF0h

ENDIF





