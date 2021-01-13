section mbr vstart=0x7c00
mov ax,cs
mov bx,ax
mov dx,ax
mov cx,ax
mov ax,0x0600
mov bx,0x0700
mov cx,0x0
mov dx,0x184f
int 10h
mov ax,0xb800
mov gs,ax
mov byte [gs:0],'1'
mov byte [gs:1],0xa4
mov byte [gs:2],'M'
mov byte [gs:3],0xa4
mov byte [gs:4],'B'
mov byte [gs:5],0xa4
mov byte [gs:6],'R'
mov byte [gs:7],0xa4

mov al,0x2
mov dx,0x1f2
out dx,al
mov al,0x2
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

no_ready:
	mov dx,0x1f7
	in al,dx
	and al,0x88
	cmp al,0x08
	jnz no_ready

mov cx,512
mov bx,0x900
go_on_read:
	mov dx,0x1f0
	in ax,dx
	mov [bx],ax
	add bx,2
	loop go_on_read

jmp 0x900

times 510 - ($ - $$) db 0
db 0x55,0xaa
