;--------------------------------------------------
; get_block - get a block of data from the modem,
;	      consisting of -
;	      (i)   block number
;	      (ii)  complement of the block number
;	      (iii) 128 data bytes (1 sector), and
;	      (iv)  checksum
; Output    : CY=0  if all went o.k.
;		A - block number of the block
;		  - 128 bytes at *(def_dma)
;	      CY=1 if an error occurred
;		  - timed out
;		  - block no. <> ~ (~ block no.)
;		  - checksum wrong
;--------------------------------------------------
;
get_block:
	call	rx_t1		; get the block no.
	ret	c		; return (CY=1) if we timed out
; принят номер блока
	ld	c,a		; save the block no.
	call	rx_t1		; get ~ block no.
	ret	c		; return (CY=1) if we timed out
; принят инверсный номер блока
	cpl			; ~ ~ block no. (we hope)
	cp	c		; is the block no. right ?
	scf
	ret	nz		; return (CY=1) if block no. wrong
; принят правильный номер блока
	ld	hl,def_dma	; put data in the default DMA buffer
	ld	b,128		; 128 bytes / block
	ld	d,0		; initial checksum
;/-----
get_1:
	call	rx_t1		; get a data byte
	ret	c		; return (CY=1) if we timed out
;
	ld	(hl),a		; save the byte
	inc	hl
	add	a,d		; generate a new checksum
	ld	d,a		; save it
	djnz	get_1		; do the whole block
;\-----
	call	rx_t1		; get checksum
	ret	c		; return (CY=1) if we timed out
; принят байт контрольной суммы
	cp	d		; checksums the same ?
	ld	a,c		; return the block no. in A
	ret	z
	scf			;Ошибка контр.суммы
	ret
;----------------------------------------------------------------
; Принять файл по протоколу XMODEM
rx_file:
	ld	c,0		; initial "previous" block no.
rx_f1:	call	tx_nak		; send an initital <nak>
;//---------
rx_f2:  ld	b,10		; retry block errors 10 times
;/---------
rx_f3:	call	rx_t10		; look for a <soh> or <eot>
	jr	nc,rx_f5	; skip if we got something
; истек тайм-аут, послать NAK
rx_f4:	call	tx_nak		; send a <nak> to signal error
	djnz	rx_f3		; retry 10 times
;\----------
; прием прерван по превышению времени ожидания
	ld	hl,txt_err	;Ошибка приема
	xor	a		; 0
prn_:	ld	(flg_xm),a	;флаг приема файла
	jp	prn_tx		;выход с сообщением
;========== выход ======================
; принят байт
rx_f5:	cp	eot		; <eot> (end of transmission) ?
	jr	nz,rx_f6	; пока нет
; принят признак успешного приема файла
	call	tx_ack		; подтвердить прием
	ld	hl,txt_ok	;
	ld	a,0ffh		;флаг приема = 1
	jr	prn_		; файл успешно принят
;
rx_f6:	cp	soh		; <soh> (start of header) ?
	jr	nz,rx_f1	; нет <soh> или <eot>
;\-- ждем <soh> или <eot> ------/
; принят признак старта заголовка <SOH>
rx_f7:	push	bc
	call	get_block	; get the block
	pop	bc
	jr	c,rx_f1		; ошибка в блоке
; в а - номер блока
	cp	c		; resent the old block ?
	jr	nz,rx_f9	; skip if yes
; повторно принят текущий блок
	call	tx_ack		; подтвердить прием
	jr	rx_f1		; ждать следующий
;
rx_f9:	inc	c		; next block no.
	cp	c		; block no.s match ?
	jr	z,rx_f10	; skip if so
; ошибка номера блока
	dec	c		; go back a block
	jr	rx_f1		; снова ждать
rx_f10:
; блок принят
	push	bc		; save the block number
IF ZX
; переписать блок из буфера приема в файл
	ld	hl,def_dma	;буфер блока
	ld	de,(adr_fl)	;текущий адрес в файле
	ld	bc,128
	ldir
	ld	(adr_fl),de	;
ELSE
	call	savesector
ENDIF
	pop	bc		; retrieve the block number
	call	tx_ack		; acknowledge the block
	jr	rx_f2		; loop for next block
;============================================
; end rx_file
