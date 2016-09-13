; rs232.inc
;
DEBUG	equ	0
;
; Процедуры работы с портом RS232 для ZX_MCard
;------------------------------------------------
SER_P	equ	0F8EFh	;Порт RS232 по Кондратьеву
P_KBD	equ	0FEh	;Порт клавиатуры Спектрума
;------------------------------------------------
;	передать байт (A)
; CY=0 передача успешна
; CY=1 выход по <BREAK>
tx_byte:PUSH	BC
	LD	C,A	;байт для передачи
;/-----
sen1byt:CALL	tst_tx	   ;готов к передаче ?
	JR	NZ,sen2byt ; да
; проверить нажатие клавиши <BREAK>
	CALL	tst_brk	   ;CY=1 если нажата
	jr	nc,sen1byt ;ждать до упора
; выход по нажатию <BREAK>
	POP	BC	   ;CY=1,Z=0
	RET
;\-----
;	Передатчик готов
sen2byt:LD	A,C	;байт здесь
	CALL	dat_ou	;передать байт (A)
	OR	A	;CY=0 OK
	POP	BC
	RET
;---------------------------------------------------------
; tx_ack - wait until the line is clear (i.e. we time out
;	   while receiving chars), and then send an <ack>
;---------------------------------------------------------
tx_ack:	call	rx_t1		; get a char, with 1 second timeout
	jr	nc,tx_ack	; if no timeout, keep gobbling chars
; истек тайм-аут
	ld	a,ack		; <ack> char
	jr	tx_byte		; send it
;---------------------------------------------------------
; tx_nak - wait until the line is clear (i.e. we time out
;	   while receiving chars), and then send a <nak>
;---------------------------------------------------------
tx_nak:	call	rx_t1		; get a char, with 1 second timeout
	jr	nc,tx_nak	; if no timeout, keep gobbling chars
; истек тайм-аут
	ld	a,nak		; <nak> char
	jr	tx_byte		; send it
;---------------------------------------------
; Прием с тайм-аутом в 1 сек
rx_t10:	push	DE
	ld	DE,0000h	; 1 сек
	jr	rec_b3
;-------------------------
; Принять байт с тайм-аутом в 0.1 сек
;	CY=0	прием OK
;	CY=1 	истек тайм-аут
rx_t1:	PUSH	DE
	LD	DE,2000h  	;тайм-аут
;/-----
rec_b3:	CALL	tst_rx	  	;готовность приемника
	JR	NZ,rec_b2	; готов
;
	DEC	DE
	LD	A,D
	OR	E
	JR	NZ,rec_b3
; истек тайм-аут
	scf			;CY=1 - признак тайм-аута
	pop	de
	ret
rec_b2:	call	dat_in		;принять байт
IF DEBUG
	ld	de, (DEBUG_OUT_ADDR)
	ld 	(de), a
	inc	de
	ld	(DEBUG_OUT_ADDR), de
ENDIF
	or	a		;CY=0 - признак OK
	pop	de
	ret
;---------------------------------
;------ Работа с портами ---------
;---------------------------------
; Принять байт из порта RS232
dat_in:	
IF ZX
	PUSH	BC
	LD	BC,SER_P	;Регистр данных
	IN	A,(C)
	POP	BC
ELSE
	ld	a, (UART_DATA)
ENDIF
	RET
;==============================
; Передать байт в порт RS232
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
; Проверить на получение байта
tst_rx:	
IF ZX
	PUSH	BC
	LD	BC,SER_P+5*100h ;Чтение (base+5)
	IN	A,(C)		;
	AND	01h		;RDY_RX(0)
	POP	BC
ELSE
	ld	a, (UART_CTL)
	and	40h
ENDIF
	RET
;=====================================
; Проверить на готовность передать
tst_tx:	
IF ZX
	PUSH	BC
	LD	BC,SER_P+6*100h
	IN	A,(C)		;(base+6)
	and	10h		;CTS ?
	JR	Z,no_tx		;не готов
; проверить Bufer передатчика
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
;  если нажат <BREAK-KEY>
;	C=1, A=0FFh
tst_brk:
IF ZX
	LD	A,7Fh		; A15=0
	IN	A,(P_KBD)	; клавиатура
	RRA			; d0=?
	jr	c,tst1brk	; нет <CAPS SHIFT>
; нажата клавиша <CAPS SHIFT>
	LD	A,0FEh		; a8=0
	IN	A,(P_KBD)	; клавиатура
	RRA			; d0=?
; C=0	+ нажата клавиша <SPACE>
	jr	c,tst1brk	; нет <SPACE>
	xor	a
	dec	a		;Z=0, CY=0
tst1brk:ccf			; инверсия CY
ELSE
	xor	a
ENDIF
	RET			;
; C=1 + нажата клавиша <SPACE>
;
IF DEBUG
DEBUG_OUT_ADDR:	defw	8000h
ENDIF
;--------------------------------------------
; end rs232.inc


