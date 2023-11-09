							;Basic operating system
							;Made by Been
							;
org 0x7C00					;Basic directives
bits 16						;
							;

%define NEWL 0x0A			;Newline character
%define CRET 0x0D			;Carriage return
%define ENDL 0x0D, 0x0A		;Endline: new line + carriage return
							;
%define NULL 0x0			;Null
							;
							;
							;BIOS INTERRUPT CALLS: AH IN HEX AND ONLY NOTABLE ARE SHOWN
%define INT_VIDEO	 0x10	;All video services
%define INT_DISK	 0x13	;Low level disk services
%define INT_KEYBOARD 0X16	;Keyboard input interrupt


							; FAT12 file system signatures
jmp short start
nop
bdb_oem: db 'MSWIN4.1'
bdb_bps: dw 512
bdb_spc: db 1
bdb_rss: dw 1
bdb_ftc: db 2
bdb_dec: dw 0E0h
bdb_tss: dw 2880
bdb_mdt: db 0F0h
bdb_spf: dw 9
bdb_spt: dw 18
bdb_hds: dw 2
bdb_hsr: dd 0
bdb_lsc: dd 0

ebr_dnb: db 0
		 db 0
ebr_sig: db 029h
ebr_vid: db 78, 56, 34, 12
ebr_vlb: db 'VolumeLabel'
ebr_sid: db 'FAT12   '


;ENTRY POINT
start:

	mov ax, 0
	mov ds, ax
	mov es, ax

	mov ss, ax
	mov sp, 0x7c00

	push es
	push word .after
	retf


.after:

	mov [ebr_dnb], dl

; 	mov ax, 1
; 	mov cl, 1
; 	mov bx, 0x7E00

	;Read drive perameters
	;SPT & Head count
	push es
	mov ah, 08h
	int INT_DISK
	jc read_error
	pop es


	and cl, 0x3F
	xor ch, ch
	mov [bdb_spt], cx		;Sectors

	inc dh
	mov [bdb_hds], dh		;Heads


	mov ax, [bdb_spf]		;LBA of root = Reserved + total sectors
	mov bl, [bdb_ftc]
	xor bh, bh
	mul bx					;ax = total sectors
	add ax, [bdb_rss]		;ax = LBA
	push ax

	mov ax, [bdb_spf]

	mov ax, [bdb_spf]		;Size of root = 32 * Entries / BPS
	shl ax, 5				;ax *= 32
	xor dx, dx
	div word [bdb_bps]		;number of sectors to read

	test dx, dx				;if dx != 0 increment
	jz .noinc
	inc ax

.noinc:

	mov cl, al				;cl = size of root
	pop ax					;ax = LBA of root
	mov dl, [ebr_dnb]		;dl = drive number
	mov bx, buffer
	call read_disk


	;Look for kernel
	xor bx, bx
	mov di, buffer


.search_main:

	mov si, str_main_filename
	mov cx, 11					; length of string
	push di
	repe cmpsb
	pop di
	je .found_main
	add di, 32
	inc bx


	cmp bx, [bdb_dec]
	jl .search_main

	mov si, str_main_error
	call prints
	jmp wait_reboot

.found_main:

	mov ax, [di + 26]
	mov [main_cluster], ax


	;load fat from disk
	mov ax, [bdb_rss]
	mov bx, buffer
	mov cl, [bdb_spf]
	mov dl, [ebr_dnb]
	call read_disk

	mov bx, MAIN_SEGMENT
	mov es, bx
	mov bx, MAIN_OFFSET

.load_main:


	mov ax, [main_cluster]

	add ax, 31

	mov cl, 1
	mov dl, [ebr_dnb]
	call read_disk

	add bx, [bdb_bps]
	;NEED ERROR HANDLING HERE!!!

	mov ax, [main_cluster]
	mov cx, 3
	mul cx
	mov cx, 2
	div cx

	mov si, buffer
	add si, ax
	mov ax, [ds:si]

	or dx, dx
	jz .even

.odd:
	shr ax, 4
	jmp .next_cluster

.even:
	and ax, 0x0FFF

.next_cluster:

	cmp ax, 0x0FF8
	jae .read_finish

	mov [main_cluster], ax
	jmp .load_main

.read_finish:

	mov dl, [ebr_dnb]


	mov ax, MAIN_SEGMENT



	mov ds, ax
	mov es, ax



	jmp MAIN_SEGMENT:MAIN_OFFSET

	jmp wait_reboot


;
;Prints string in Teletype output
;Arguments:
;	si: Char* to be printed
;
prints:
	push si
	push ax

.loop:
	lodsb
	or al, al
	jz .done

	mov ah, 0x0e
	xor bh, bh
	int INT_VIDEO

	jmp .loop

.done:
	pop ax
	pop si
	ret


;
;Error Handlers:
;

read_error:
	mov si, str_read_error
	call prints
	jmp wait_reboot

main_error:
	mov si, str_main_error
	call prints
	jmp wait_reboot

wait_reboot:
	mov ah, 0
	int INT_KEYBOARD
	jmp 0FFFFh:0

.halt:
	cli
	hlt
;
;Convects LBA address to CHS
;Arguments:
;	ax: 		LBA address
;Return:
;	cx [0-5]: 	Sector
;	cx [6-15]: 	Cylinder
;	dh: 		Head
;
lba_to_chs:

	push ax
	push dx

	xor dx, dx
	div word [bdb_spt]	;ax = LBA / S/T
						;dx = LBA % S/T
	inc dx				;dx = sector
	mov cx, dx			;cx = sector

	xor dx, dx
	div word [bdb_hds]	;ax = (LBA / S/T) / Heads = cylinder
						;dx = (LBA / S/T) & Heads = head
	mov dh, dl			;dh = Head				| FINAL RETURN
	mov ch, al			;ch = Cylinder [0-7]
	shl ah, 6
	or cl, ah			;cl = Cylinder [8-9]	| FINAL RETURN

	pop ax
	mov dl, al			;Restore DL
	pop ax

	ret

;
;Reads sector from disk
;Arguments:
;	ax: LBA address
;	cl: # sectors to read (capped at 128)
;	dl: drive number
;	es:bx: pointer where to store data
;
read_disk:

	push ax
	push bx
	push cx
	push dx
	push di

	push cx
	call lba_to_chs
	pop ax

	mov ah, 02h
	mov di, 3

.retry:
	pusha
	stc
	int INT_DISK
	jnc .done

	;Fail condition
	popa
	call reset_disk

	dec di
	test di, di
	jnz .retry

	jmp read_error

.done:
	popa

	pop di
	pop dx
	pop cx
	pop bx
	pop ax

	ret

;
;Resets disk controller
;Arguments:
;	dl: drive number
;
reset_disk:
	pusha
	mov ah, 0
	stc
	int INT_DISK
	jc read_error
	popa
	ret


section.data:

;Strings:
str_read_error: 		db 'ERROR: B1', ENDL, 0
str_main_error:			db 'ERROR: B2 ', ENDL, 0
str_main_filename:		db 'MAIN    BIN'
main_cluster:			dw 0
char_endl:				db ENDL, 0



MAIN_SEGMENT     equ 0x2000
MAIN_OFFSET      equ 0





times 510 + $$ - $ db 0
dw 0AA55h

buffer: 				;times 50 db
