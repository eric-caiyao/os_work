[bits 32]

global switch_to

switch_to:
	
	mov eax,[esp + 4] ;; from
	mov ebx,[esp + 8] ;; to

	push ebp
	push edx
	push edi
	push esi

	;; save old
	mov [eax],esp
	;; recovery new
	mov esp,ebx
	
	pop esi
	pop edi
	pop edx
	pop ebp

	ret
