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

extern _prints


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

global __U4M
__U4M:
    shl edx, 16         ; dx to upper half of edx
    mov dx, ax          ; m1 in edx
    mov eax, edx        ; m1 in eax

    shl ecx, 16         ; cx to upper half of ecx
    mov cx, bx          ; m2 in ecx

    mul ecx             ; result in edx:eax (we only need eax)
    mov edx, eax        ; move upper half to dx
    shr edx, 16

    ret

global _x86_div64_32
_x86_div64_32:

    ; make new call frame
    push bp             ; save old call frame
    mov bp, sp          ; initialize new call frame

    push bx

    ; divide upper 32 bits
    mov eax, [bp + 8]   ; eax <- upper 32 bits of dividend
    mov ecx, [bp + 12]  ; ecx <- divisor
    xor edx, edx
    div ecx             ; eax - quot, edx - remainder

    ; store upper 32 bits of quotient
    mov bx, [bp + 16]
    mov [bx + 4], eax

    ; divide lower 32 bits
    mov eax, [bp + 4]   ; eax <- lower 32 bits of dividend
                        ; edx <- old remainder
    div ecx

    ; store results
    mov [bx], eax
    mov bx, [bp + 18]
    mov [bx], edx

    pop bx

    ; restore old call frame
    mov sp, bp
    pop bp
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
; Waits for user to press key then
;		continues the process
;
global _asm_key_pause
_asm_key_pause:
    mov ah, 0
	int INT_KEYBOARD

    ret
;
; void _cdecl asm_disk_reset(byte drive);
;
global _asm_disk_reset
_asm_disk_reset:

	; make new call frame
    push bp             ; save old call frame
    mov bp, sp          ; initialize new call frame

    mov ah, 0
    mov dl, [bp + 4]    ; dl - drive
    stc
    int 13h

    mov ax, 1
    sbb ax, 0           ; 1 on success, 0 on fail   

    ; restore old call frame
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

	; make new call frame
    push bp             ; save old call frame
    mov bp, sp          ; initialize new call frame

    ; save modified regs
    push bx
    push es

    ; setup args
    mov dl, [bp + 4]    ; dl - drive

    mov ch, [bp + 6]    ; ch - cylinder (lower 8 bits)
    mov cl, [bp + 7]    ; cl - cylinder to bits 6-7
    shl cl, 6
    
    mov al, [bp + 8]    ; cl - sector to bits 0-5
    and al, 3Fh
    or cl, al

    mov dh, [bp + 10]   ; dh - head

    mov al, [bp + 12]   ; al - count

    mov bx, [bp + 16]   ; es:bx - far pointer to data out
    mov es, bx
    mov bx, [bp + 14]

    ; call int13h
    mov ah, 02h
    stc
    int 13h
    push ASM_READ_ERROR
    call _prints

    ; set return value
    mov ax, 1
    sbb ax, 0           ; 1 on success, 0 on fail   

    ; restore regs
    pop es
    pop bx

    ; restore old call frame
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

	; make new call frame
    push bp             ; save old call frame
    mov bp, sp          ; initialize new call frame

    ; save regs
    push es
    push bx
    push si
    push di

    ; call int13h
    mov dl, [bp + 4]    ; dl - disk drive
    mov ah, 08h
    mov di, 0           ; es:di - 0000:0000
    mov es, di
    stc
    int 13h

    ; return
    mov ax, 1
    sbb ax, 0

    ; out params
    mov si, [bp + 6]    ; drive type from bl
    mov [si], bl

    mov bl, ch          ; cylinders - lower bits in ch
    mov bh, cl          ; cylinders - upper bits in cl (6-7)
    shr bh, 6
    mov si, [bp + 8]
    mov [si], bx

    xor ch, ch          ; sectors - lower 5 bits in cl
    and cl, 3Fh
    mov si, [bp + 10]
    mov [si], cx

    mov cl, dh          ; heads - dh
    mov si, [bp + 12]
    mov [si], cx

    ; restore regs
    pop di
    pop si
    pop bx
    pop es

    ; restore old call frame
    mov sp, bp
    pop bp
    ret


ASM_READ_ERROR: db "An error occured while reading from disk", 0xA, 0xD, 0


