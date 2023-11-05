							;Basic operating system
							;Made by Been
							;
org 0						;Basic directives
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
%define INT_RTC		 0x1A	;Real time clock

start:

	mov si, str_welcome
	call printf
	


.halt:
	cli
	hlt



;
;Prints string in Teletype output
;Arguments:
;	si: Char* to be printed
;
printf:
	push si
	push ax

.loop:
	lodsb
	or al, al
	jz .done

	mov ah, 0x0e
	mov bh, 0
	int INT_VIDEO

	jmp .loop

.done:
	pop ax
	pop si
	ret


; ;
; ;Prints an integer to the display
; ;Arguments:
; ;	ax: Integer
; ;
;
; print_int:
; 	push cx
; 	mov di, buffer
;
; 	xor dx, dx
; 	mov cx, 10000
; 	div cx
; 	jz .thousand
; 	add ax, 48
; 	mov byte [di], al
; 	inc di
; .thousand:
; 	mov ax, dx
; 	xor dx, dx
; 	mov cx, 1000
; 	div cx
; 	jz .hundred
; 	add ax, 48
; 	mov byte [di], al
; 	inc di
; .hundred:
; 	mov ax, dx
; 	xor dx, dx
; 	mov cx, 100
; 	div cx
; 	jz .ten
; 	add ax, 48
; 	mov byte [di], al
; 	inc di
; .ten:
; 	mov ax, dx
; 	xor dx, dx
; 	mov cx, 10
; 	div cx
; 	jz .one
; 	add ax, 48
; 	mov byte [di], al
; 	inc di
; .one:
; 	add dx, 48
; 	mov byte [di], dl
; 	inc di
; 	mov byte [di], NEWL
; 	inc di
; 	mov byte [di], CRET
; 	inc di
; 	mov byte [di], NULL
; 	mov si, buffer
; 	call printf
; 	pop cx
; ;
; ;Clears Teletype output
; ;
; clearscr:
; 	mov cl, 48
; 	mov di, buffer
; .loop:
; 	mov byte [di], NEWL
; 	inc di
;
; 	dec cl
; 	jz .done
; 	jmp .loop
; .done:
; 	mov byte [di], CRET
; 	inc di
; 	mov byte [di], NULL
; 	mov si, buffer
; 	call printf
; 	ret
;
; primes:
; 	mov si, str_primes
; 	call printf
; 	mov ax, 2
;
; .prime:
; 	call print_int
; 	int INT_KEYBOARD
; .number:
; 	mov cx, ax
; 	inc ax
; .divisor:
; 	cmp cx, 1
; 	je .prime		;PRIME NUMBER
;
;
;
; 	call print_int
;
;
; 	ret



;Strings:
str_welcome: 		db 'Welcome to KERNEL!', ENDL, 0
;str_primes:			db 'Press any key to generate the next prime', ENDL, 0

str_buffer: 		resb 50
