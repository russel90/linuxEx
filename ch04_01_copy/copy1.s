	.file	"copy1.c"
	.section	.rodata
.LC0:
	.string	"--help"
.LC1:
	.string	"%s old-file new-file\n"
.LC2:
	.string	"opening file %s"
.LC3:
	.string	"couldn't write whole buffer"
.LC4:
	.string	"read"
.LC5:
	.string	"close input"
.LC6:
	.string	"close output"
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$1088, %rsp
	movl	%edi, -1076(%rbp)
	movq	%rsi, -1088(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	cmpl	$3, -1076(%rbp)
	jne	.L2
	movq	-1088(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movl	$.LC0, %esi
	movq	%rax, %rdi
	call	strcmp
	testl	%eax, %eax
	jne	.L3
.L2:
	movq	-1088(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rsi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	usageErr
.L3:
	movq	-1088(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	open
	movl	%eax, -1064(%rbp)
	cmpl	$-1, -1064(%rbp)
	jne	.L4
	movq	-1088(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rsi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	errExit
.L4:
	movl	$577, -1060(%rbp)
	movl	$438, -1056(%rbp)
	movq	-1088(%rbp), %rax
	addq	$16, %rax
	movq	(%rax), %rax
	movl	-1056(%rbp), %edx
	movl	-1060(%rbp), %ecx
	movl	%ecx, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	open
	movl	%eax, -1052(%rbp)
	cmpl	$-1, -1052(%rbp)
	jne	.L6
	movq	-1088(%rbp), %rax
	addq	$16, %rax
	movq	(%rax), %rax
	movq	%rax, %rsi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	errExit
.L7:
	movq	-1048(%rbp), %rdx
	leaq	-1040(%rbp), %rcx
	movl	-1052(%rbp), %eax
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	write
	cmpq	-1048(%rbp), %rax
	je	.L6
	movl	$.LC3, %edi
	movl	$0, %eax
	call	fatal
.L6:
	leaq	-1040(%rbp), %rcx
	movl	-1064(%rbp), %eax
	movl	$1024, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	read
	movq	%rax, -1048(%rbp)
	cmpq	$0, -1048(%rbp)
	jg	.L7
	cmpq	$-1, -1048(%rbp)
	jne	.L8
	movl	$.LC4, %edi
	movl	$0, %eax
	call	errExit
.L8:
	movl	-1064(%rbp), %eax
	movl	%eax, %edi
	call	close
	cmpl	$-1, %eax
	jne	.L9
	movl	$.LC5, %edi
	movl	$0, %eax
	call	errExit
.L9:
	movl	-1052(%rbp), %eax
	movl	%eax, %edi
	call	close
	cmpl	$-1, %eax
	jne	.L10
	movl	$.LC6, %edi
	movl	$0, %eax
	call	errExit
.L10:
	movl	$0, %edi
	call	exit
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
