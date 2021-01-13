[bits 32]

extern put_str
extern c_version_handle
extern sys_call_handle_table
extern put_char
global interrupt_handle_entry
global system_call_handle_entry

section .data
interrupt_str db "interrupt occur!",0xa,0

section .text
interrupt_handle_entry:
	push 64
	call put_char
	add esp,4
	push ds
	push es
	push fs
	push gs
	pushad
	mov al,0x20
	out 0xa0,al
	out 0x20,al
	push 32 ; vector no
	call c_version_handle
	add esp,4
	popad
	pop gs
	pop fs
	pop es
	pop ds
iret

;; system call interrupt handle entry. support one param
system_call_handle_entry:
	push ds
	push es
	push fs
	push gs
	pushad
	shl eax,2
	add eax,sys_call_handle_table
	mov eax,[eax]
	push ebx
	call eax
	add esp,4
	mov [esp + 7 * 4],eax
	mov al,0x20
	out 0xa0,al
	out 0x20,al
	popad
	pop gs
	pop fs
	pop es
	pop ds
iret
