bits 16

section _ENTRY class=CODE

%define NEWL 0x0A			;Newline character
%define CRET 0x0D			;Carriage return
%define ENDL 0x0D, 0x0A		;Endline: new line + carriage return
							;
%define NULL 0x0			;Null
							;
							;
							;BIOS INTERRUPT CALLS:
%define INT_VIDEO	 0x10	;All video services
%define INT_DISK	 0x13	;Low level disk services
%define INT_KEYBOARD 0X16	;Keyboard input interrupt
%define INT_RTC		 0x1A	;Real time clock

extern _asm_tt_printc		; Prints string to the teletype display
							; Args: string pointer in stack

extern _asm_key_reboot		; Reboots BIOS

extern _boot_main			; void boot_main(word bootDrive)

extern _prints				; void prints_tt(char* str)





global entry

entry:

	cli						; Using small memory model:
	mov ax, ds				; Take the data segment from sector1
	mov ss, ax				; Move it to the stack segment register

	mov sp, 0				; Set up stack pointer
	mov bp, sp				; Set up base pointer
	sti


	mov dh, 0
	push dx
	call _boot_main			; Call main function

	push str_error_escape	; if: main function returns:
	call _prints			; print error to screen

	;call _asm_key_reboot

	cli
	hlt

str_error_escape: 		db 'An error ocurred in the bootloader, press any key to restart BIOS', ENDL, 0
