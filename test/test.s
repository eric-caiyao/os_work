	.file	"test.c"
	.section	.rodata
.LC0:
	.string	"%s"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	andl	$-16, %esp
	subl	$1056, %esp
	movl	$0, 1052(%esp)
	jmp	.L2
.L3:
	leal	28(%esp), %edx
	movl	1052(%esp), %eax
	addl	%edx, %eax
	movb	$65, (%eax)
	addl	$1, 1052(%esp)
.L2:
	cmpl	$1022, 1052(%esp)
	jle	.L3
	movb	$0, 1051(%esp)
	leal	28(%esp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC0, (%esp)
	call	printf
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-39)"
	.section	.note.GNU-stack,"",@progbits
