	.file	"modeset.c"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"/dev/dri/card0"
.LC1:
	.string	"using card '%s'\n"
.LC2:
	.string	"cannot open '%s': %m\n"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC3:
	.string	"drm device '%s' does not support dumb buffers\n"
	.align 8
.LC4:
	.string	"cannot retrieve DRM resources (%d): %m\n"
	.align 8
.LC5:
	.string	"cannot retrieve DRM connector %u:%u (%d): %m\n"
	.section	.rodata.str1.1
.LC6:
	.string	"ignoring unused connector %u\n"
	.section	.rodata.str1.8
	.align 8
.LC7:
	.string	"no valid mode for connector %u\n"
	.align 8
.LC8:
	.string	"mode for connector %u is %ux%u\n"
	.align 8
.LC9:
	.string	"cannot retrieve encoder %u:%u (%d): %m\n"
	.align 8
.LC10:
	.string	"cannot find suitable CRTC for connector %u\n"
	.align 8
.LC11:
	.string	"no valid crtc for connector %u\n"
	.align 8
.LC12:
	.string	"cannot create dumb buffer (%d): %m\n"
	.align 8
.LC13:
	.string	"cannot create framebuffer (%d): %m\n"
	.align 8
.LC14:
	.string	"cannot map dumb buffer (%d): %m\n"
	.align 8
.LC15:
	.string	"cannot mmap dumb buffer (%d): %m\n"
	.align 8
.LC16:
	.string	"cannot create framebuffer for connector %u\n"
	.align 8
.LC17:
	.string	"cannot setup device for connector %u:%u (%d): %m\n"
	.align 8
.LC18:
	.string	"cannot set CRTC for connector %u (%d): %m\n"
	.section	.rodata.str1.1
.LC19:
	.string	"/dev/input/mouse0"
.LC20:
	.string	"drawing"
	.section	.rodata.str1.8
	.align 8
.LC21:
	.string	"modeset failed with error %d: %m\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB26:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$152, %rsp
	.cfi_def_cfa_offset 208
	movq	%fs:40, %rax
	movq	%rax, 136(%rsp)
	xorl	%eax, %eax
	leaq	.LC0(%rip), %rbx
	cmpl	$1, %edi
	jle	.L2
	movq	8(%rsi), %rbx
.L2:
	movq	stderr(%rip), %r15
	movq	%rbx, %rdx
	leaq	.LC1(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	$524290, %esi
	movq	%rbx, %rdi
	movl	$0, %eax
	call	open@PLT
	movl	%eax, %r14d
	testl	%eax, %eax
	js	.L100
	leaq	96(%rsp), %rdx
	movl	$1, %esi
	movl	%eax, %edi
	call	drmGetCap@PLT
	testl	%eax, %eax
	js	.L6
	cmpq	$0, 96(%rsp)
	je	.L6
.L5:
	movl	%r14d, %edi
	call	drmModeGetResources@PLT
	movq	%rax, %r13
	testq	%rax, %rax
	je	.L8
	movl	$0, 48(%rsp)
	cmpl	$0, 32(%rax)
	je	.L10
	leaq	96(%rsp), %rax
	movq	%rax, 24(%rsp)
	leaq	76(%rsp), %rax
	movq	%rax, 40(%rsp)
	leaq	80(%rsp), %rax
	movq	%rax, 32(%rsp)
	movl	%r14d, 12(%rsp)
	jmp	.L9
.L100:
	call	__errno_location@PLT
	movl	(%rax), %r12d
	movl	%r12d, %ebp
	negl	%ebp
	movq	%rbx, %rdx
	leaq	.LC2(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	testl	%r12d, %r12d
	je	.L5
.L4:
	call	__errno_location@PLT
	movl	%ebp, %edx
	negl	%edx
	movl	%edx, (%rax)
	leaq	.LC21(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	%ebp, %eax
	movq	136(%rsp), %rsi
	xorq	%fs:40, %rsi
	jne	.L101
	addq	$152, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L6:
	.cfi_restore_state
	movq	%rbx, %rdx
	leaq	.LC3(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	%r14d, %edi
	call	close@PLT
	movl	$-95, %ebp
	jmp	.L4
.L8:
	call	__errno_location@PLT
	movq	%rax, %rbx
	movl	(%rax), %edx
	leaq	.LC4(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	(%rbx), %eax
	movl	%eax, %ebp
	negl	%ebp
	testl	%eax, %eax
	je	.L12
	movl	%r14d, %edi
	call	close@PLT
	jmp	.L4
.L108:
	call	__errno_location@PLT
	movq	40(%r13), %rdx
	movq	16(%rsp), %rsi
	movl	(%rdx,%rsi,4), %ecx
	movl	(%rax), %r8d
	movl	48(%rsp), %edx
	leaq	.LC5(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	jmp	.L14
.L109:
	leaq	.LC6(%rip), %rsi
	movq	%r15, %rdi
	call	fprintf@PLT
	jmp	.L16
.L110:
	leaq	.LC7(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	$-14, %r12d
	jmp	.L18
.L111:
	movl	12(%rsp), %edi
	call	drmModeGetEncoder@PLT
	testq	%rax, %rax
	je	.L19
	movl	8(%rax), %r12d
	testl	%r12d, %r12d
	je	.L20
	movq	modeset_list(%rip), %rdx
	testq	%rdx, %rdx
	je	.L21
	cmpl	%r12d, 116(%rdx)
	je	.L20
.L22:
	movq	(%rdx), %rdx
	testq	%rdx, %rdx
	je	.L21
	cmpl	116(%rdx), %r12d
	jne	.L22
.L20:
	movq	%rax, %rdi
	call	drmModeFreeEncoder@PLT
	jmp	.L19
.L21:
	testl	%r12d, %r12d
	js	.L20
	movq	%rax, %rdi
	call	drmModeFreeEncoder@PLT
	movl	%r12d, 116(%rbx)
	jmp	.L24
.L26:
	call	__errno_location@PLT
	movq	80(%rbp), %rdx
	movl	(%rdx,%r14,4), %ecx
	movl	(%rax), %r8d
	movl	%r12d, %edx
	leaq	.LC9(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	jmp	.L29
.L31:
	testl	%r14d, %r14d
	jns	.L102
.L30:
	leaq	1(%rcx), %rdx
	cmpq	%r8, %rcx
	je	.L28
	movq	%rdx, %rcx
.L34:
	movl	%esi, %edx
	sall	%cl, %edx
	testl	%r9d, %edx
	je	.L30
	movq	24(%r13), %rdx
	movl	(%rdx,%rcx,4), %r14d
	testq	%rax, %rax
	je	.L31
	cmpl	116(%rax), %r14d
	je	.L30
	movq	%rax, %rdx
.L32:
	movq	(%rdx), %rdx
	testq	%rdx, %rdx
	je	.L31
	cmpl	116(%rdx), %r14d
	jne	.L32
	jmp	.L30
.L102:
	call	drmModeFreeEncoder@PLT
	movl	%r14d, 116(%rbx)
.L24:
	movq	24(%rsp), %rdx
	movq	$0, 12(%rdx)
	movq	$0, 20(%rdx)
	movl	$0, 28(%rdx)
	movl	8(%rbx), %eax
	movl	%eax, 100(%rsp)
	movl	12(%rbx), %eax
	movl	%eax, 96(%rsp)
	movl	$32, 104(%rsp)
	movl	$3223348402, %esi
	movl	12(%rsp), %edi
	call	drmIoctl@PLT
	testl	%eax, %eax
	js	.L103
	movl	116(%rsp), %r9d
	movl	%r9d, 16(%rbx)
	movq	120(%rsp), %rax
	movl	%eax, 20(%rbx)
	movl	112(%rsp), %eax
	movl	%eax, 24(%rbx)
	movl	12(%rbx), %edx
	movl	8(%rbx), %esi
	leaq	108(%rbx), %rcx
	pushq	%rcx
	.cfi_def_cfa_offset 216
	pushq	%rax
	.cfi_def_cfa_offset 224
	movl	$32, %r8d
	movl	$24, %ecx
	movl	28(%rsp), %edi
	call	drmModeAddFB@PLT
	addq	$16, %rsp
	.cfi_def_cfa_offset 208
	testl	%eax, %eax
	jne	.L104
	movq	32(%rsp), %rdx
	movq	$0, 4(%rdx)
	movl	$0, 12(%rdx)
	movl	24(%rbx), %eax
	movl	%eax, 80(%rsp)
	movl	$3222299827, %esi
	movl	12(%rsp), %edi
	call	drmIoctl@PLT
	testl	%eax, %eax
	jne	.L105
	movl	20(%rbx), %esi
	movq	88(%rsp), %r9
	movl	12(%rsp), %r8d
	movl	$1, %ecx
	movl	$3, %edx
	movl	$0, %edi
	call	mmap@PLT
	movq	%rax, 32(%rbx)
	cmpq	$-1, %rax
	je	.L106
	movl	20(%rbx), %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	jmp	.L42
.L28:
	call	drmModeFreeEncoder@PLT
.L29:
	addl	$1, %r12d
	cmpl	72(%rbp), %r12d
	jnb	.L25
.L35:
	movl	%r12d, %r14d
	movq	80(%rbp), %rax
	movl	(%rax,%r14,4), %esi
	movl	12(%rsp), %edi
	call	drmModeGetEncoder@PLT
	movq	%rax, %rdi
	testq	%rax, %rax
	je	.L26
	movl	16(%r13), %edx
	testl	%edx, %edx
	je	.L28
	movl	12(%rax), %r9d
	movq	modeset_list(%rip), %rax
	leal	-1(%rdx), %r8d
	movl	$0, %ecx
	movl	$1, %esi
	jmp	.L34
.L25:
	movl	0(%rbp), %edx
	leaq	.LC10(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	0(%rbp), %edx
	leaq	.LC11(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
.L16:
	movq	%rbx, %rdi
	call	free@PLT
	movq	%rbp, %rdi
	call	drmModeFreeConnector@PLT
.L14:
	addl	$1, 48(%rsp)
	movl	48(%rsp), %eax
	cmpl	32(%r13), %eax
	jnb	.L107
.L9:
	movl	48(%rsp), %eax
	movq	%rax, %rcx
	movq	%rax, 16(%rsp)
	movq	40(%r13), %rax
	movl	(%rax,%rcx,4), %esi
	movl	12(%rsp), %edi
	call	drmModeGetConnector@PLT
	movq	%rax, %rbp
	testq	%rax, %rax
	je	.L108
	movl	$128, %edi
	call	malloc@PLT
	movq	%rax, %rbx
	movl	$16, %ecx
	movl	$0, %eax
	movq	%rbx, %rdi
	rep stosq
	movl	0(%rbp), %edx
	movl	%edx, 112(%rbx)
	cmpl	$1, 16(%rbp)
	jne	.L109
	cmpl	$0, 32(%rbp)
	je	.L110
	movq	40(%rbp), %rax
	movdqu	(%rax), %xmm0
	movups	%xmm0, 40(%rbx)
	movdqu	16(%rax), %xmm1
	movups	%xmm1, 56(%rbx)
	movdqu	32(%rax), %xmm2
	movups	%xmm2, 72(%rbx)
	movdqu	48(%rax), %xmm3
	movups	%xmm3, 88(%rbx)
	movl	64(%rax), %eax
	movl	%eax, 104(%rbx)
	movq	40(%rbp), %rax
	movzwl	4(%rax), %ecx
	movl	%ecx, 8(%rbx)
	movq	40(%rbp), %rax
	movzwl	14(%rax), %r8d
	movl	%r8d, 12(%rbx)
	movl	0(%rbp), %edx
	leaq	.LC8(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	4(%rbp), %esi
	testl	%esi, %esi
	jne	.L111
.L19:
	cmpl	$0, 72(%rbp)
	je	.L25
	movl	$0, %r12d
	jmp	.L35
.L103:
	call	__errno_location@PLT
	movq	%rax, %r12
	movl	(%rax), %edx
	leaq	.LC12(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	(%r12), %r12d
	negl	%r12d
	jmp	.L36
.L104:
	call	__errno_location@PLT
	movq	%rax, %r12
	movl	(%rax), %edx
	leaq	.LC13(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	(%r12), %r12d
	negl	%r12d
.L38:
	movl	24(%rbx), %eax
	movl	%eax, 76(%rsp)
	movq	40(%rsp), %rdx
	movl	$3221513396, %esi
	movl	12(%rsp), %edi
	call	drmIoctl@PLT
.L36:
	testl	%r12d, %r12d
	jne	.L112
.L42:
	movq	%rbp, %rdi
	call	drmModeFreeConnector@PLT
	movq	modeset_list(%rip), %rax
	movq	%rax, (%rbx)
	movq	%rbx, modeset_list(%rip)
	jmp	.L14
.L105:
	call	__errno_location@PLT
	movq	%rax, %r12
	movl	(%rax), %edx
	leaq	.LC14(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	(%r12), %r12d
	negl	%r12d
.L40:
	movl	108(%rbx), %esi
	movl	12(%rsp), %edi
	call	drmModeRmFB@PLT
	jmp	.L38
.L106:
	call	__errno_location@PLT
	movq	%rax, %r12
	movl	(%rax), %edx
	leaq	.LC15(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	(%r12), %r12d
	negl	%r12d
	jmp	.L40
.L112:
	movl	0(%rbp), %edx
	leaq	.LC16(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	cmpl	$-2, %r12d
	je	.L16
.L18:
	call	__errno_location@PLT
	movl	%r12d, %r8d
	negl	%r8d
	movl	%r8d, (%rax)
	movq	40(%r13), %rax
	movq	16(%rsp), %rsi
	movl	(%rax,%rsi,4), %ecx
	movl	48(%rsp), %edx
	leaq	.LC17(%rip), %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	jmp	.L16
.L107:
	movl	12(%rsp), %r14d
.L10:
	movq	%r13, %rdi
	call	drmModeFreeResources@PLT
.L12:
	movq	modeset_list(%rip), %rbx
	leaq	.LC18(%rip), %rbp
	jmp	.L43
.L114:
	call	__errno_location@PLT
	movl	112(%rbx), %edx
	movl	(%rax), %ecx
	movq	%rbp, %rsi
	movq	%r15, %rdi
	movl	$0, %eax
	call	fprintf@PLT
.L44:
	movq	(%rbx), %rbx
.L43:
	testq	%rbx, %rbx
	je	.L113
	movl	116(%rbx), %esi
	movl	%r14d, %edi
	call	drmModeGetCrtc@PLT
	movq	%rax, 120(%rbx)
	movl	108(%rbx), %edx
	movl	116(%rbx), %esi
	leaq	40(%rbx), %rax
	pushq	%rax
	.cfi_def_cfa_offset 216
	pushq	$1
	.cfi_def_cfa_offset 224
	leaq	112(%rbx), %r9
	movl	$0, %r8d
	movl	$0, %ecx
	movl	%r14d, %edi
	call	drmModeSetCrtc@PLT
	addq	$16, %rsp
	.cfi_def_cfa_offset 208
	testl	%eax, %eax
	je	.L44
	jmp	.L114
.L113:
	movl	$0, %r9d
	movl	$0, %r8d
	movl	$33, %ecx
	movl	$1, %edx
	movl	$4, %esi
	movl	$0, %edi
	call	mmap@PLT
	movl	$0, %edx
	movl	$0, %esi
	leaq	.LC19(%rip), %rdi
	movl	$0, %eax
	call	open@PLT
	movl	%eax, 16(%rsp)
	movl	$8, %edx
	leaq	.LC20(%rip), %rsi
	movl	$1, %edi
	call	write@PLT
	leaq	134(%rsp), %rax
	movq	%rax, 24(%rsp)
	movl	$0, %ebp
	movl	56(%rsp), %r13d
	movl	60(%rsp), %r12d
.L61:
	addl	$1, %ebx
	movl	$3, %edx
	movq	24(%rsp), %rsi
	movl	16(%rsp), %edi
	call	read@PLT
	movsbl	135(%rsp), %eax
	addl	%eax, %r13d
	movsbl	136(%rsp), %eax
	subl	%eax, %r12d
	cmpb	$9, 134(%rsp)
	je	.L115
.L46:
	movq	modeset_list(%rip), %rcx
	testq	%rcx, %rcx
	je	.L61
	movl	$-858993459, %edx
	movl	%ebx, %eax
	mull	%edx
	shrl	$4, %edx
	sall	$8, %edx
	movl	%edx, 12(%rsp)
	leal	-24(%r13), %r10d
	leal	24(%r13), %r9d
	leal	24(%r12), %r8d
	leal	-24(%r12), %eax
	movl	%eax, 48(%rsp)
	jmp	.L60
.L115:
	leal	-91(%r13), %eax
	cmpl	$408, %eax
	ja	.L46
	leal	-91(%r12), %eax
	cmpl	$108, %eax
	jbe	.L116
	leal	-311(%r12), %eax
	cmpl	$88, %eax
	jbe	.L117
	leal	-511(%r12), %eax
	movl	$410, %r15d
	cmpl	$88, %eax
	jbe	.L53
	jmp	.L46
.L119:
	subl	$1, %edx
	je	.L118
.L47:
	movl	$110, %eax
.L49:
	subl	$1, %eax
	jne	.L49
	jmp	.L119
.L116:
	movl	$410, %edx
	jmp	.L47
.L121:
	subl	$1, %edx
	je	.L120
.L50:
	movl	$90, %eax
.L52:
	subl	$1, %eax
	jne	.L52
	jmp	.L121
.L117:
	movl	$410, %edx
	jmp	.L50
.L122:
	subl	$1, %r15d
	je	.L46
.L53:
	movl	$90, %r14d
.L54:
	movl	$60, %edi
	movl	$0, %eax
	call	syscall@PLT
	subl	$1, %r14d
	jne	.L54
	jmp	.L122
.L118:
	movb	$0, 55(%rsp)
	jmp	.L46
.L120:
	movb	$100, 55(%rsp)
	jmp	.L46
.L59:
	addl	$1, %esi
	cmpl	12(%rcx), %esi
	jnb	.L58
.L62:
	movl	$0, %edi
	cmpl	$0, 8(%rcx)
	je	.L59
.L57:
	movl	%esi, %eax
	imull	16(%rcx), %eax
	leal	(%rax,%rdi,4), %r14d
	cmpl	%edi, %r10d
	setb	%al
	movzbl	%al, %eax
	cmpl	%edi, %r9d
	cmovbe	%ebp, %eax
	cmpl	%esi, %r8d
	seta	%dl
	movzbl	%dl, %edx
	imull	%edx, %eax
	cmpl	%esi, 48(%rsp)
	setb	%dl
	movzbl	%dl, %edx
	imull	%edx, %eax
	imull	$90, %eax, %eax
	orl	12(%rsp), %eax
	cmpl	$499, %edi
	setbe	%dl
	movzbl	%dl, %edx
	cmpl	$90, %edi
	cmovbe	%ebp, %edx
	movl	%edx, %r15d
	cmpl	$310, %esi
	seta	%dl
	movzbl	%dl, %edx
	imull	%r15d, %edx
	cmpl	$399, %esi
	setbe	%r15b
	movzbl	%r15b, %r15d
	imull	%r15d, %edx
	imull	$30, %edx, %edx
	addl	%r11d, %edx
	sall	$16, %edx
	orl	%edx, %eax
	movq	32(%rcx), %rdx
	movl	%eax, (%rdx,%r14)
	addl	$1, %edi
	cmpl	8(%rcx), %edi
	jb	.L57
	jmp	.L59
.L58:
	movq	(%rcx), %rcx
	testq	%rcx, %rcx
	je	.L61
.L60:
	movl	$0, %esi
	movsbl	55(%rsp), %r11d
	cmpl	$0, 12(%rcx)
	jne	.L62
	jmp	.L58
.L101:
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE26:
	.size	main, .-main
	.comm	box,16384,32
	.comm	prop,80380352,32
	.local	modeset_list
	.comm	modeset_list,8,8
	.ident	"GCC: (Alpine 8.3.0) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
