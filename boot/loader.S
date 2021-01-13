section loader vstart=0x900
[bits 16]
jmp start
GDT:
	dd 0 ;first descriptor low 32
        dd 0 ; first descriptor high 32
        dd 0000000000000000_1111111111111111b
        dd 00000000_1_1_0_0_1111_1_00_1_1000_00000000b
        dd 0000000000000000_1111111111111111b
        dd 00000000_1_1_0_0_1111_1_00_1_0010_00000000b
        ; 1000000000000000 1011
        ; 1000000000000000
        dd 1000000000000000_1000000000000000b
        dd 00000000_1_1_0_0_0000_1_00_1_0010_00001011b
gdt_size equ $ - GDT
gdt_limit equ gdt_size - 1
gdt_ptr:
	dw gdt_limit
	dd GDT

code_selector equ 0000000000001000b
data_selector equ 0000000000010000b
video_selector equ 0000000000011000b


ards_buf times 244 db 0
ards_nr dw 0

total_mem_bytes dd 0

page_entry_table_addr equ 0x100000

test_val dd 0x12345678

start:
mov eax,total_mem_bytes
mov eax,GDT



mov ebx,0x0
mov edx,0x534d4150
mov ax,0x0000
mov es,ax
mov di,ards_buf

.e820_mem_get_loop:
	mov eax,0x0000e820
	mov ecx,20
	int 0x15
	add di,cx
	inc word [ards_nr]
	cmp ebx,0
	jnz  .e820_mem_get_loop

mov edx,0x0
mov ecx,[ards_nr]
mov eax,ards_buf

.find_max_mem:
	mov ebx,[eax]
	add ebx,[eax + 8]
	add eax,20
	cmp edx,ebx
	jge .back_to_find_max_mem
	mov edx,ebx
.back_to_find_max_mem:
	loop .find_max_mem
	mov [total_mem_bytes],edx

; open a20
in al,0x92
or al,00000010b
out 0x92,al

lgdt [gdt_ptr]

mov eax,cr0
or eax,0x00000001
mov cr0,eax

jmp dword code_selector:protected_code


[bits 32]
protected_code:
	mov ax,video_selector
	mov gs,ax
	mov ax,data_selector
	mov ds,ax
	mov ss,ax
	mov es,ax
	mov byte [gs:160],'P'

; loader kernel bin from 9th sector
mov al,0x20 ; 20 sectors
mov dx,0x1f2
out dx,al
mov al,0x9 ; from 9th sector
mov dx,0x1f3
out dx,al
mov al,0x0
mov dx,0x1f4
out dx,al
mov al,0x0
mov dx,0x1f5
out dx,al
mov al,0xe0
mov dx,0x1f6
out dx,al
mov al,0x20
mov dx,0x1f7
out dx,al

.no_ready:
	mov dx,0x1f7
	in al,dx
	and al,0x88
	cmp al,0x08
	jnz .no_ready

mov cx,5120
mov ebx,0x70000 ; kernel.bin start memory addr
.go_on_read:
	mov dx,0x1f0
	in ax,dx
	mov [ebx],ax
	add ebx,2
	loop .go_on_read
; bin copy
; ax = first program header offset
; bx = program header size
; cx = program header count
; each program header
; program physic offset + bin base memory address = program memory address
; program load virtual address
; cur program size
; if this current program header is unvalid just add ax

; init page table
.init_page_table:
	; init low 1M
	mov eax,page_entry_table_addr + 0x1000
	mov ecx,256
	mov ebx,0
	or ebx,100b | 10b | 1b
	.init_low_1M:
		mov [ds:eax],ebx
		add ebx,0x1000
		add eax,4
		loop .init_low_1M
	; init page directory table 
	mov eax,page_entry_table_addr
	add eax,0x1000
	or eax,100b | 10b | 1b
	mov dword [page_entry_table_addr],eax ;; 0 
	mov dword [page_entry_table_addr + 3072],eax ;; low of high 1G
	
		
	;; init high 1G
	mov eax,page_entry_table_addr + 0x2000
	or eax,100b | 10b | 1b
	mov ecx,254
	mov esi,0
	mov ebx,page_entry_table_addr
	add ebx,3076 ; 768 page directory table entry

	.init_high_1G_page_directory_entry:
		mov [ebx+esi*4],eax
		inc esi
		add eax,0x1000
		loop .init_high_1G_page_directory_entry

	mov eax,page_entry_table_addr
	or eax,100b | 10b | 1b
	mov dword [page_entry_table_addr + 4092],eax 

mov ebx,[gdt_ptr + 2]
or dword [ebx + 0x18 + 4],0xc0000000
add dword [gdt_ptr + 2],0xc0000000
mov eax,page_entry_table_addr
mov cr3,eax
mov eax,cr0
or eax,0x80000000
mov cr0,eax
lgdt [gdt_ptr]
mov byte [gs:160],'V'



jmp init_kernel
mem_cpy:
	push ebp
	mov ebp,esp

	push dword eax
	push dword ebx
	push dword ecx
	push dword edx
	
	xor eax,eax
	xor ebx,ebx
	xor ecx,ecx
	xor edx,edx

	mov eax,[ebp + 8] ; to virtual address
	mov ebx,[ebp + 16] ;from memory address
	mov ecx,[ebp + 12] ; size
	
	cld
	mov edi,eax
	mov esi,ebx
	rep movsb
	
	pop edx
	pop ecx
	pop ebx
	pop eax
	pop ebp
ret

init_kernel:
mov esp,0x900
xor eax,eax
xor ebx,ebx
xor ecx,ecx
xor edx,edx
mov ax,[0x70000 + 42] ;program header size 
mov ebx,[0x70000 + 28] ;first program header offset
mov cx,[0x70000 + 44] ;program header count
add ebx,0x70000

each_program_header:
	cmp byte [ebx],0
	je null_segment
	mov edx,[ebx + 4]
	add edx,0x70000 ; start memory address
	push edx
	push dword [ebx + 16]; size
	push dword [ebx + 8] ; to virtual address
	call mem_cpy
	add esp,12 ; clear stack
null_segment:
	add ebx,eax
	loop each_program_header

mov esp,0xc009f000
jmp 0xc0001500

jmp $
