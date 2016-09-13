DEBUG	equ	0
;
; Calculate block CRC (additional or CRC16)
;
calc_crc:
	LD      HL,0            ; for XMODEM, KERMIT and ARC
	LD      DE, def_dma     ; set pointer to buffer
	LD      B, 128		; set counter to string length
	ld	A, (crcmode)
	cp	crc
	jr	z, tloop
cloop:	ld	a, (de)
	add	A, l
	ld	l, a
	inc	de
	djnz	cloop	
	ret
tloop:	LD      A,(DE)          ; get character of string
	INC     DE              ; increment pointer
	CALL    crc16_xmodem	; do CRC on character
	DJNZ    tloop           ; and loop until string done
	xor	a
	call	crc16_xmodem
	xor	a
	call	crc16_xmodem	; 128 BYTES TRANSMITTED SO ADD TO CRC TWO
	ld	a, h		; transmit high byte of CRC16
;
; Transmit byte with timeout from A
;
tx_t1:	ld	de, 0
	LD	c,A	;байт для передачи
;/-----
send1:	CALL	tst_tx	  
	JR	NZ,sendb 
	or	e
	CALL	z, tst_brk   ;CY=1 если нажата клавиша
	RET	C
	dec	de
	ld	a, d
	or	e
	jr	nz, send1
	scf			; timeout flag
	ret
;
sendb:	LD	A,C	;байт здесь
IF DEBUG
	push	hl
	ld	hl, (DEBUG_OUT_ADDR2)
	ld 	(hl), a
	inc	hl
	ld	(DEBUG_OUT_ADDR2), hl
	pop	hl
ENDIF
	CALL	dat_ou	;передать байт (A)
	OR	A	;CY=0 OK
	RET
;
;--------------------------------------------------
; put_block - put a block of data to the modem,
;	      consisting of -
;	      (i)   block number
;	      (ii)  complement of the block number
;	      (iii) 128 data bytes (1 sector) at *(def_dma)
;	      (iv)  checksum
; Output    : CY=0  if all went o.k.
;		A - block number of the block
;	      CY=1 if an error occurred
;		  - timed out
;--------------------------------------------------
;
put_block:
; номер блока
	ld	a, soh
	call	tx_t1		; put the start_of_header byte
	ret	c		; return (CY=1) if we timed out
	ld	a, (blockno)	
	call	tx_t1		; put the block no, timeout 1 sec
	ret	c		; return (CY=1) if we timed out
; принят инверсный номер блока
	ld	a, (blockno)	
	cpl			; ~ block no.
	call	tx_t1		; put the ~(block no).
	ret	c		; return (CY=1) if we timed out
	ld	hl,def_dma	; put data in the default DMA buffer
	ld	b,128		; 128 bytes / block
put_1:
	ld	a, (hl)
	call	tx_t1		; put a data byte
	ret	c		; return (CY=1) if we timed out
	inc	hl
	djnz	put_1		; do the whole block
	call 	calc_crc
	ld	a, l		; low byte of CRC16 or entire checksum (byte)
	jp	tx_t1		; put checksum
;
;    SOH = chr(1)  = CTRL-A =
;    EOT = chr(4)  = CTRL-D = End of Transmission
;    ACK = chr(6)  = CTRL-F = Positive Acknowledgement
;    NAK = chr(21) = CTRL-U = Negative Acknowledgement
;    CAN = chr(24) = CTRL-X = Cancel
;    CRC = 'C'
;
;----------------------------------------------------------------
; Передать файл по протоколу XMODEM
;
; 1. Init: block_no:=0, OpenFile
;    Читаем байт от принимающего хоста в Acc, timeout=30sec
;    if Acc<>NAC then exit
;
; 2. block_no:=block_no+1, Tries_no:=0
;    Читаем 128-байтный блок из файла.
;    Если конец файла - send EOT, read NAK, Abort()
;
; 3. if Tries_no=10 then Abort else 
;     - Tries_no:=Tries_no+1
;     - посылаем блок (SOH, block_no, 255-block_no, data, sum)  
;     - Читаем байт от принимающего хоста в Acc
;        - если Acc=CAN, then Abort()
;        - если Acc=NAK, goto 3
;        - если Acc=ACK, goto 2
;
; Abort: send EOT
;        check for input char, read if ready (ACK),
;        exit
;
; Data flow example. <xx> represents the checksum byte.
;
;  SENDER                           RECIEVER
;                                   Times out after 10 seconds,
;                           <---    <nak>
;  <soh> 01 FE -data- <xx>   --->
;                           <---    <ack>
;  <soh> 02 FD -data- <xx>   --->   (data gets line hit)
;                           <---    <nak>
;  <soh> 02 FD -data- <xx>   --->
;                           <---    <ack>
;  <soh> 03 FC -data- <xx>   --->
;    (ack gets garbaged)    <---    <ack>
;  <soh> 03 FC -data- <xx>   --->
;                           <---    <ack>
;  <eot>                     --->
;                           <---    <ack>
;
tx_file:ld	b, 50
tx_f0:	call	rx_t10
	jr	c, tx_f1
	cp	nak
	jr	z, tx_f20
	cp	crc
	jr	z, tx_f20
tx_f1:	djnz	tx_f0
	scf			; CY=1 (ERROR)
	ret
tx_f20:	ld	(crcmode), a
	jr	tx_f22
tx_f21:	ld	hl, blockno	; next block (initially = 0+1 = 1)
	inc	(hl)
tx_f22:	
IF DEBUG
	ld	hl, msgdbg1
	call	pstr
ENDIF
	call	readsector	; Z if not EOF
	jr	z, tx_f2
	ld	a, eot		; read fail - stop transmission
	call	tx_t1
	call	rx_t1		; read NAK
	jr	Abort
tx_f2:	xor	a
	ld	(triesno), a
tx_f3:	ld	hl, triesno
	ld	a, (hl)
	cp	10
	jr	nc, Abort
	inc	(hl)		; triesno:=triesno+1
IF DEBUG
	ld	hl, msgdbg2
	call	pstr
ENDIF
	call	put_block
	jr	c, Abort	; if timeout
	call	rx_t10		; read NAK | ACK | CAN
	cp	crc
	jr	z, tx_f2	; repeat first block
	cp	can
	jr	z, Abort
	cp	nak
	jr	z, tx_f3	; retry transmit block
	cp	ack
	jr	z, tx_f21	; next block
	scf			; CY=1 (ERROR)
	ret
Abort:
	ld	a, eot
	call	tx_t1		; send EOT
	call	rx_t1		; read ACK
	or	a		; CY=0 (NO ERROR)
	ret
; 
blockno: db	1
triesno: db	0
crcmode: db	0
;
IF DEBUG
msgdbg1:defb	'readsector',13,10,0
msgdbg2:defb	'put_block',13,10,0
DEBUG_OUT_ADDR2:	defw	8080h
ENDIF

