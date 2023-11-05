bits 16

section _TEXT class=CODE

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
%define INT_RTC		 0x1A	;Real time clock

global __U4D
__U4D:
	shl edx, 16 		; Dx to upper half of edx
	mov dx, ax
	mov eax, edx
	xor edx, edx

	shl ecx, 16
	mov cx, bx

	div ecx
	mov ebx, edx
	mov ecx, edx
	shr ecx, 16

	mov edx, eax
	shr edx, 16

	ret
;
; Prints string in Teletype output
; Arguments:
;	character, page
;
global _asm_tt_printc
_asm_tt_printc:

    push bp
    mov bp, sp

    push bx


	mov ah, 0Eh
	mov al, [bp + 4]
	mov bh, [bp + 6]
	int INT_VIDEO

	pop bx
	mov sp, bp
	pop bp
	ret


;
; Waits for user to press key then jumps to
;		beginning of the BIOS
;
global _asm_key_reboot
_asm_key_reboot:
    mov ah, 0
	int INT_KEYBOARD
	jmp 0FFFFh:0

	cli
	hlt
;
; void _cdecl asm_disk_reset(byte drive);
;
global _asm_disk_reset
_asm_disk_reset:

	push bp
    mov bp, sp

	mov dl, [bp + 4]
	xor ah, ah
	stc
	int INT_DISK

	mov ax, 1
	sbb ax, 0

    pop bx
	mov sp, bp
	pop bp
	ret

;
; void _cdecl asm_disk_read(byte drive,
;                           word cylinder,
;                           word head,
;                           word sector,
;                           byte count,
;                           byte far* rp_data);
;
global _asm_disk_read
_asm_disk_read:

	push bp
    mov bp, sp

    push bx
    push es

	mov dl, [bp + 4]

	mov ch, [bp + 6]		; Cylinder lower 8 bits
	mov cl, [bp + 7]		; Cylinder bits 6-7
	shl cl, 6

	mov dh, [bp + 10]		; Head number

	mov al, [bp + 8]
	and al, 3Fh
	or cl, al				; Sector bits 0-5

	mov al, [bp + 12]		; Count

	mov bx, [bp + 16]		; es:bx far pointer to output
	mov es, bx
	mov bx, [bp + 14]

	mov al, 02h
	stc
	int INT_DISK

	mov ax, 1
	sbb ax, 0

	pop es
	pop bx

    pop bx
	mov sp, bp
	pop bp
	ret


;
; coid _cdecl asm_disk_get_parameters(byte drive,
;                                     byte* rp_dtype,
;                                     word* rp_cylinders,
;                                     word* rp_sectors,
;                                     word* rp_heads);
;
global _asm_disk_get_parameters
_asm_disk_get_parameters:

	push bp
    mov bp, sp

    push es
	push bx
	push si
	push di

	mov al, [bp + 4]
	mov ah, 08h
	xor di, di
	mov es, di
	stc
	int INT_DISK

	mov ax, 1
	sbb ax, 0

	mov si, [bp + 6]
	mov [si], bl

	mov bl, ch
	mov bh, cl
	shr bh, 6
	mov si, [bp + 8]
	mov [si], bx
	pop di
	pop es

	xor ch, ch
	and cl, 3Fh
	mov si, [bp + 10]
	mov [si], cx

	mov cl, dh
	mov si, [bp + 12]
	mov [si], cx


	pop di
	pop si
	pop bx
	pop es

	pop bx
	mov sp, bp
	pop bp
	ret




