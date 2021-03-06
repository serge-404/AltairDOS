; rs232.inc
;
DEBUG	equ	0
;
; ��������� ������ � ������ RS232 ��� ZX_MCard
;------------------------------------------------
SER_P	equ	0F8EFh	;���� RS232 �� �����������
P_KBD	equ	0FEh	;���� ���������� ���������
;------------------------------------------------
;	�������� ���� (A)
; CY=0 �������� �������
; CY=1 ����� �� <BREAK>
tx_byte:PUSH	BC
	LD	C,A	;���� ��� ��������
;/-----
sen1byt:CALL	tst_tx	   ;����� � �������� ?
	JR	NZ,sen2byt ; ��
; ��������� ������� ������� <BREAK>
	CALL	tst_brk	   ;CY=1 ���� ������
	jr	nc,sen1byt ;����� �� �����
; ����� �� ������� <BREAK>
	POP	BC	   ;CY=1,Z=0
	RET
;\-----
;	���������� �����
sen2byt:LD	A,C	;���� �����
	CALL	dat_ou	;�������� ���� (A)
	OR	A	;CY=0 OK
	POP	BC
	RET
;---------------------------------------------------------
; tx_ack - wait until the line is clear (i.e. we time out
;	   while receiving chars), and then send an <ack>
;---------------------------------------------------------
tx_ack:	call	rx_t1		; get a char, with 1 second timeout
	jr	nc,tx_ack	; if no timeout, keep gobbling chars
; ����� ����-���
	ld	a,ack		; <ack> char
	jr	tx_byte		; send it
;---------------------------------------------------------
; tx_nak - wait until the line is clear (i.e. we time out
;	   while receiving chars), and then send a <nak>
;---------------------------------------------------------
tx_nak:	call	rx_t1		; get a char, with 1 second timeout
	jr	nc,tx_nak	; if no timeout, keep gobbling chars
; ����� ����-���
	ld	a,nak		; <nak> char
	jr	tx_byte		; send it
;---------------------------------------------
; ����� � ����-����� � 1 ���
rx_t10:	push	DE
	ld	DE,0000h	; 1 ���
	jr	rec_b3
;-------------------------
; ������� ���� � ����-����� � 0.1 ���
;	CY=0	����� OK
;	CY=1 	����� ����-���
rx_t1:	PUSH	DE
	LD	DE,2000h  	;����-���
;/-----
rec_b3:	CALL	tst_rx	  	;���������� ���������
	JR	NZ,rec_b2	; �����
;
	DEC	DE
	LD	A,D
	OR	E
	JR	NZ,rec_b3
; ����� ����-���
	scf			;CY=1 - ������� ����-����
	pop	de
	ret
rec_b2:	call	dat_in		;������� ����
IF DEBUG
	ld	de, (DEBUG_OUT_ADDR)
	ld 	(de), a
	inc	de
	ld	(DEBUG_OUT_ADDR), de
ENDIF
	or	a		;CY=0 - ������� OK
	pop	de
	ret
;---------------------------------
;------ ������ � ������� ---------
;---------------------------------
; ������� ���� �� ����� RS232
dat_in:	
IF ZX
	PUSH	BC
	LD	BC,SER_P	;������� ������
	IN	A,(C)
	POP	BC
ELSE
	ld	a, (UART_DATA)
ENDIF
	RET
;==============================
; �������� ���� � ���� RS232
dat_ou:	
IF ZX
	PUSH	BC
	LD	BC,SER_P
	OUT	(C),A
	POP	BC
ELSE
	ld	(UART_DATA), a
ENDIF
	RET
;==============================
; ��������� �� ��������� �����
tst_rx:	
IF ZX
	PUSH	BC
	LD	BC,SER_P+5*100h ;������ (base+5)
	IN	A,(C)		;
	AND	01h		;RDY_RX(0)
	POP	BC
ELSE
	ld	a, (UART_CTL)
	and	40h
ENDIF
	RET
;=====================================
; ��������� �� ���������� ��������
tst_tx:	
IF ZX
	PUSH	BC
	LD	BC,SER_P+6*100h
	IN	A,(C)		;(base+6)
	and	10h		;CTS ?
	JR	Z,no_tx		;�� �����
; ��������� Bufer �����������
	DEC	B		;BC=SER_P+5*100h
	IN	A,(C)		;(base+5)
	and	20h		;Bufer empty ?
no_tx:	POP	BC
ELSE
	ld	a, (UART_CTL)
	and	80h
ENDIF
	RET
;============================
res_rts:
IF ZX
	PUSH	BC
	LD	BC,SER_P+4*100h
	LD	A,01h		;RTS=0,DTR=1
	jr	set_reg
ENDIF
;============================
set_rts:
IF ZX
	push	bc
	ld	bc,SER_P+4*100h
	ld	a,03h		;RTS=1,DTR=1
set_reg:OUT	(C),A
	POP	BC
ENDIF
	RET
;-----------------------------------------------
;  ���� ����� <BREAK-KEY>
;	C=1, A=0FFh
tst_brk:
IF ZX
	LD	A,7Fh		; A15=0
	IN	A,(P_KBD)	; ����������
	RRA			; d0=?
	jr	c,tst1brk	; ��� <CAPS SHIFT>
; ������ ������� <CAPS SHIFT>
	LD	A,0FEh		; a8=0
	IN	A,(P_KBD)	; ����������
	RRA			; d0=?
; C=0	+ ������ ������� <SPACE>
	jr	c,tst1brk	; ��� <SPACE>
	xor	a
	dec	a		;Z=0, CY=0
tst1brk:ccf			; �������� CY
ELSE
	xor	a
ENDIF
	RET			;
; C=1 + ������ ������� <SPACE>
;
IF DEBUG
DEBUG_OUT_ADDR:	defw	8000h
ENDIF
;--------------------------------------------
; end rs232.inc


