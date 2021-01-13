[bits 32]
section .text
global put_char
global put_str
global put_int
global put_int32
global put_hex
global put_hex_new
global put_32
global setcur

int_32_cache dq 0
put_32:
	push eax
	push ebx
	push ecx
	push edx
	mov eax,[esp + 20]
	mov ebx,0xF0000000
	mov ecx,8
	handle_one_hex:
	mov edx,eax
	and edx,ebx
	push ecx
	sub ecx,1
	shl cl,2
	shr edx,cl
	pop ecx
	push edx
	call put_hex_new
	add esp,4
	shr ebx,4
	loop handle_one_hex
	pop edx
	pop ecx
	pop ebx
	pop eax
	ret
put_hex_new:
	push eax
	mov eax,[esp + 8]
	cmp eax,15
	ja .unvalid_value
	cmp eax,10
	jb .less_than_10
	add eax,0x11 - 0xa
	.less_than_10:
	add eax,0x30
	push eax
	call put_char
	add esp,4
	pop eax
	ret
	.unvalid_value:
	mov eax,'?'
	push eax
	call put_char
	add esp,4
	pop eax
	ret

put_hex:
	push eax
	push ebx
	push ecx
	push edx
	xor eax,eax
	xor ebx,ebx
	xor ecx,ecx
	xor edx,edx

	mov ecx,4 ;; byte count
	mov ebx,[esp + 4 * 5] ;; start address
	_print_loop:

	xor eax,eax
	mov ah,[ebx]
	and ax,0xF000
	shr ax,12
	cmp ax,10
	jl _low_10_1
	sub ax,10
	add ax,'a'
	sub ax,'0'
	_low_10_1:
	add ax,'0'
	push eax
	call put_char
	add esp,4
	
	xor eax,eax
	mov ah,[ebx]
	and ax,0x0F00
	shr ax,8
	cmp ax,10
	jl _low_10_2
	sub ax,9
	add ax,'a'
	sub ax,'0'
	_low_10_2:
	add ax,'0'
	push eax
	call put_char
	add esp,4

	add ebx,1

	loop _print_loop

	pop edx
	pop ecx
	pop ebx
	pop eax
	ret
put_int32:
	mov ebx,[esp + 4]
	xor eax,eax
	mov al,bl ;; low 8 bit
	shr al,4
	and al,0x0F
	add al,48
	push eax
	call put_char
	add esp,4	

	mov al,bl ;; low 8 bit
	and al,0x0F
	add al,48
	push eax
	call put_char
	add esp,4	

	mov al,bh ;; second 8 bit
	shr al,4
	and al,0x0F
	add al,48
	push eax
	call put_char
	add esp,4

	mov al,bh ;; second 8 bit
	and al,0x0F
	add al,48
	push eax
	call put_char
	add esp,4

	shr ebx,16

	mov al,bl ;; thrid 8 bit
	shr al,4
	and al,0x0f
	add al,48
	push eax
	call put_char
	add esp,4

	mov al,bl ;; thrid 8 bit
	and al,0x0f
	add al,48
	push eax
	call put_char
	add esp,4

	mov al,bh ;; forth 8 bit
	shr al,4
	and al,0x0f
	add al,48
	push eax
	call put_char
	add esp,4

	mov al,bh ;; forth 8 bit
	and al,0x0f
	add al,48
	push eax
	call put_char
	add esp,4

	ret
put_int:
	mov eax,int_32_cache
	mov ebx,[esp + 4]
	.loop_div:
		cmp ebx,0
		je .end_div
		push ebx
		push 10
		call .div_32
		add esp,8
		mov ebx,edi
		add dx,48
		mov [eax],dl
		inc eax
		loop .loop_div
	.end_div:
		add dx,48
		mov [eax],dx
	mov eax,[int_32_cache]
	push eax
	call put_char
	add esp,4
	ret

; return edi & dx 
.div_32:
	push eax
	push ebx
	push ecx

	mov eax,[esp + 20] ;diver
	mov ebx,[esp + 16] ;dived
	
	mov cx,ax ; backup dived low 16
	shr eax,16
	mov dx,0
	div bx
	mov di,ax ; store high 16
	mov ax,cx
	;mov dx,0
	div bx
	shl edi,16 ;shift to high 16
	mov di,ax  ; set low 16
	
	pop ecx
	pop ebx
	pop eax
	ret
put_str:
	push ebx
	push ecx
	xor ecx,ecx
	mov ebx,[esp + 12]
	.put_one_char:
		mov cl,[ebx]
		cmp cl,0
		je .str_end
		push ecx
		call put_char
		add esp,4
		inc ebx
	loop .put_one_char
	.str_end:
	pop ecx
	pop ebx
	ret

put_char:
	cli
	pushad
	mov dx,0x03d4
	mov al,0x0e
	out dx,al
	mov dx,0x03d5
	in al,dx 
	mov ah,al
	mov dx,0x03d4
	mov al,0x0f
	out dx,al
	mov dx,0x03d5
	in al,dx
	mov bx,ax

	mov eax,[esp + 36]
	cmp al,0x08
	je .backspace
	cmp al,0x0a
	je .lf
	cmp al,0x0d
	je .cr

.write_char:
	shl bx,1
	mov byte [gs:bx],al
	inc bx
	mov byte [gs:bx],0x07
	shr bx,1
	inc bx
	cmp bx,1999
	ja .scroll
	jmp setcur

.backspace:
	mov al,0x20
	shl bx,1
	mov byte [gs:bx],al
	inc bx
	mov byte [gs:bx],0x07
	shr bx,1
	jmp setcur

.lf:
.cr:
	mov ax,bx
	mov dh,80
	div dh
	shr ax,8
	sub bx,ax
	add bx,80
	cmp bx,1999
	ja .scroll
	jmp setcur

.scroll:
	cld
	mov edi,0
	add edi,0xb8000
	mov esi,160
	add esi,0xb8000
	mov ecx,3840
	rep movsb
	mov ecx,160
	mov bx,3840
	.clear_row: 
		mov byte [gs:bx],0x20
		inc bx
		mov byte [gs:bx],0x07
		inc bx
	loop .clear_row
	mov bx,1920
setcur:
	mov dx,0x03d4
	mov al,0x0e
	out dx,al
	mov dx,0x03d5
	mov al,bh
	out dx,al

	mov dx,0x03d4
	mov al,0x0f
	out dx,al
	mov dx,0x03d5
	mov al,bl
	out dx,al
	popad
	sti
ret

