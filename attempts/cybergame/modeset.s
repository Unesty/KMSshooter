	.file	"modeset.c"
	.text
.Ltext0:
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
.LC22:
	.string	"modeset failed with error %d: %m\n"
	.text
	.globl	main
	.type	main, @function
main:
.LVL0:
.LFB29:
	.file 1 "modeset.c"
	.loc 1 546 1 view -0
	.cfi_startproc
	.loc 1 546 1 is_stmt 0 view .LVU1
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
	subq	$136, %rsp
	.cfi_def_cfa_offset 192
	.loc 1 546 1 view .LVU2
	movq	%fs:40, %rax
	movq	%rax, 120(%rsp)
	xorl	%eax, %eax
	.loc 1 547 2 is_stmt 1 view .LVU3
	.loc 1 548 2 view .LVU4
	.loc 1 549 2 view .LVU5
	.loc 1 552 2 view .LVU6
	.loc 1 555 8 is_stmt 0 view .LVU7
	leaq	.LC0(%rip), %rbx
	.loc 1 552 5 view .LVU8
	cmpl	$1, %edi
	jle	.L2
	.loc 1 553 3 is_stmt 1 view .LVU9
	.loc 1 553 8 is_stmt 0 view .LVU10
	movq	8(%rsi), %rbx
.LVL1:
.L2:
	.loc 1 557 2 is_stmt 1 view .LVU11
	movq	%rbx, %rdx
	leaq	.LC1(%rip), %rsi
.LVL2:
	.loc 1 557 2 is_stmt 0 view .LVU12
	movq	stderr(%rip), %rdi
.LVL3:
	.loc 1 557 2 view .LVU13
	movl	$0, %eax
	call	fprintf@PLT
.LVL4:
	.loc 1 560 2 is_stmt 1 view .LVU14
.LBB20:
.LBI20:
	.loc 1 83 12 view .LVU15
.LBB21:
	.loc 1 85 2 view .LVU16
	.loc 1 86 2 view .LVU17
	.loc 1 88 2 view .LVU18
	.loc 1 88 7 is_stmt 0 view .LVU19
	movl	$524290, %esi
	movq	%rbx, %rdi
	movl	$0, %eax
	call	open64@PLT
.LVL5:
	movl	%eax, %r14d
.LVL6:
	.loc 1 89 2 is_stmt 1 view .LVU20
	.loc 1 89 5 is_stmt 0 view .LVU21
	testl	%eax, %eax
	js	.L95
	.loc 1 95 2 is_stmt 1 view .LVU22
	.loc 1 95 6 is_stmt 0 view .LVU23
	leaq	80(%rsp), %rdx
	movl	$1, %esi
	movl	%eax, %edi
	call	drmGetCap@PLT
.LVL7:
	.loc 1 95 5 view .LVU24
	testl	%eax, %eax
	js	.L6
	.loc 1 95 56 view .LVU25
	cmpq	$0, 80(%rsp)
	je	.L6
.LVL8:
.L5:
	.loc 1 95 56 view .LVU26
.LBE21:
.LBE20:
	.loc 1 565 2 is_stmt 1 view .LVU27
.LBB25:
.LBI25:
	.loc 1 190 12 view .LVU28
.LBB26:
	.loc 1 192 2 view .LVU29
	.loc 1 193 2 view .LVU30
	.loc 1 194 2 view .LVU31
	.loc 1 195 2 view .LVU32
	.loc 1 196 2 view .LVU33
	.loc 1 199 2 view .LVU34
	.loc 1 199 8 is_stmt 0 view .LVU35
	movl	%r14d, %edi
	call	drmModeGetResources@PLT
.LVL9:
	movq	%rax, %r15
.LVL10:
	.loc 1 200 2 is_stmt 1 view .LVU36
	.loc 1 200 5 is_stmt 0 view .LVU37
	testq	%rax, %rax
	je	.L8
.LVL11:
	.loc 1 207 9 view .LVU38
	movl	$0, 4(%rsp)
	.loc 1 207 2 view .LVU39
	cmpl	$0, 32(%rax)
	je	.L10
.LBB27:
.LBB28:
.LBB29:
.LBB30:
	.loc 1 451 2 view .LVU40
	leaq	80(%rsp), %rax
.LVL12:
	.loc 1 451 2 view .LVU41
	movq	%rax, 16(%rsp)
	.loc 1 506 2 view .LVU42
	leaq	60(%rsp), %rax
	movq	%rax, 32(%rsp)
	.loc 1 476 2 view .LVU43
	leaq	64(%rsp), %rax
	movq	%rax, 24(%rsp)
	jmp	.L9
.LVL13:
.L95:
	.loc 1 476 2 view .LVU44
.LBE30:
.LBE29:
.LBE28:
.LBE27:
.LBE26:
.LBE25:
.LBB55:
.LBB22:
	.loc 1 90 3 is_stmt 1 view .LVU45
	.loc 1 90 10 is_stmt 0 view .LVU46
	call	__errno_location@PLT
.LVL14:
	.loc 1 90 10 view .LVU47
	movl	(%rax), %r12d
	.loc 1 90 7 view .LVU48
	movl	%r12d, %ebp
	negl	%ebp
.LVL15:
	.loc 1 91 3 is_stmt 1 view .LVU49
	movq	%rbx, %rdx
	leaq	.LC2(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL16:
	.loc 1 92 3 view .LVU50
	.loc 1 92 3 is_stmt 0 view .LVU51
.LBE22:
.LBE55:
	.loc 1 561 2 is_stmt 1 view .LVU52
	.loc 1 561 5 is_stmt 0 view .LVU53
	testl	%r12d, %r12d
	je	.L5
.LVL17:
.L4:
	.loc 1 590 2 is_stmt 1 view .LVU54
	.loc 1 591 3 view .LVU55
	call	__errno_location@PLT
.LVL18:
	.loc 1 591 11 is_stmt 0 view .LVU56
	movl	%ebp, %edx
	negl	%edx
	.loc 1 591 9 view .LVU57
	movl	%edx, (%rax)
	.loc 1 592 3 is_stmt 1 view .LVU58
	leaq	.LC22(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL19:
	.loc 1 596 2 view .LVU59
	.loc 1 597 1 is_stmt 0 view .LVU60
	movl	%ebp, %eax
	movq	120(%rsp), %rcx
	xorq	%fs:40, %rcx
	jne	.L96
	addq	$136, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
.LVL20:
	.loc 1 597 1 view .LVU61
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.LVL21:
.L6:
	.cfi_restore_state
.LBB56:
.LBB23:
	.loc 1 97 3 is_stmt 1 view .LVU62
	movq	%rbx, %rdx
	leaq	.LC3(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL22:
	.loc 1 99 3 view .LVU63
	movl	%r14d, %edi
	call	close@PLT
.LVL23:
	.loc 1 100 3 view .LVU64
	.loc 1 100 3 is_stmt 0 view .LVU65
.LBE23:
.LBE56:
	.loc 1 561 2 is_stmt 1 view .LVU66
.LBB57:
.LBB24:
	.loc 1 100 10 is_stmt 0 view .LVU67
	movl	$-95, %ebp
	jmp	.L4
.LVL24:
.L8:
	.loc 1 100 10 view .LVU68
.LBE24:
.LBE57:
.LBB58:
.LBB53:
	.loc 1 201 3 is_stmt 1 view .LVU69
	.loc 1 202 4 is_stmt 0 view .LVU70
	call	__errno_location@PLT
.LVL25:
	.loc 1 202 4 view .LVU71
	movq	%rax, %rbx
.LVL26:
	.loc 1 201 3 view .LVU72
	movl	(%rax), %edx
	leaq	.LC4(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL27:
	.loc 1 203 3 is_stmt 1 view .LVU73
	.loc 1 203 11 is_stmt 0 view .LVU74
	movl	(%rbx), %eax
	.loc 1 203 10 view .LVU75
	movl	%eax, %ebp
	negl	%ebp
.LVL28:
	.loc 1 203 10 view .LVU76
.LBE53:
.LBE58:
	.loc 1 566 2 is_stmt 1 view .LVU77
	.loc 1 566 5 is_stmt 0 view .LVU78
	testl	%eax, %eax
	je	.L12
	.loc 1 567 3 is_stmt 1 view .LVU79
.LDL1:
	.loc 1 588 2 view .LVU80
	movl	%r14d, %edi
	call	close@PLT
.LVL29:
	jmp	.L4
.LVL30:
.L102:
.LBB59:
.LBB54:
	.loc 1 211 4 view .LVU81
	.loc 1 212 28 is_stmt 0 view .LVU82
	call	__errno_location@PLT
.LVL31:
	.loc 1 211 4 view .LVU83
	movq	40(%r15), %rdx
	movq	8(%rsp), %rcx
	movl	(%rdx,%rcx,4), %ecx
	movl	(%rax), %r8d
	movl	4(%rsp), %edx
	leaq	.LC5(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL32:
	.loc 1 213 4 is_stmt 1 view .LVU84
	jmp	.L14
.LVL33:
.L103:
.LBB48:
.LBB43:
	.loc 1 281 3 view .LVU85
	leaq	.LC6(%rip), %rsi
	movq	stderr(%rip), %rdi
	call	fprintf@PLT
.LVL34:
	.loc 1 283 3 view .LVU86
	.loc 1 283 3 is_stmt 0 view .LVU87
.LBE43:
.LBE48:
	.loc 1 223 3 is_stmt 1 view .LVU88
	jmp	.L16
.LVL35:
.L104:
.LBB49:
.LBB44:
	.loc 1 288 3 view .LVU89
	leaq	.LC7(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL36:
	.loc 1 290 3 view .LVU90
	.loc 1 290 3 is_stmt 0 view .LVU91
.LBE44:
.LBE49:
	.loc 1 223 3 is_stmt 1 view .LVU92
.LBB50:
.LBB45:
	.loc 1 290 10 is_stmt 0 view .LVU93
	movl	$-14, %r12d
	jmp	.L18
.LVL37:
.L105:
.LBB34:
.LBB35:
	.loc 1 351 3 is_stmt 1 view .LVU94
	.loc 1 351 9 is_stmt 0 view .LVU95
	movl	%r14d, %edi
	call	drmModeGetEncoder@PLT
.LVL38:
	.loc 1 355 2 is_stmt 1 view .LVU96
	.loc 1 355 5 is_stmt 0 view .LVU97
	testq	%rax, %rax
	je	.L19
	.loc 1 356 3 is_stmt 1 view .LVU98
	.loc 1 356 10 is_stmt 0 view .LVU99
	movl	8(%rax), %r12d
	.loc 1 356 6 view .LVU100
	testl	%r12d, %r12d
	je	.L20
	.loc 1 357 4 is_stmt 1 view .LVU101
.LVL39:
	.loc 1 358 4 view .LVU102
	.loc 1 358 14 is_stmt 0 view .LVU103
	movq	modeset_list(%rip), %rdx
.LVL40:
	.loc 1 358 4 view .LVU104
	testq	%rdx, %rdx
	je	.L21
	.loc 1 359 5 is_stmt 1 view .LVU105
	.loc 1 359 8 is_stmt 0 view .LVU106
	cmpl	116(%rdx), %r12d
	je	.L20
.L22:
	.loc 1 358 41 view .LVU107
	movq	(%rdx), %rdx
.LVL41:
	.loc 1 358 4 view .LVU108
	testq	%rdx, %rdx
	je	.L21
	.loc 1 359 5 is_stmt 1 view .LVU109
	.loc 1 359 8 is_stmt 0 view .LVU110
	cmpl	116(%rdx), %r12d
	jne	.L22
.LVL42:
.L20:
	.loc 1 372 3 is_stmt 1 view .LVU111
	movq	%rax, %rdi
	call	drmModeFreeEncoder@PLT
.LVL43:
	.loc 1 372 3 is_stmt 0 view .LVU112
	jmp	.L19
.LVL44:
.L21:
	.loc 1 365 4 is_stmt 1 view .LVU113
	.loc 1 365 7 is_stmt 0 view .LVU114
	testl	%r12d, %r12d
	js	.L20
	.loc 1 366 5 is_stmt 1 view .LVU115
	movq	%rax, %rdi
	call	drmModeFreeEncoder@PLT
.LVL45:
	.loc 1 367 5 view .LVU116
	.loc 1 367 15 is_stmt 0 view .LVU117
	movl	%r12d, 116(%rbx)
	.loc 1 368 5 is_stmt 1 view .LVU118
	jmp	.L24
.LVL46:
.L26:
	.loc 1 382 4 view .LVU119
	.loc 1 383 27 is_stmt 0 view .LVU120
	call	__errno_location@PLT
.LVL47:
	.loc 1 382 4 view .LVU121
	movq	80(%rbp), %rdx
	movl	(%rdx,%r13,4), %ecx
	movl	(%rax), %r8d
	movl	%r12d, %edx
	leaq	.LC9(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL48:
	.loc 1 384 4 is_stmt 1 view .LVU122
	jmp	.L29
.LVL49:
.L31:
	.loc 1 403 4 view .LVU123
	.loc 1 403 7 is_stmt 0 view .LVU124
	testl	%r13d, %r13d
	jns	.L97
.LVL50:
.L30:
	.loc 1 403 7 view .LVU125
	leaq	1(%rcx), %rdx
	.loc 1 388 3 view .LVU126
	cmpq	%rcx, %r8
	je	.L28
	.loc 1 388 3 view .LVU127
	movq	%rdx, %rcx
.LVL51:
.L34:
	.loc 1 390 4 is_stmt 1 view .LVU128
	.loc 1 390 35 is_stmt 0 view .LVU129
	movl	%esi, %edx
	sall	%cl, %edx
	.loc 1 390 7 view .LVU130
	testl	%r9d, %edx
	je	.L30
	.loc 1 394 4 is_stmt 1 view .LVU131
	.loc 1 394 21 is_stmt 0 view .LVU132
	movq	24(%r15), %rdx
	movl	(%rdx,%rcx,4), %r13d
.LVL52:
	.loc 1 395 4 is_stmt 1 view .LVU133
	.loc 1 395 4 is_stmt 0 view .LVU134
	testq	%rax, %rax
	je	.L31
	.loc 1 396 5 is_stmt 1 view .LVU135
	.loc 1 396 8 is_stmt 0 view .LVU136
	cmpl	116(%rax), %r13d
	je	.L30
	.loc 1 395 14 view .LVU137
	movq	%rax, %rdx
.LVL53:
.L32:
	.loc 1 395 41 view .LVU138
	movq	(%rdx), %rdx
.LVL54:
	.loc 1 395 4 view .LVU139
	testq	%rdx, %rdx
	je	.L31
	.loc 1 396 5 is_stmt 1 view .LVU140
	.loc 1 396 8 is_stmt 0 view .LVU141
	cmpl	116(%rdx), %r13d
	jne	.L32
	jmp	.L30
.LVL55:
.L97:
	.loc 1 404 5 is_stmt 1 view .LVU142
	call	drmModeFreeEncoder@PLT
.LVL56:
	.loc 1 405 5 view .LVU143
	.loc 1 405 15 is_stmt 0 view .LVU144
	movl	%r13d, 116(%rbx)
	.loc 1 406 5 is_stmt 1 view .LVU145
.LVL57:
.L24:
	.loc 1 406 5 is_stmt 0 view .LVU146
.LBE35:
.LBE34:
	.loc 1 302 2 is_stmt 1 view .LVU147
	.loc 1 309 2 view .LVU148
.LBB38:
.LBI29:
	.loc 1 443 12 view .LVU149
.LBB31:
	.loc 1 445 2 view .LVU150
	.loc 1 446 2 view .LVU151
	.loc 1 447 2 view .LVU152
	.loc 1 448 2 view .LVU153
	.loc 1 451 2 view .LVU154
	movq	16(%rsp), %rdx
	movq	$0, 12(%rdx)
	movq	$0, 20(%rdx)
	movl	$0, 28(%rdx)
	.loc 1 452 2 view .LVU155
	.loc 1 452 13 is_stmt 0 view .LVU156
	movl	8(%rbx), %eax
	movl	%eax, 84(%rsp)
	.loc 1 453 2 is_stmt 1 view .LVU157
	.loc 1 453 14 is_stmt 0 view .LVU158
	movl	12(%rbx), %eax
	movl	%eax, 80(%rsp)
	.loc 1 454 2 is_stmt 1 view .LVU159
	.loc 1 454 11 is_stmt 0 view .LVU160
	movl	$32, 88(%rsp)
	.loc 1 455 2 is_stmt 1 view .LVU161
	.loc 1 455 8 is_stmt 0 view .LVU162
	movl	$3223348402, %esi
	movl	%r14d, %edi
	call	drmIoctl@PLT
.LVL58:
	.loc 1 456 2 is_stmt 1 view .LVU163
	.loc 1 456 5 is_stmt 0 view .LVU164
	testl	%eax, %eax
	js	.L98
	.loc 1 461 2 is_stmt 1 view .LVU165
	.loc 1 461 20 is_stmt 0 view .LVU166
	movl	100(%rsp), %r9d
	.loc 1 461 14 view .LVU167
	movl	%r9d, 16(%rbx)
	.loc 1 462 2 is_stmt 1 view .LVU168
	.loc 1 462 12 is_stmt 0 view .LVU169
	movq	104(%rsp), %rax
.LVL59:
	.loc 1 462 12 view .LVU170
	movl	%eax, 20(%rbx)
	.loc 1 463 2 is_stmt 1 view .LVU171
	.loc 1 463 20 is_stmt 0 view .LVU172
	movl	96(%rsp), %eax
	.loc 1 463 14 view .LVU173
	movl	%eax, 24(%rbx)
	.loc 1 466 2 is_stmt 1 view .LVU174
	.loc 1 466 8 is_stmt 0 view .LVU175
	movl	12(%rbx), %edx
	movl	8(%rbx), %esi
	leaq	108(%rbx), %rcx
	pushq	%rcx
	.cfi_def_cfa_offset 200
	pushq	%rax
	.cfi_def_cfa_offset 208
	movl	$32, %r8d
	movl	$24, %ecx
	movl	%r14d, %edi
	call	drmModeAddFB@PLT
.LVL60:
	.loc 1 468 2 is_stmt 1 view .LVU176
	.loc 1 468 5 is_stmt 0 view .LVU177
	addq	$16, %rsp
	.cfi_def_cfa_offset 192
	testl	%eax, %eax
	jne	.L99
	.loc 1 476 2 is_stmt 1 view .LVU178
	movq	24(%rsp), %rdx
	movq	$0, 4(%rdx)
	movl	$0, 12(%rdx)
	.loc 1 477 2 view .LVU179
	.loc 1 477 14 is_stmt 0 view .LVU180
	movl	24(%rbx), %eax
.LVL61:
	.loc 1 477 14 view .LVU181
	movl	%eax, 64(%rsp)
	.loc 1 478 2 is_stmt 1 view .LVU182
	.loc 1 478 8 is_stmt 0 view .LVU183
	movl	$3222299827, %esi
	movl	%r14d, %edi
	call	drmIoctl@PLT
.LVL62:
	.loc 1 479 2 is_stmt 1 view .LVU184
	.loc 1 479 5 is_stmt 0 view .LVU185
	testl	%eax, %eax
	jne	.L100
	.loc 1 487 2 is_stmt 1 view .LVU186
	.loc 1 487 13 is_stmt 0 view .LVU187
	movl	20(%rbx), %esi
	movq	72(%rsp), %r9
	movl	%r14d, %r8d
	movl	$1, %ecx
	movl	$3, %edx
	movl	$0, %edi
	call	mmap64@PLT
.LVL63:
	.loc 1 487 11 view .LVU188
	movq	%rax, 32(%rbx)
	.loc 1 489 2 is_stmt 1 view .LVU189
	.loc 1 489 5 is_stmt 0 view .LVU190
	cmpq	$-1, %rax
	je	.L101
	.loc 1 497 2 is_stmt 1 view .LVU191
	movl	20(%rbx), %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
.LVL64:
	.loc 1 499 2 view .LVU192
	.loc 1 499 2 is_stmt 0 view .LVU193
.LBE31:
.LBE38:
	.loc 1 310 2 is_stmt 1 view .LVU194
	jmp	.L42
.LVL65:
.L28:
.LBB39:
.LBB36:
	.loc 1 410 3 view .LVU195
	call	drmModeFreeEncoder@PLT
.LVL66:
.L29:
	.loc 1 379 40 is_stmt 0 view .LVU196
	addl	$1, %r12d
.LVL67:
	.loc 1 379 2 view .LVU197
	cmpl	72(%rbp), %r12d
	jnb	.L25
.LVL68:
.L35:
	.loc 1 380 3 is_stmt 1 view .LVU198
	.loc 1 380 45 is_stmt 0 view .LVU199
	movl	%r12d, %r13d
	.loc 1 380 9 view .LVU200
	movq	80(%rbp), %rax
	movl	(%rax,%r13,4), %esi
	movl	%r14d, %edi
	call	drmModeGetEncoder@PLT
.LVL69:
	movq	%rax, %rdi
.LVL70:
	.loc 1 381 3 is_stmt 1 view .LVU201
	.loc 1 381 6 is_stmt 0 view .LVU202
	testq	%rax, %rax
	je	.L26
.LVL71:
	.loc 1 388 22 view .LVU203
	movl	16(%r15), %edx
	.loc 1 388 3 view .LVU204
	testl	%edx, %edx
	je	.L28
	.loc 1 390 13 view .LVU205
	movl	12(%rax), %r9d
	.loc 1 395 14 view .LVU206
	movq	modeset_list(%rip), %rax
.LVL72:
	.loc 1 395 14 view .LVU207
	leal	-1(%rdx), %r8d
	movl	$0, %ecx
	.loc 1 390 35 view .LVU208
	movl	$1, %esi
	jmp	.L34
.LVL73:
.L25:
	.loc 1 413 2 is_stmt 1 view .LVU209
	movl	0(%rbp), %edx
	leaq	.LC10(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL74:
	.loc 1 415 2 view .LVU210
	.loc 1 415 2 is_stmt 0 view .LVU211
.LBE36:
.LBE39:
	.loc 1 302 2 is_stmt 1 view .LVU212
	.loc 1 303 3 view .LVU213
	movl	0(%rbp), %edx
	leaq	.LC11(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL75:
	.loc 1 305 3 view .LVU214
	.loc 1 305 3 is_stmt 0 view .LVU215
.LBE45:
.LBE50:
	.loc 1 223 3 is_stmt 1 view .LVU216
	.loc 1 224 4 view .LVU217
.L16:
	.loc 1 229 4 view .LVU218
	movq	%rbx, %rdi
	call	free@PLT
.LVL76:
	.loc 1 230 4 view .LVU219
	movq	%rbp, %rdi
	call	drmModeFreeConnector@PLT
.LVL77:
	.loc 1 231 4 view .LVU220
.L14:
	.loc 1 207 41 is_stmt 0 view .LVU221
	addl	$1, 4(%rsp)
.LVL78:
	.loc 1 207 41 view .LVU222
	movl	4(%rsp), %eax
.LVL79:
	.loc 1 207 2 view .LVU223
	cmpl	32(%r15), %eax
	jnb	.L10
.LVL80:
.L9:
	.loc 1 209 3 is_stmt 1 view .LVU224
	.loc 1 209 49 is_stmt 0 view .LVU225
	movl	4(%rsp), %eax
	movq	%rax, %rcx
	movq	%rax, 8(%rsp)
	.loc 1 209 10 view .LVU226
	movq	40(%r15), %rax
	movl	(%rax,%rcx,4), %esi
	movl	%r14d, %edi
	call	drmModeGetConnector@PLT
.LVL81:
	movq	%rax, %rbp
.LVL82:
	.loc 1 210 3 is_stmt 1 view .LVU227
	.loc 1 210 6 is_stmt 0 view .LVU228
	testq	%rax, %rax
	je	.L102
	.loc 1 217 3 is_stmt 1 view .LVU229
	.loc 1 217 9 is_stmt 0 view .LVU230
	movl	$128, %edi
	call	malloc@PLT
.LVL83:
	.loc 1 217 9 view .LVU231
	movq	%rax, %rbx
.LVL84:
	.loc 1 218 3 is_stmt 1 view .LVU232
	movl	$16, %ecx
	movl	$0, %eax
.LVL85:
	.loc 1 218 3 is_stmt 0 view .LVU233
	movq	%rbx, %rdi
	rep stosq
	.loc 1 219 3 is_stmt 1 view .LVU234
	.loc 1 219 19 is_stmt 0 view .LVU235
	movl	0(%rbp), %edx
	.loc 1 219 13 view .LVU236
	movl	%edx, 112(%rbx)
	.loc 1 222 3 is_stmt 1 view .LVU237
.LVL86:
.LBB51:
.LBI27:
	.loc 1 274 12 view .LVU238
.LBB46:
	.loc 1 277 2 view .LVU239
	.loc 1 280 2 view .LVU240
	.loc 1 280 5 is_stmt 0 view .LVU241
	cmpl	$1, 16(%rbp)
	jne	.L103
	.loc 1 287 2 is_stmt 1 view .LVU242
	.loc 1 287 5 is_stmt 0 view .LVU243
	cmpl	$0, 32(%rbp)
	je	.L104
	.loc 1 294 2 is_stmt 1 view .LVU244
	movq	40(%rbp), %rax
	movdqu	(%rax), %xmm1
	movups	%xmm1, 40(%rbx)
	movdqu	16(%rax), %xmm2
	movups	%xmm2, 56(%rbx)
	movdqu	32(%rax), %xmm3
	movups	%xmm3, 72(%rbx)
	movdqu	48(%rax), %xmm4
	movups	%xmm4, 88(%rbx)
	movl	64(%rax), %eax
	movl	%eax, 104(%rbx)
	.loc 1 295 2 view .LVU245
	.loc 1 295 29 is_stmt 0 view .LVU246
	movq	40(%rbp), %rax
	movzwl	4(%rax), %ecx
	.loc 1 295 13 view .LVU247
	movl	%ecx, 8(%rbx)
	.loc 1 296 2 is_stmt 1 view .LVU248
	.loc 1 296 30 is_stmt 0 view .LVU249
	movq	40(%rbp), %rax
	movzwl	14(%rax), %r8d
	.loc 1 296 14 view .LVU250
	movl	%r8d, 12(%rbx)
	.loc 1 297 2 is_stmt 1 view .LVU251
	movl	0(%rbp), %edx
	leaq	.LC8(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL87:
	.loc 1 301 2 view .LVU252
.LBB40:
.LBI34:
	.loc 1 341 12 view .LVU253
.LBB37:
	.loc 1 344 2 view .LVU254
	.loc 1 345 2 view .LVU255
	.loc 1 346 2 view .LVU256
	.loc 1 347 2 view .LVU257
	.loc 1 350 2 view .LVU258
	.loc 1 350 10 is_stmt 0 view .LVU259
	movl	4(%rbp), %esi
	.loc 1 350 5 view .LVU260
	testl	%esi, %esi
	jne	.L105
.L19:
.LVL88:
	.loc 1 379 2 view .LVU261
	cmpl	$0, 72(%rbp)
	je	.L25
	movl	$0, %r12d
	jmp	.L35
.LVL89:
.L98:
	.loc 1 379 2 view .LVU262
.LBE37:
.LBE40:
.LBB41:
.LBB32:
	.loc 1 457 3 is_stmt 1 view .LVU263
	.loc 1 458 4 is_stmt 0 view .LVU264
	call	__errno_location@PLT
.LVL90:
	.loc 1 458 4 view .LVU265
	movq	%rax, %r12
	.loc 1 457 3 view .LVU266
	movl	(%rax), %edx
	leaq	.LC12(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL91:
	.loc 1 459 3 is_stmt 1 view .LVU267
	.loc 1 459 10 is_stmt 0 view .LVU268
	movl	(%r12), %r12d
	negl	%r12d
	jmp	.L36
.LVL92:
.L99:
	.loc 1 469 3 is_stmt 1 view .LVU269
	.loc 1 470 4 is_stmt 0 view .LVU270
	call	__errno_location@PLT
.LVL93:
	.loc 1 470 4 view .LVU271
	movq	%rax, %r12
	.loc 1 469 3 view .LVU272
	movl	(%rax), %edx
	leaq	.LC13(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL94:
	.loc 1 471 3 is_stmt 1 view .LVU273
	.loc 1 471 7 is_stmt 0 view .LVU274
	movl	(%r12), %r12d
	negl	%r12d
.LVL95:
	.loc 1 472 3 is_stmt 1 view .LVU275
.L38:
	.loc 1 504 2 view .LVU276
	.loc 1 505 2 view .LVU277
	.loc 1 505 14 is_stmt 0 view .LVU278
	movl	24(%rbx), %eax
	movl	%eax, 60(%rsp)
	.loc 1 506 2 is_stmt 1 view .LVU279
	movq	32(%rsp), %rdx
	movl	$3221513396, %esi
	movl	%r14d, %edi
	call	drmIoctl@PLT
.LVL96:
	.loc 1 507 2 view .LVU280
.L36:
	.loc 1 507 2 is_stmt 0 view .LVU281
.LBE32:
.LBE41:
	.loc 1 310 2 is_stmt 1 view .LVU282
	.loc 1 310 5 is_stmt 0 view .LVU283
	testl	%r12d, %r12d
	jne	.L106
.LVL97:
.L42:
	.loc 1 310 5 view .LVU284
.LBE46:
.LBE51:
	.loc 1 223 3 is_stmt 1 view .LVU285
	.loc 1 235 3 view .LVU286
	movq	%rbp, %rdi
	call	drmModeFreeConnector@PLT
.LVL98:
	.loc 1 236 3 view .LVU287
	.loc 1 236 13 is_stmt 0 view .LVU288
	movq	modeset_list(%rip), %rax
	movq	%rax, (%rbx)
	.loc 1 237 3 is_stmt 1 view .LVU289
	.loc 1 237 16 is_stmt 0 view .LVU290
	movq	%rbx, modeset_list(%rip)
	jmp	.L14
.LVL99:
.L100:
.LBB52:
.LBB47:
.LBB42:
.LBB33:
	.loc 1 480 3 is_stmt 1 view .LVU291
	.loc 1 481 4 is_stmt 0 view .LVU292
	call	__errno_location@PLT
.LVL100:
	.loc 1 481 4 view .LVU293
	movq	%rax, %r12
	.loc 1 480 3 view .LVU294
	movl	(%rax), %edx
	leaq	.LC14(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL101:
	.loc 1 482 3 is_stmt 1 view .LVU295
	.loc 1 482 7 is_stmt 0 view .LVU296
	movl	(%r12), %r12d
	negl	%r12d
.LVL102:
	.loc 1 483 3 is_stmt 1 view .LVU297
.L40:
	.loc 1 502 2 view .LVU298
	movl	108(%rbx), %esi
	movl	%r14d, %edi
	call	drmModeRmFB@PLT
.LVL103:
	jmp	.L38
.LVL104:
.L101:
	.loc 1 490 3 view .LVU299
	.loc 1 491 4 is_stmt 0 view .LVU300
	call	__errno_location@PLT
.LVL105:
	movq	%rax, %r12
	.loc 1 490 3 view .LVU301
	movl	(%rax), %edx
	leaq	.LC15(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL106:
	.loc 1 492 3 is_stmt 1 view .LVU302
	.loc 1 492 7 is_stmt 0 view .LVU303
	movl	(%r12), %r12d
	negl	%r12d
.LVL107:
	.loc 1 493 3 is_stmt 1 view .LVU304
	jmp	.L40
.LVL108:
.L106:
	.loc 1 493 3 is_stmt 0 view .LVU305
.LBE33:
.LBE42:
	.loc 1 311 3 is_stmt 1 view .LVU306
	movl	0(%rbp), %edx
	leaq	.LC16(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL109:
	.loc 1 313 3 view .LVU307
	.loc 1 313 3 is_stmt 0 view .LVU308
.LBE47:
.LBE52:
	.loc 1 223 3 is_stmt 1 view .LVU309
	.loc 1 224 4 view .LVU310
	.loc 1 224 7 is_stmt 0 view .LVU311
	cmpl	$-2, %r12d
	je	.L16
.LVL110:
.L18:
	.loc 1 225 5 is_stmt 1 view .LVU312
	call	__errno_location@PLT
.LVL111:
	.loc 1 225 13 is_stmt 0 view .LVU313
	movl	%r12d, %r8d
	negl	%r8d
	.loc 1 225 11 view .LVU314
	movl	%r8d, (%rax)
	.loc 1 226 5 is_stmt 1 view .LVU315
	movq	40(%r15), %rax
	movq	8(%rsp), %rcx
	movl	(%rax,%rcx,4), %ecx
	movl	4(%rsp), %edx
	leaq	.LC17(%rip), %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL112:
	jmp	.L16
.LVL113:
.L10:
	.loc 1 241 2 view .LVU316
	movq	%r15, %rdi
	call	drmModeFreeResources@PLT
.LVL114:
	.loc 1 242 2 view .LVU317
	.loc 1 242 2 is_stmt 0 view .LVU318
.LBE54:
.LBE59:
	.loc 1 566 2 is_stmt 1 view .LVU319
.L12:
	.loc 1 570 2 view .LVU320
	.loc 1 570 12 is_stmt 0 view .LVU321
	movq	modeset_list(%rip), %rbx
.LVL115:
	.loc 1 575 4 view .LVU322
	leaq	.LC18(%rip), %rbp
	.loc 1 570 2 view .LVU323
	jmp	.L43
.LVL116:
.L108:
	.loc 1 575 4 is_stmt 1 view .LVU324
	.loc 1 576 17 is_stmt 0 view .LVU325
	call	__errno_location@PLT
.LVL117:
	.loc 1 575 4 view .LVU326
	movl	112(%rbx), %edx
	movl	(%rax), %ecx
	movq	%rbp, %rsi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf@PLT
.LVL118:
.L44:
	.loc 1 570 39 discriminator 2 view .LVU327
	movq	(%rbx), %rbx
.LVL119:
.L43:
	.loc 1 570 2 discriminator 1 view .LVU328
	testq	%rbx, %rbx
	je	.L107
	.loc 1 571 3 is_stmt 1 view .LVU329
	.loc 1 571 22 is_stmt 0 view .LVU330
	movl	116(%rbx), %esi
	movl	%r14d, %edi
	call	drmModeGetCrtc@PLT
.LVL120:
	.loc 1 571 20 view .LVU331
	movq	%rax, 120(%rbx)
	.loc 1 572 3 is_stmt 1 view .LVU332
	.loc 1 572 9 is_stmt 0 view .LVU333
	movl	108(%rbx), %edx
	movl	116(%rbx), %esi
	.loc 1 573 26 view .LVU334
	leaq	40(%rbx), %rax
	.loc 1 572 9 view .LVU335
	pushq	%rax
	.cfi_def_cfa_offset 200
	pushq	$1
	.cfi_def_cfa_offset 208
	leaq	112(%rbx), %r9
	movl	$0, %r8d
	movl	$0, %ecx
	movl	%r14d, %edi
	call	drmModeSetCrtc@PLT
.LVL121:
	.loc 1 574 3 is_stmt 1 view .LVU336
	.loc 1 574 6 is_stmt 0 view .LVU337
	addq	$16, %rsp
	.cfi_def_cfa_offset 192
	testl	%eax, %eax
	je	.L44
	jmp	.L108
.LVL122:
.L107:
	.loc 1 580 2 is_stmt 1 view .LVU338
.LBB60:
.LBI60:
	.loc 1 655 13 view .LVU339
.LBB61:
	.loc 1 657 2 view .LVU340
	.loc 1 659 2 view .LVU341
	.loc 1 660 2 view .LVU342
	.loc 1 661 2 view .LVU343
	.loc 1 662 2 view .LVU344
	.loc 1 663 2 view .LVU345
	.loc 1 689 2 view .LVU346
	.loc 1 689 7 is_stmt 0 view .LVU347
	movl	$0, %edx
	movl	$0, %esi
	leaq	.LC19(%rip), %rdi
	movl	$0, %eax
	call	open64@PLT
.LVL123:
	movl	%eax, 4(%rsp)
.LVL124:
	.loc 1 700 2 is_stmt 1 view .LVU348
	.loc 1 701 2 view .LVU349
	movl	$8, %edx
	leaq	.LC20(%rip), %rsi
	movl	$1, %edi
	call	write@PLT
.LVL125:
	.loc 1 707 3 is_stmt 0 view .LVU350
	leaq	118(%rsp), %rax
	movq	%rax, 8(%rsp)
	.loc 1 948 69 view .LVU351
	movl	44(%rsp), %r15d
.LVL126:
.L63:
	.loc 1 702 2 is_stmt 1 view .LVU352
	.loc 1 703 3 view .LVU353
	.loc 1 707 3 view .LVU354
	movl	$3, %edx
	movq	8(%rsp), %rsi
	movl	4(%rsp), %edi
	call	read@PLT
.LVL127:
	.loc 1 708 3 view .LVU355
#APP
# 708 "modeset.c" 1
	xor %rax,%rax; mov fede(%rip),%rdi; lea mouse(%rip),%rsi; mov 3,%rdx; syscall
# 0 "" 2
	.loc 1 709 3 view .LVU356
.LVL128:
	.loc 1 710 3 view .LVU357
	.loc 1 711 3 view .LVU358
	.loc 1 711 12 is_stmt 0 view .LVU359
#NO_APP
	movsbl	119(%rsp), %eax
.LVL129:
	.loc 1 711 5 view .LVU360
	addl	%eax, 40(%rsp)
.LVL130:
	.loc 1 712 3 is_stmt 1 view .LVU361
	.loc 1 712 12 is_stmt 0 view .LVU362
	movsbl	120(%rsp), %eax
.LVL131:
	.loc 1 712 5 view .LVU363
	subl	%eax, %r15d
.LVL132:
	.loc 1 881 3 is_stmt 1 view .LVU364
	.loc 1 881 5 is_stmt 0 view .LVU365
	cmpb	$9, 118(%rsp)
	je	.L109
.LVL133:
.L46:
	.loc 1 913 3 is_stmt 1 view .LVU366
	.loc 1 913 13 is_stmt 0 view .LVU367
	movq	modeset_list(%rip), %rbx
.LVL134:
	.loc 1 913 3 view .LVU368
	testq	%rbx, %rbx
	je	.L63
	.loc 1 948 88 view .LVU369
	movl	$-2004318071, %edx
	movl	%r15d, %eax
	mull	%edx
	shrl	$6, %edx
	movl	%edx, %ebp
	jmp	.L62
.LVL135:
.L109:
	.loc 1 882 4 is_stmt 1 view .LVU370
	.loc 1 882 12 is_stmt 0 view .LVU371
	movl	40(%rsp), %eax
.LVL136:
	.loc 1 882 12 view .LVU372
	subl	$91, %eax
	.loc 1 882 6 view .LVU373
	cmpl	$408, %eax
	ja	.L46
	.loc 1 883 5 is_stmt 1 view .LVU374
	.loc 1 883 13 is_stmt 0 view .LVU375
	leal	-91(%r15), %eax
	.loc 1 883 7 view .LVU376
	cmpl	$108, %eax
	jbe	.L110
	.loc 1 891 5 is_stmt 1 view .LVU377
	.loc 1 891 14 is_stmt 0 view .LVU378
	leal	-311(%r15), %eax
	.loc 1 891 7 view .LVU379
	cmpl	$88, %eax
	jbe	.L111
	.loc 1 899 5 is_stmt 1 view .LVU380
	.loc 1 899 14 is_stmt 0 view .LVU381
	leal	-511(%r15), %eax
.LBB62:
	.loc 1 900 14 view .LVU382
	movl	$90, %ebp
.LBB63:
	.loc 1 902 25 view .LVU383
	movl	$0, %r12d
.LBE63:
.LBE62:
	.loc 1 899 7 view .LVU384
	cmpl	$88, %eax
	jbe	.L53
	jmp	.L46
.LVL137:
.L112:
.LBB65:
	.loc 1 884 35 view .LVU385
	addl	$1, %edx
.LVL138:
	.loc 1 884 6 view .LVU386
	cmpl	$500, %edx
	je	.L46
.LVL139:
.L47:
.LBB66:
	.loc 1 885 15 view .LVU387
	movl	$90, %eax
.LVL140:
.L49:
	.loc 1 887 8 is_stmt 1 view .LVU388
	.loc 1 887 35 is_stmt 0 view .LVU389
	movl	%edx, %esi
	imull	8(%rcx), %esi
	.loc 1 887 47 view .LVU390
	leal	(%rsi,%rax), %esi
	.loc 1 887 54 view .LVU391
	movq	32(%rcx), %rdi
	movl	$230400, (%rdi,%rsi)
	.loc 1 885 36 view .LVU392
	addl	$1, %eax
.LVL141:
	.loc 1 885 7 view .LVU393
	cmpl	$200, %eax
	jne	.L49
	jmp	.L112
.LVL142:
.L110:
	.loc 1 885 7 view .LVU394
.LBE66:
	.loc 1 884 14 view .LVU395
	movl	$90, %edx
.LBB67:
	.loc 1 887 25 view .LVU396
	movl	$0, %ecx
	jmp	.L47
.LVL143:
.L113:
	.loc 1 887 25 view .LVU397
.LBE67:
.LBE65:
.LBB68:
	.loc 1 892 35 view .LVU398
	addl	$1, %edx
.LVL144:
	.loc 1 892 6 view .LVU399
	cmpl	$500, %edx
	je	.L46
.LVL145:
.L50:
.LBB69:
	.loc 1 893 15 view .LVU400
	movl	$310, %eax
.LVL146:
.L52:
	.loc 1 895 8 is_stmt 1 view .LVU401
	.loc 1 895 35 is_stmt 0 view .LVU402
	movl	%edx, %esi
	imull	8(%rcx), %esi
	.loc 1 895 47 view .LVU403
	leal	(%rsi,%rax), %esi
	.loc 1 895 54 view .LVU404
	movq	32(%rcx), %rdi
	movl	$58982400, (%rdi,%rsi)
	.loc 1 893 37 view .LVU405
	addl	$1, %eax
.LVL147:
	.loc 1 893 7 view .LVU406
	cmpl	$400, %eax
	jne	.L52
	jmp	.L113
.LVL148:
.L111:
	.loc 1 893 7 view .LVU407
.LBE69:
	.loc 1 892 14 view .LVU408
	movl	$90, %edx
.LBB70:
	.loc 1 895 25 view .LVU409
	movl	$0, %ecx
	jmp	.L50
.LVL149:
.L114:
	.loc 1 895 25 view .LVU410
.LBE70:
.LBE68:
.LBB71:
	.loc 1 900 35 view .LVU411
	addl	$1, %ebp
.LVL150:
	.loc 1 900 6 view .LVU412
	cmpl	$500, %ebp
	je	.L46
.LVL151:
.L53:
.LBB64:
	.loc 1 901 15 view .LVU413
	movl	$510, %ebx
.LVL152:
.L54:
	.loc 1 902 8 is_stmt 1 view .LVU414
	.loc 1 902 35 is_stmt 0 view .LVU415
	movl	%ebp, %eax
	imull	8(%r12), %eax
	.loc 1 902 47 view .LVU416
	leal	(%rax,%rbx), %eax
	.loc 1 902 54 view .LVU417
	movq	32(%r12), %rdx
	movl	$900, (%rdx,%rax)
	.loc 1 903 8 is_stmt 1 view .LVU418
	movl	$60, %edi
	movl	$0, %eax
	call	syscall@PLT
.LVL153:
	.loc 1 901 37 is_stmt 0 view .LVU419
	addl	$1, %ebx
.LVL154:
	.loc 1 901 7 view .LVU420
	cmpl	$600, %ebx
	jne	.L54
	jmp	.L114
.LVL155:
.L61:
	.loc 1 901 7 view .LVU421
.LBE64:
.LBE71:
	.loc 1 914 34 view .LVU422
	addl	$1, %r12d
.LVL156:
	.loc 1 914 4 view .LVU423
	cmpl	12(%rbx), %r12d
	jnb	.L60
.LVL157:
.L64:
	.loc 1 915 12 view .LVU424
	movl	$0, %r13d
	.loc 1 915 5 view .LVU425
	cmpl	$0, 8(%rbx)
	je	.L61
.LVL158:
.L59:
	.loc 1 916 6 is_stmt 1 view .LVU426
	.loc 1 916 25 is_stmt 0 view .LVU427
	movl	%r12d, %eax
	imull	16(%rbx), %eax
	.loc 1 916 10 view .LVU428
	leal	(%rax,%r13,4), %r14d
.LVL159:
	.loc 1 948 6 is_stmt 1 view .LVU429
	.loc 1 948 77 is_stmt 0 view .LVU430
	movl	$-1792967503, %eax
	mull	%r14d
	shrl	$19, %edx
	.loc 1 948 70 view .LVU431
	imull	%ebp, %edx
	pxor	%xmm0, %xmm0
	cvtsi2sdq	%rdx, %xmm0
	call	sin@PLT
.LVL160:
	.loc 1 948 7 view .LVU432
	movl	%r14d, %r14d
	.loc 1 948 34 view .LVU433
	movq	32(%rbx), %rdx
	.loc 1 948 69 view .LVU434
	mulsd	.LC21(%rip), %xmm0
	.loc 1 948 60 view .LVU435
	cvttsd2si	%xmm0, %eax
	.loc 1 948 58 view .LVU436
	orl	$6539264, %eax
	movl	%eax, (%rdx,%r14)
	.loc 1 915 34 view .LVU437
	addl	$1, %r13d
.LVL161:
	.loc 1 915 5 view .LVU438
	cmpl	8(%rbx), %r13d
	jb	.L59
	jmp	.L61
.LVL162:
.L60:
	.loc 1 913 40 view .LVU439
	movq	(%rbx), %rbx
.LVL163:
	.loc 1 913 3 view .LVU440
	testq	%rbx, %rbx
	je	.L63
.L62:
.LVL164:
	.loc 1 914 11 view .LVU441
	movl	$0, %r12d
	.loc 1 914 4 view .LVU442
	cmpl	$0, 12(%rbx)
	jne	.L64
	.loc 1 914 4 view .LVU443
	jmp	.L60
.LVL165:
.L96:
	.loc 1 914 4 view .LVU444
.LBE61:
.LBE60:
	.loc 1 597 1 view .LVU445
	call	__stack_chk_fail@PLT
.LVL166:
	.loc 1 597 1 view .LVU446
	.cfi_endproc
.LFE29:
	.size	main, .-main
	.comm	box,16384,32
	.comm	prop,80380352,32
	.local	modeset_list
	.comm	modeset_list,8,8
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC21:
	.long	0
	.long	1079574528
	.text
.Letext0:
	.file 2 "/usr/include/errno.h"
	.file 3 "/usr/include/bits/types.h"
	.file 4 "/mnt/sda6/usr/lib/gcc/x86_64-pc-linux-gnu/8.2.1/include/stddef.h"
	.file 5 "/usr/include/bits/stdint-intn.h"
	.file 6 "/usr/include/bits/stdint-uintn.h"
	.file 7 "/usr/include/bits/types/struct_FILE.h"
	.file 8 "/usr/include/bits/types/FILE.h"
	.file 9 "/usr/include/stdio.h"
	.file 10 "/usr/include/bits/sys_errlist.h"
	.file 11 "/usr/include/time.h"
	.file 12 "/usr/include/unistd.h"
	.file 13 "/usr/include/bits/getopt_core.h"
	.file 14 "/usr/include/asm-generic/int-ll64.h"
	.file 15 "/usr/include/libdrm/drm_mode.h"
	.file 16 "/usr/include/xf86drmMode.h"
	.file 17 "/usr/include/math.h"
	.file 18 "/usr/include/fcntl.h"
	.file 19 "/usr/include/xf86drm.h"
	.file 20 "/usr/include/sys/mman.h"
	.file 21 "/usr/include/stdlib.h"
	.file 22 "<built-in>"
	.file 23 "/usr/include/bits/mathcalls.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x1a2e
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF215
	.byte	0xc
	.long	.LASF216
	.long	.LASF217
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF0
	.byte	0x2
	.byte	0x2d
	.byte	0xe
	.long	0x39
	.uleb128 0x3
	.byte	0x8
	.long	0x3f
	.uleb128 0x4
	.byte	0x1
	.byte	0x6
	.long	.LASF2
	.uleb128 0x5
	.long	0x3f
	.uleb128 0x2
	.long	.LASF1
	.byte	0x2
	.byte	0x2e
	.byte	0xe
	.long	0x39
	.uleb128 0x6
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x4
	.byte	0x1
	.byte	0x8
	.long	.LASF3
	.uleb128 0x4
	.byte	0x2
	.byte	0x7
	.long	.LASF4
	.uleb128 0x4
	.byte	0x4
	.byte	0x7
	.long	.LASF5
	.uleb128 0x4
	.byte	0x8
	.byte	0x7
	.long	.LASF6
	.uleb128 0x4
	.byte	0x1
	.byte	0x6
	.long	.LASF7
	.uleb128 0x7
	.long	.LASF9
	.byte	0x3
	.byte	0x25
	.byte	0x17
	.long	0x5e
	.uleb128 0x4
	.byte	0x2
	.byte	0x5
	.long	.LASF8
	.uleb128 0x7
	.long	.LASF10
	.byte	0x3
	.byte	0x27
	.byte	0x1c
	.long	0x65
	.uleb128 0x7
	.long	.LASF11
	.byte	0x3
	.byte	0x28
	.byte	0x14
	.long	0x57
	.uleb128 0x7
	.long	.LASF12
	.byte	0x3
	.byte	0x29
	.byte	0x16
	.long	0x6c
	.uleb128 0x4
	.byte	0x8
	.byte	0x5
	.long	.LASF13
	.uleb128 0x7
	.long	.LASF14
	.byte	0x3
	.byte	0x2c
	.byte	0x1b
	.long	0x73
	.uleb128 0x7
	.long	.LASF15
	.byte	0x3
	.byte	0x96
	.byte	0x19
	.long	0xb8
	.uleb128 0x7
	.long	.LASF16
	.byte	0x3
	.byte	0x97
	.byte	0x1b
	.long	0xb8
	.uleb128 0x8
	.byte	0x8
	.uleb128 0x7
	.long	.LASF17
	.byte	0x4
	.byte	0xd8
	.byte	0x17
	.long	0x73
	.uleb128 0x7
	.long	.LASF18
	.byte	0x5
	.byte	0x1a
	.byte	0x13
	.long	0xa0
	.uleb128 0x7
	.long	.LASF19
	.byte	0x6
	.byte	0x18
	.byte	0x13
	.long	0x81
	.uleb128 0x7
	.long	.LASF20
	.byte	0x6
	.byte	0x19
	.byte	0x14
	.long	0x94
	.uleb128 0x7
	.long	.LASF21
	.byte	0x6
	.byte	0x1a
	.byte	0x14
	.long	0xac
	.uleb128 0x7
	.long	.LASF22
	.byte	0x6
	.byte	0x1b
	.byte	0x14
	.long	0xbf
	.uleb128 0x9
	.long	.LASF80
	.byte	0xd8
	.byte	0x7
	.byte	0x31
	.byte	0x8
	.long	0x2b4
	.uleb128 0xa
	.long	.LASF23
	.byte	0x7
	.byte	0x33
	.byte	0x7
	.long	0x57
	.byte	0
	.uleb128 0xa
	.long	.LASF24
	.byte	0x7
	.byte	0x36
	.byte	0x9
	.long	0x39
	.byte	0x8
	.uleb128 0xa
	.long	.LASF25
	.byte	0x7
	.byte	0x37
	.byte	0x9
	.long	0x39
	.byte	0x10
	.uleb128 0xa
	.long	.LASF26
	.byte	0x7
	.byte	0x38
	.byte	0x9
	.long	0x39
	.byte	0x18
	.uleb128 0xa
	.long	.LASF27
	.byte	0x7
	.byte	0x39
	.byte	0x9
	.long	0x39
	.byte	0x20
	.uleb128 0xa
	.long	.LASF28
	.byte	0x7
	.byte	0x3a
	.byte	0x9
	.long	0x39
	.byte	0x28
	.uleb128 0xa
	.long	.LASF29
	.byte	0x7
	.byte	0x3b
	.byte	0x9
	.long	0x39
	.byte	0x30
	.uleb128 0xa
	.long	.LASF30
	.byte	0x7
	.byte	0x3c
	.byte	0x9
	.long	0x39
	.byte	0x38
	.uleb128 0xa
	.long	.LASF31
	.byte	0x7
	.byte	0x3d
	.byte	0x9
	.long	0x39
	.byte	0x40
	.uleb128 0xa
	.long	.LASF32
	.byte	0x7
	.byte	0x40
	.byte	0x9
	.long	0x39
	.byte	0x48
	.uleb128 0xa
	.long	.LASF33
	.byte	0x7
	.byte	0x41
	.byte	0x9
	.long	0x39
	.byte	0x50
	.uleb128 0xa
	.long	.LASF34
	.byte	0x7
	.byte	0x42
	.byte	0x9
	.long	0x39
	.byte	0x58
	.uleb128 0xa
	.long	.LASF35
	.byte	0x7
	.byte	0x44
	.byte	0x16
	.long	0x2cd
	.byte	0x60
	.uleb128 0xa
	.long	.LASF36
	.byte	0x7
	.byte	0x46
	.byte	0x14
	.long	0x2d3
	.byte	0x68
	.uleb128 0xa
	.long	.LASF37
	.byte	0x7
	.byte	0x48
	.byte	0x7
	.long	0x57
	.byte	0x70
	.uleb128 0xa
	.long	.LASF38
	.byte	0x7
	.byte	0x49
	.byte	0x7
	.long	0x57
	.byte	0x74
	.uleb128 0xa
	.long	.LASF39
	.byte	0x7
	.byte	0x4a
	.byte	0xb
	.long	0xcb
	.byte	0x78
	.uleb128 0xa
	.long	.LASF40
	.byte	0x7
	.byte	0x4d
	.byte	0x12
	.long	0x65
	.byte	0x80
	.uleb128 0xa
	.long	.LASF41
	.byte	0x7
	.byte	0x4e
	.byte	0xf
	.long	0x7a
	.byte	0x82
	.uleb128 0xa
	.long	.LASF42
	.byte	0x7
	.byte	0x4f
	.byte	0x8
	.long	0x2d9
	.byte	0x83
	.uleb128 0xa
	.long	.LASF43
	.byte	0x7
	.byte	0x51
	.byte	0xf
	.long	0x2e9
	.byte	0x88
	.uleb128 0xa
	.long	.LASF44
	.byte	0x7
	.byte	0x59
	.byte	0xd
	.long	0xd7
	.byte	0x90
	.uleb128 0xa
	.long	.LASF45
	.byte	0x7
	.byte	0x5b
	.byte	0x17
	.long	0x2f4
	.byte	0x98
	.uleb128 0xa
	.long	.LASF46
	.byte	0x7
	.byte	0x5c
	.byte	0x19
	.long	0x2ff
	.byte	0xa0
	.uleb128 0xa
	.long	.LASF47
	.byte	0x7
	.byte	0x5d
	.byte	0x14
	.long	0x2d3
	.byte	0xa8
	.uleb128 0xa
	.long	.LASF48
	.byte	0x7
	.byte	0x5e
	.byte	0x9
	.long	0xe3
	.byte	0xb0
	.uleb128 0xa
	.long	.LASF49
	.byte	0x7
	.byte	0x5f
	.byte	0xa
	.long	0xe5
	.byte	0xb8
	.uleb128 0xa
	.long	.LASF50
	.byte	0x7
	.byte	0x60
	.byte	0x7
	.long	0x57
	.byte	0xc0
	.uleb128 0xa
	.long	.LASF51
	.byte	0x7
	.byte	0x62
	.byte	0x8
	.long	0x305
	.byte	0xc4
	.byte	0
	.uleb128 0x7
	.long	.LASF52
	.byte	0x8
	.byte	0x7
	.byte	0x19
	.long	0x12d
	.uleb128 0xb
	.long	.LASF218
	.byte	0x7
	.byte	0x2b
	.byte	0xe
	.uleb128 0xc
	.long	.LASF53
	.uleb128 0x3
	.byte	0x8
	.long	0x2c8
	.uleb128 0x3
	.byte	0x8
	.long	0x12d
	.uleb128 0xd
	.long	0x3f
	.long	0x2e9
	.uleb128 0xe
	.long	0x73
	.byte	0
	.byte	0
	.uleb128 0x3
	.byte	0x8
	.long	0x2c0
	.uleb128 0xc
	.long	.LASF54
	.uleb128 0x3
	.byte	0x8
	.long	0x2ef
	.uleb128 0xc
	.long	.LASF55
	.uleb128 0x3
	.byte	0x8
	.long	0x2fa
	.uleb128 0xd
	.long	0x3f
	.long	0x315
	.uleb128 0xe
	.long	0x73
	.byte	0x13
	.byte	0
	.uleb128 0x3
	.byte	0x8
	.long	0x46
	.uleb128 0x5
	.long	0x315
	.uleb128 0x2
	.long	.LASF56
	.byte	0x9
	.byte	0x89
	.byte	0xe
	.long	0x32c
	.uleb128 0x3
	.byte	0x8
	.long	0x2b4
	.uleb128 0x2
	.long	.LASF57
	.byte	0x9
	.byte	0x8a
	.byte	0xe
	.long	0x32c
	.uleb128 0x2
	.long	.LASF58
	.byte	0x9
	.byte	0x8b
	.byte	0xe
	.long	0x32c
	.uleb128 0x2
	.long	.LASF59
	.byte	0xa
	.byte	0x1a
	.byte	0xc
	.long	0x57
	.uleb128 0xd
	.long	0x31b
	.long	0x361
	.uleb128 0xf
	.byte	0
	.uleb128 0x5
	.long	0x356
	.uleb128 0x2
	.long	.LASF60
	.byte	0xa
	.byte	0x1b
	.byte	0x1a
	.long	0x361
	.uleb128 0x2
	.long	.LASF61
	.byte	0xa
	.byte	0x1e
	.byte	0xc
	.long	0x57
	.uleb128 0x2
	.long	.LASF62
	.byte	0xa
	.byte	0x1f
	.byte	0x1a
	.long	0x361
	.uleb128 0x4
	.byte	0x8
	.byte	0x5
	.long	.LASF63
	.uleb128 0x4
	.byte	0x8
	.byte	0x7
	.long	.LASF64
	.uleb128 0xd
	.long	0x3f
	.long	0x3a8
	.uleb128 0xe
	.long	0x73
	.byte	0x1f
	.byte	0
	.uleb128 0xd
	.long	0x39
	.long	0x3b8
	.uleb128 0xe
	.long	0x73
	.byte	0x1
	.byte	0
	.uleb128 0x2
	.long	.LASF65
	.byte	0xb
	.byte	0x9f
	.byte	0xe
	.long	0x3a8
	.uleb128 0x2
	.long	.LASF66
	.byte	0xb
	.byte	0xa0
	.byte	0xc
	.long	0x57
	.uleb128 0x2
	.long	.LASF67
	.byte	0xb
	.byte	0xa1
	.byte	0x11
	.long	0xb8
	.uleb128 0x2
	.long	.LASF68
	.byte	0xb
	.byte	0xa6
	.byte	0xe
	.long	0x3a8
	.uleb128 0x2
	.long	.LASF69
	.byte	0xb
	.byte	0xae
	.byte	0xc
	.long	0x57
	.uleb128 0x2
	.long	.LASF70
	.byte	0xb
	.byte	0xaf
	.byte	0x11
	.long	0xb8
	.uleb128 0x10
	.long	.LASF71
	.byte	0xb
	.value	0x118
	.byte	0xc
	.long	0x57
	.uleb128 0x10
	.long	.LASF72
	.byte	0xc
	.value	0x21f
	.byte	0xf
	.long	0x41a
	.uleb128 0x3
	.byte	0x8
	.long	0x39
	.uleb128 0x10
	.long	.LASF73
	.byte	0xc
	.value	0x221
	.byte	0xf
	.long	0x41a
	.uleb128 0x2
	.long	.LASF74
	.byte	0xd
	.byte	0x24
	.byte	0xe
	.long	0x39
	.uleb128 0x2
	.long	.LASF75
	.byte	0xd
	.byte	0x32
	.byte	0xc
	.long	0x57
	.uleb128 0x2
	.long	.LASF76
	.byte	0xd
	.byte	0x37
	.byte	0xc
	.long	0x57
	.uleb128 0x2
	.long	.LASF77
	.byte	0xd
	.byte	0x3b
	.byte	0xc
	.long	0x57
	.uleb128 0x7
	.long	.LASF78
	.byte	0xe
	.byte	0x1b
	.byte	0x16
	.long	0x6c
	.uleb128 0x7
	.long	.LASF79
	.byte	0xe
	.byte	0x1f
	.byte	0x2a
	.long	0x391
	.uleb128 0x3
	.byte	0x8
	.long	0x57
	.uleb128 0x11
	.long	.LASF81
	.byte	0x20
	.byte	0xf
	.value	0x2c0
	.byte	0x8
	.long	0x4ec
	.uleb128 0x12
	.long	.LASF82
	.byte	0xf
	.value	0x2c1
	.byte	0x8
	.long	0x45d
	.byte	0
	.uleb128 0x12
	.long	.LASF83
	.byte	0xf
	.value	0x2c2
	.byte	0x8
	.long	0x45d
	.byte	0x4
	.uleb128 0x13
	.string	"bpp"
	.byte	0xf
	.value	0x2c3
	.byte	0x8
	.long	0x45d
	.byte	0x8
	.uleb128 0x12
	.long	.LASF84
	.byte	0xf
	.value	0x2c4
	.byte	0x8
	.long	0x45d
	.byte	0xc
	.uleb128 0x12
	.long	.LASF85
	.byte	0xf
	.value	0x2c6
	.byte	0x8
	.long	0x45d
	.byte	0x10
	.uleb128 0x12
	.long	.LASF86
	.byte	0xf
	.value	0x2c7
	.byte	0x8
	.long	0x45d
	.byte	0x14
	.uleb128 0x12
	.long	.LASF87
	.byte	0xf
	.value	0x2c8
	.byte	0x8
	.long	0x469
	.byte	0x18
	.byte	0
	.uleb128 0x11
	.long	.LASF88
	.byte	0x10
	.byte	0xf
	.value	0x2cc
	.byte	0x8
	.long	0x525
	.uleb128 0x12
	.long	.LASF85
	.byte	0xf
	.value	0x2ce
	.byte	0x8
	.long	0x45d
	.byte	0
	.uleb128 0x13
	.string	"pad"
	.byte	0xf
	.value	0x2cf
	.byte	0x8
	.long	0x45d
	.byte	0x4
	.uleb128 0x12
	.long	.LASF89
	.byte	0xf
	.value	0x2d5
	.byte	0x8
	.long	0x469
	.byte	0x8
	.byte	0
	.uleb128 0x11
	.long	.LASF90
	.byte	0x4
	.byte	0xf
	.value	0x2d8
	.byte	0x8
	.long	0x542
	.uleb128 0x12
	.long	.LASF85
	.byte	0xf
	.value	0x2d9
	.byte	0x8
	.long	0x45d
	.byte	0
	.byte	0
	.uleb128 0x9
	.long	.LASF91
	.byte	0x50
	.byte	0x10
	.byte	0xba
	.byte	0x10
	.long	0x5ec
	.uleb128 0xa
	.long	.LASF92
	.byte	0x10
	.byte	0xbc
	.byte	0x6
	.long	0x57
	.byte	0
	.uleb128 0x14
	.string	"fbs"
	.byte	0x10
	.byte	0xbd
	.byte	0xc
	.long	0x5ec
	.byte	0x8
	.uleb128 0xa
	.long	.LASF93
	.byte	0x10
	.byte	0xbf
	.byte	0x6
	.long	0x57
	.byte	0x10
	.uleb128 0xa
	.long	.LASF94
	.byte	0x10
	.byte	0xc0
	.byte	0xc
	.long	0x5ec
	.byte	0x18
	.uleb128 0xa
	.long	.LASF95
	.byte	0x10
	.byte	0xc2
	.byte	0x6
	.long	0x57
	.byte	0x20
	.uleb128 0xa
	.long	.LASF96
	.byte	0x10
	.byte	0xc3
	.byte	0xc
	.long	0x5ec
	.byte	0x28
	.uleb128 0xa
	.long	.LASF97
	.byte	0x10
	.byte	0xc5
	.byte	0x6
	.long	0x57
	.byte	0x30
	.uleb128 0xa
	.long	.LASF98
	.byte	0x10
	.byte	0xc6
	.byte	0xc
	.long	0x5ec
	.byte	0x38
	.uleb128 0xa
	.long	.LASF99
	.byte	0x10
	.byte	0xc8
	.byte	0xb
	.long	0x115
	.byte	0x40
	.uleb128 0xa
	.long	.LASF100
	.byte	0x10
	.byte	0xc8
	.byte	0x16
	.long	0x115
	.byte	0x44
	.uleb128 0xa
	.long	.LASF101
	.byte	0x10
	.byte	0xc9
	.byte	0xb
	.long	0x115
	.byte	0x48
	.uleb128 0xa
	.long	.LASF102
	.byte	0x10
	.byte	0xc9
	.byte	0x17
	.long	0x115
	.byte	0x4c
	.byte	0
	.uleb128 0x3
	.byte	0x8
	.long	0x115
	.uleb128 0x7
	.long	.LASF103
	.byte	0x10
	.byte	0xca
	.byte	0x3
	.long	0x542
	.uleb128 0x9
	.long	.LASF104
	.byte	0x44
	.byte	0x10
	.byte	0xcc
	.byte	0x10
	.long	0x6cf
	.uleb128 0xa
	.long	.LASF105
	.byte	0x10
	.byte	0xcd
	.byte	0xb
	.long	0x115
	.byte	0
	.uleb128 0xa
	.long	.LASF106
	.byte	0x10
	.byte	0xce
	.byte	0xb
	.long	0x109
	.byte	0x4
	.uleb128 0xa
	.long	.LASF107
	.byte	0x10
	.byte	0xce
	.byte	0x15
	.long	0x109
	.byte	0x6
	.uleb128 0xa
	.long	.LASF108
	.byte	0x10
	.byte	0xce
	.byte	0x22
	.long	0x109
	.byte	0x8
	.uleb128 0xa
	.long	.LASF109
	.byte	0x10
	.byte	0xce
	.byte	0x2d
	.long	0x109
	.byte	0xa
	.uleb128 0xa
	.long	.LASF110
	.byte	0x10
	.byte	0xce
	.byte	0x35
	.long	0x109
	.byte	0xc
	.uleb128 0xa
	.long	.LASF111
	.byte	0x10
	.byte	0xcf
	.byte	0xb
	.long	0x109
	.byte	0xe
	.uleb128 0xa
	.long	.LASF112
	.byte	0x10
	.byte	0xcf
	.byte	0x15
	.long	0x109
	.byte	0x10
	.uleb128 0xa
	.long	.LASF113
	.byte	0x10
	.byte	0xcf
	.byte	0x22
	.long	0x109
	.byte	0x12
	.uleb128 0xa
	.long	.LASF114
	.byte	0x10
	.byte	0xcf
	.byte	0x2d
	.long	0x109
	.byte	0x14
	.uleb128 0xa
	.long	.LASF115
	.byte	0x10
	.byte	0xcf
	.byte	0x35
	.long	0x109
	.byte	0x16
	.uleb128 0xa
	.long	.LASF116
	.byte	0x10
	.byte	0xd1
	.byte	0xb
	.long	0x115
	.byte	0x18
	.uleb128 0xa
	.long	.LASF84
	.byte	0x10
	.byte	0xd3
	.byte	0xb
	.long	0x115
	.byte	0x1c
	.uleb128 0xa
	.long	.LASF117
	.byte	0x10
	.byte	0xd4
	.byte	0xb
	.long	0x115
	.byte	0x20
	.uleb128 0xa
	.long	.LASF118
	.byte	0x10
	.byte	0xd5
	.byte	0x7
	.long	0x398
	.byte	0x24
	.byte	0
	.uleb128 0x7
	.long	.LASF119
	.byte	0x10
	.byte	0xd6
	.byte	0x3
	.long	0x5fe
	.uleb128 0x7
	.long	.LASF120
	.byte	0x10
	.byte	0xd6
	.byte	0x15
	.long	0x6e7
	.uleb128 0x3
	.byte	0x8
	.long	0x5fe
	.uleb128 0x3
	.byte	0x8
	.long	0x121
	.uleb128 0x9
	.long	.LASF121
	.byte	0x64
	.byte	0x10
	.byte	0xff
	.byte	0x10
	.long	0x77b
	.uleb128 0x12
	.long	.LASF122
	.byte	0x10
	.value	0x100
	.byte	0xb
	.long	0x115
	.byte	0
	.uleb128 0x12
	.long	.LASF123
	.byte	0x10
	.value	0x101
	.byte	0xb
	.long	0x115
	.byte	0x4
	.uleb128 0x13
	.string	"x"
	.byte	0x10
	.value	0x103
	.byte	0xb
	.long	0x115
	.byte	0x8
	.uleb128 0x13
	.string	"y"
	.byte	0x10
	.value	0x103
	.byte	0xe
	.long	0x115
	.byte	0xc
	.uleb128 0x12
	.long	.LASF83
	.byte	0x10
	.value	0x104
	.byte	0xb
	.long	0x115
	.byte	0x10
	.uleb128 0x12
	.long	.LASF82
	.byte	0x10
	.value	0x104
	.byte	0x12
	.long	0x115
	.byte	0x14
	.uleb128 0x12
	.long	.LASF124
	.byte	0x10
	.value	0x105
	.byte	0x6
	.long	0x57
	.byte	0x18
	.uleb128 0x12
	.long	.LASF125
	.byte	0x10
	.value	0x106
	.byte	0x12
	.long	0x6cf
	.byte	0x1c
	.uleb128 0x12
	.long	.LASF126
	.byte	0x10
	.value	0x108
	.byte	0x6
	.long	0x57
	.byte	0x60
	.byte	0
	.uleb128 0x15
	.long	.LASF127
	.byte	0x10
	.value	0x10a
	.byte	0x3
	.long	0x6f3
	.uleb128 0x11
	.long	.LASF128
	.byte	0x14
	.byte	0x10
	.value	0x10c
	.byte	0x10
	.long	0x7dd
	.uleb128 0x12
	.long	.LASF129
	.byte	0x10
	.value	0x10d
	.byte	0xb
	.long	0x115
	.byte	0
	.uleb128 0x12
	.long	.LASF130
	.byte	0x10
	.value	0x10e
	.byte	0xb
	.long	0x115
	.byte	0x4
	.uleb128 0x12
	.long	.LASF122
	.byte	0x10
	.value	0x10f
	.byte	0xb
	.long	0x115
	.byte	0x8
	.uleb128 0x12
	.long	.LASF131
	.byte	0x10
	.value	0x110
	.byte	0xb
	.long	0x115
	.byte	0xc
	.uleb128 0x12
	.long	.LASF132
	.byte	0x10
	.value	0x111
	.byte	0xb
	.long	0x115
	.byte	0x10
	.byte	0
	.uleb128 0x15
	.long	.LASF133
	.byte	0x10
	.value	0x112
	.byte	0x3
	.long	0x788
	.uleb128 0x16
	.byte	0x7
	.byte	0x4
	.long	0x6c
	.byte	0x10
	.value	0x114
	.byte	0xe
	.long	0x80c
	.uleb128 0x17
	.long	.LASF134
	.byte	0x1
	.uleb128 0x17
	.long	.LASF135
	.byte	0x2
	.uleb128 0x17
	.long	.LASF136
	.byte	0x3
	.byte	0
	.uleb128 0x15
	.long	.LASF137
	.byte	0x10
	.value	0x118
	.byte	0x3
	.long	0x7ea
	.uleb128 0x16
	.byte	0x7
	.byte	0x4
	.long	0x6c
	.byte	0x10
	.value	0x11a
	.byte	0xe
	.long	0x84d
	.uleb128 0x17
	.long	.LASF138
	.byte	0x1
	.uleb128 0x17
	.long	.LASF139
	.byte	0x2
	.uleb128 0x17
	.long	.LASF140
	.byte	0x3
	.uleb128 0x17
	.long	.LASF141
	.byte	0x4
	.uleb128 0x17
	.long	.LASF142
	.byte	0x5
	.uleb128 0x17
	.long	.LASF143
	.byte	0x6
	.byte	0
	.uleb128 0x15
	.long	.LASF144
	.byte	0x10
	.value	0x121
	.byte	0x3
	.long	0x819
	.uleb128 0x11
	.long	.LASF145
	.byte	0x58
	.byte	0x10
	.value	0x123
	.byte	0x10
	.long	0x93b
	.uleb128 0x12
	.long	.LASF146
	.byte	0x10
	.value	0x124
	.byte	0xb
	.long	0x115
	.byte	0
	.uleb128 0x12
	.long	.LASF129
	.byte	0x10
	.value	0x125
	.byte	0xb
	.long	0x115
	.byte	0x4
	.uleb128 0x12
	.long	.LASF147
	.byte	0x10
	.value	0x126
	.byte	0xb
	.long	0x115
	.byte	0x8
	.uleb128 0x12
	.long	.LASF148
	.byte	0x10
	.value	0x127
	.byte	0xb
	.long	0x115
	.byte	0xc
	.uleb128 0x12
	.long	.LASF149
	.byte	0x10
	.value	0x128
	.byte	0x14
	.long	0x80c
	.byte	0x10
	.uleb128 0x12
	.long	.LASF150
	.byte	0x10
	.value	0x129
	.byte	0xb
	.long	0x115
	.byte	0x14
	.uleb128 0x12
	.long	.LASF151
	.byte	0x10
	.value	0x129
	.byte	0x14
	.long	0x115
	.byte	0x18
	.uleb128 0x12
	.long	.LASF152
	.byte	0x10
	.value	0x12a
	.byte	0x12
	.long	0x84d
	.byte	0x1c
	.uleb128 0x12
	.long	.LASF153
	.byte	0x10
	.value	0x12c
	.byte	0x6
	.long	0x57
	.byte	0x20
	.uleb128 0x12
	.long	.LASF154
	.byte	0x10
	.value	0x12d
	.byte	0x15
	.long	0x6db
	.byte	0x28
	.uleb128 0x12
	.long	.LASF155
	.byte	0x10
	.value	0x12f
	.byte	0x6
	.long	0x57
	.byte	0x30
	.uleb128 0x12
	.long	.LASF156
	.byte	0x10
	.value	0x130
	.byte	0xc
	.long	0x5ec
	.byte	0x38
	.uleb128 0x12
	.long	.LASF157
	.byte	0x10
	.value	0x131
	.byte	0xc
	.long	0x6ed
	.byte	0x40
	.uleb128 0x12
	.long	.LASF97
	.byte	0x10
	.value	0x133
	.byte	0x6
	.long	0x57
	.byte	0x48
	.uleb128 0x12
	.long	.LASF98
	.byte	0x10
	.value	0x134
	.byte	0xc
	.long	0x5ec
	.byte	0x50
	.byte	0
	.uleb128 0x15
	.long	.LASF158
	.byte	0x10
	.value	0x135
	.byte	0x3
	.long	0x85a
	.uleb128 0x4
	.byte	0x4
	.byte	0x4
	.long	.LASF159
	.uleb128 0x4
	.byte	0x8
	.byte	0x4
	.long	.LASF160
	.uleb128 0x10
	.long	.LASF161
	.byte	0x11
	.value	0x305
	.byte	0xc
	.long	0x57
	.uleb128 0x9
	.long	.LASF162
	.byte	0x80
	.byte	0x1
	.byte	0x98
	.byte	0x8
	.long	0xa0c
	.uleb128 0xa
	.long	.LASF163
	.byte	0x1
	.byte	0x99
	.byte	0x16
	.long	0xa0c
	.byte	0
	.uleb128 0xa
	.long	.LASF83
	.byte	0x1
	.byte	0x9b
	.byte	0xb
	.long	0x115
	.byte	0x8
	.uleb128 0xa
	.long	.LASF82
	.byte	0x1
	.byte	0x9c
	.byte	0xb
	.long	0x115
	.byte	0xc
	.uleb128 0xa
	.long	.LASF164
	.byte	0x1
	.byte	0x9d
	.byte	0xb
	.long	0x115
	.byte	0x10
	.uleb128 0xa
	.long	.LASF87
	.byte	0x1
	.byte	0x9e
	.byte	0xb
	.long	0x115
	.byte	0x14
	.uleb128 0xa
	.long	.LASF85
	.byte	0x1
	.byte	0x9f
	.byte	0xb
	.long	0x115
	.byte	0x18
	.uleb128 0x14
	.string	"map"
	.byte	0x1
	.byte	0xa0
	.byte	0xb
	.long	0xa12
	.byte	0x20
	.uleb128 0xa
	.long	.LASF125
	.byte	0x1
	.byte	0xa2
	.byte	0x12
	.long	0x6cf
	.byte	0x28
	.uleb128 0x14
	.string	"fb"
	.byte	0x1
	.byte	0xa3
	.byte	0xb
	.long	0x115
	.byte	0x6c
	.uleb128 0xa
	.long	.LASF165
	.byte	0x1
	.byte	0xa4
	.byte	0xb
	.long	0x115
	.byte	0x70
	.uleb128 0xa
	.long	.LASF166
	.byte	0x1
	.byte	0xa5
	.byte	0xb
	.long	0x115
	.byte	0x74
	.uleb128 0xa
	.long	.LASF167
	.byte	0x1
	.byte	0xa6
	.byte	0xf
	.long	0xa18
	.byte	0x78
	.byte	0
	.uleb128 0x3
	.byte	0x8
	.long	0x963
	.uleb128 0x3
	.byte	0x8
	.long	0xfd
	.uleb128 0x3
	.byte	0x8
	.long	0x77b
	.uleb128 0x18
	.long	.LASF219
	.byte	0x1
	.byte	0xa9
	.byte	0x1c
	.long	0xa0c
	.uleb128 0x9
	.byte	0x3
	.quad	modeset_list
	.uleb128 0xd
	.long	0x948
	.long	0xa4c
	.uleb128 0x19
	.long	0x73
	.value	0x2c65
	.uleb128 0x19
	.long	0x73
	.value	0x6e7
	.byte	0
	.uleb128 0x1a
	.long	.LASF168
	.byte	0x1
	.value	0x27e
	.byte	0x7
	.long	0xa34
	.uleb128 0x9
	.byte	0x3
	.quad	prop
	.uleb128 0xd
	.long	0x948
	.long	0xa79
	.uleb128 0xe
	.long	0x73
	.byte	0x3f
	.uleb128 0xe
	.long	0x73
	.byte	0x3f
	.byte	0
	.uleb128 0x1b
	.string	"box"
	.byte	0x1
	.value	0x27f
	.byte	0x7
	.long	0xa63
	.uleb128 0x9
	.byte	0x3
	.quad	box
	.uleb128 0x1c
	.long	.LASF220
	.byte	0x1
	.value	0x3c7
	.byte	0xd
	.long	0xac4
	.uleb128 0x1d
	.string	"fd"
	.byte	0x1
	.value	0x3c7
	.byte	0x21
	.long	0x57
	.uleb128 0x1e
	.long	.LASF169
	.byte	0x1
	.value	0x3c9
	.byte	0x16
	.long	0xa0c
	.uleb128 0x1e
	.long	.LASF170
	.byte	0x1
	.value	0x3ca
	.byte	0x1f
	.long	0x525
	.byte	0
	.uleb128 0x1f
	.long	.LASF221
	.byte	0x1
	.value	0x28f
	.byte	0xd
	.byte	0x1
	.long	0xc00
	.uleb128 0x20
	.string	"r"
	.byte	0x1
	.value	0x291
	.byte	0xa
	.long	0xfd
	.uleb128 0x20
	.string	"g"
	.byte	0x1
	.value	0x291
	.byte	0xd
	.long	0xfd
	.uleb128 0x20
	.string	"b"
	.byte	0x1
	.value	0x291
	.byte	0x10
	.long	0xfd
	.uleb128 0x20
	.string	"i"
	.byte	0x1
	.value	0x293
	.byte	0xf
	.long	0x6c
	.uleb128 0x20
	.string	"j"
	.byte	0x1
	.value	0x293
	.byte	0x12
	.long	0x6c
	.uleb128 0x20
	.string	"k"
	.byte	0x1
	.value	0x293
	.byte	0x15
	.long	0x6c
	.uleb128 0x20
	.string	"off"
	.byte	0x1
	.value	0x293
	.byte	0x18
	.long	0x6c
	.uleb128 0x1e
	.long	.LASF171
	.byte	0x1
	.value	0x293
	.byte	0x1c
	.long	0x6c
	.uleb128 0x20
	.string	"jt"
	.byte	0x1
	.value	0x293
	.byte	0x22
	.long	0x6c
	.uleb128 0x20
	.string	"kt"
	.byte	0x1
	.value	0x293
	.byte	0x25
	.long	0x6c
	.uleb128 0x1e
	.long	.LASF172
	.byte	0x1
	.value	0x293
	.byte	0x28
	.long	0x6c
	.uleb128 0x20
	.string	"mx"
	.byte	0x1
	.value	0x294
	.byte	0x6
	.long	0x57
	.uleb128 0x20
	.string	"my"
	.byte	0x1
	.value	0x294
	.byte	0x9
	.long	0x57
	.uleb128 0x20
	.string	"cx"
	.byte	0x1
	.value	0x295
	.byte	0xf
	.long	0x6c
	.uleb128 0x20
	.string	"cy"
	.byte	0x1
	.value	0x295
	.byte	0x12
	.long	0x6c
	.uleb128 0x1e
	.long	.LASF169
	.byte	0x1
	.value	0x296
	.byte	0x16
	.long	0xa0c
	.uleb128 0x1e
	.long	.LASF173
	.byte	0x1
	.value	0x297
	.byte	0x7
	.long	0xc00
	.uleb128 0x21
	.long	0xbbf
	.uleb128 0x1e
	.long	.LASF174
	.byte	0x1
	.value	0x374
	.byte	0xe
	.long	0x57
	.uleb128 0x22
	.uleb128 0x1e
	.long	.LASF175
	.byte	0x1
	.value	0x375
	.byte	0xf
	.long	0x57
	.byte	0
	.byte	0
	.uleb128 0x21
	.long	0xbe1
	.uleb128 0x1e
	.long	.LASF174
	.byte	0x1
	.value	0x37c
	.byte	0xe
	.long	0x57
	.uleb128 0x22
	.uleb128 0x1e
	.long	.LASF175
	.byte	0x1
	.value	0x37d
	.byte	0xf
	.long	0x57
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0x1e
	.long	.LASF174
	.byte	0x1
	.value	0x384
	.byte	0xe
	.long	0x57
	.uleb128 0x22
	.uleb128 0x1e
	.long	.LASF175
	.byte	0x1
	.value	0x385
	.byte	0xf
	.long	0x57
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0xd
	.long	0x3f
	.long	0xc10
	.uleb128 0xe
	.long	0x73
	.byte	0x1
	.byte	0
	.uleb128 0x23
	.long	.LASF222
	.byte	0x1
	.value	0x221
	.byte	0x5
	.long	0x57
	.quad	.LFB29
	.quad	.LFE29-.LFB29
	.uleb128 0x1
	.byte	0x9c
	.long	0x16f9
	.uleb128 0x24
	.long	.LASF176
	.byte	0x1
	.value	0x221
	.byte	0xe
	.long	0x57
	.long	.LLST0
	.long	.LVUS0
	.uleb128 0x24
	.long	.LASF177
	.byte	0x1
	.value	0x221
	.byte	0x1b
	.long	0x41a
	.long	.LLST1
	.long	.LVUS1
	.uleb128 0x25
	.string	"ret"
	.byte	0x1
	.value	0x223
	.byte	0x6
	.long	0x57
	.long	.LLST2
	.long	.LVUS2
	.uleb128 0x25
	.string	"fd"
	.byte	0x1
	.value	0x223
	.byte	0xb
	.long	0x57
	.long	.LLST3
	.long	.LVUS3
	.uleb128 0x26
	.long	.LASF178
	.byte	0x1
	.value	0x224
	.byte	0xe
	.long	0x315
	.long	.LLST4
	.long	.LVUS4
	.uleb128 0x26
	.long	.LASF169
	.byte	0x1
	.value	0x225
	.byte	0x16
	.long	0xa0c
	.long	.LLST5
	.long	.LVUS5
	.uleb128 0x27
	.long	.LASF181
	.byte	0x1
	.value	0x24d
	.byte	0x1
	.uleb128 0x28
	.long	.LASF223
	.byte	0x1
	.value	0x24b
	.byte	0x1
	.quad	.LDL1
	.uleb128 0x29
	.long	0x18aa
	.quad	.LBI20
	.value	.LVU15
	.long	.Ldebug_ranges0+0
	.byte	0x1
	.value	0x230
	.byte	0x8
	.long	0xdd9
	.uleb128 0x2a
	.long	0x18c7
	.long	.LLST6
	.long	.LVUS6
	.uleb128 0x2a
	.long	0x18bb
	.long	.LLST7
	.long	.LVUS7
	.uleb128 0x2b
	.long	.Ldebug_ranges0+0
	.uleb128 0x2c
	.long	0x18d3
	.long	.LLST8
	.long	.LVUS8
	.uleb128 0x2c
	.long	0x18de
	.long	.LLST9
	.long	.LVUS9
	.uleb128 0x2d
	.long	0x18ea
	.uleb128 0x3
	.byte	0x91
	.sleb128 -112
	.uleb128 0x2e
	.quad	.LVL5
	.long	0x18f7
	.long	0xd48
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x73
	.sleb128 0
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x5
	.byte	0xc
	.long	0x80002
	.byte	0
	.uleb128 0x2e
	.quad	.LVL7
	.long	0x1903
	.long	0xd6c
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x3
	.byte	0x91
	.sleb128 -112
	.byte	0
	.uleb128 0x30
	.quad	.LVL14
	.long	0x1910
	.uleb128 0x2e
	.quad	.LVL16
	.long	0x191c
	.long	0xd9e
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC2
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x73
	.sleb128 0
	.byte	0
	.uleb128 0x2e
	.quad	.LVL22
	.long	0x191c
	.long	0xdc3
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC3
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x73
	.sleb128 0
	.byte	0
	.uleb128 0x31
	.quad	.LVL23
	.long	0x1929
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x29
	.long	0x1853
	.quad	.LBI25
	.value	.LVU28
	.long	.Ldebug_ranges0+0x50
	.byte	0x1
	.value	0x235
	.byte	0x8
	.long	0x13e9
	.uleb128 0x2a
	.long	0x1864
	.long	.LLST10
	.long	.LVUS10
	.uleb128 0x2b
	.long	.Ldebug_ranges0+0x50
	.uleb128 0x2c
	.long	0x186f
	.long	.LLST11
	.long	.LVUS11
	.uleb128 0x2c
	.long	0x187b
	.long	.LLST12
	.long	.LVUS12
	.uleb128 0x2c
	.long	0x1887
	.long	.LLST13
	.long	.LVUS13
	.uleb128 0x2c
	.long	0x1891
	.long	.LLST14
	.long	.LVUS14
	.uleb128 0x2c
	.long	0x189d
	.long	.LLST15
	.long	.LVUS15
	.uleb128 0x32
	.long	0x1800
	.quad	.LBI27
	.value	.LVU238
	.long	.Ldebug_ranges0+0x90
	.byte	0x1
	.byte	0xde
	.byte	0x9
	.long	0x12a6
	.uleb128 0x2a
	.long	0x1838
	.long	.LLST16
	.long	.LVUS16
	.uleb128 0x2a
	.long	0x182b
	.long	.LLST17
	.long	.LVUS17
	.uleb128 0x2a
	.long	0x181e
	.long	.LLST18
	.long	.LVUS18
	.uleb128 0x2a
	.long	0x1812
	.long	.LLST19
	.long	.LVUS19
	.uleb128 0x2b
	.long	.Ldebug_ranges0+0x90
	.uleb128 0x2c
	.long	0x1845
	.long	.LLST20
	.long	.LVUS20
	.uleb128 0x29
	.long	0x16f9
	.quad	.LBI29
	.value	.LVU149
	.long	.Ldebug_ranges0+0x100
	.byte	0x1
	.value	0x135
	.byte	0x8
	.long	0x10c5
	.uleb128 0x2a
	.long	0x1717
	.long	.LLST21
	.long	.LVUS21
	.uleb128 0x2a
	.long	0x170b
	.long	.LLST22
	.long	.LVUS22
	.uleb128 0x2b
	.long	.Ldebug_ranges0+0x100
	.uleb128 0x2d
	.long	0x1724
	.uleb128 0x3
	.byte	0x91
	.sleb128 -112
	.uleb128 0x2d
	.long	0x1731
	.uleb128 0x3
	.byte	0x91
	.sleb128 -132
	.uleb128 0x2d
	.long	0x173e
	.uleb128 0x3
	.byte	0x91
	.sleb128 -128
	.uleb128 0x2c
	.long	0x174b
	.long	.LLST23
	.long	.LVUS23
	.uleb128 0x33
	.long	0x1758
	.quad	.L38
	.uleb128 0x33
	.long	0x1761
	.quad	.L40
	.uleb128 0x2e
	.quad	.LVL58
	.long	0x1936
	.long	0xf4c
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x5
	.byte	0xc
	.long	0xc02064b2
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x4
	.byte	0x91
	.sleb128 -176
	.byte	0x6
	.byte	0
	.uleb128 0x2e
	.quad	.LVL60
	.long	0x1942
	.long	0xf6f
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x1
	.byte	0x48
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x58
	.uleb128 0x2
	.byte	0x8
	.byte	0x20
	.byte	0
	.uleb128 0x2e
	.quad	.LVL62
	.long	0x1936
	.long	0xf98
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x5
	.byte	0xc
	.long	0xc01064b3
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x4
	.byte	0x91
	.sleb128 -168
	.byte	0x6
	.byte	0
	.uleb128 0x2e
	.quad	.LVL63
	.long	0x194f
	.long	0xfbf
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x30
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x1
	.byte	0x33
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x58
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.byte	0
	.uleb128 0x2e
	.quad	.LVL64
	.long	0x195b
	.long	0xfd6
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x1
	.byte	0x30
	.byte	0
	.uleb128 0x30
	.quad	.LVL90
	.long	0x1910
	.uleb128 0x2e
	.quad	.LVL91
	.long	0x191c
	.long	0x1002
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC12
	.byte	0
	.uleb128 0x30
	.quad	.LVL93
	.long	0x1910
	.uleb128 0x2e
	.quad	.LVL94
	.long	0x191c
	.long	0x102e
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC13
	.byte	0
	.uleb128 0x2e
	.quad	.LVL96
	.long	0x1936
	.long	0x1057
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x5
	.byte	0xc
	.long	0xc00464b4
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x4
	.byte	0x91
	.sleb128 -160
	.byte	0x6
	.byte	0
	.uleb128 0x30
	.quad	.LVL100
	.long	0x1910
	.uleb128 0x2e
	.quad	.LVL101
	.long	0x191c
	.long	0x1083
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC14
	.byte	0
	.uleb128 0x2e
	.quad	.LVL103
	.long	0x1966
	.long	0x109b
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.byte	0
	.uleb128 0x30
	.quad	.LVL105
	.long	0x1910
	.uleb128 0x31
	.quad	.LVL106
	.long	0x191c
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC15
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x29
	.long	0x176b
	.quad	.LBI34
	.value	.LVU253
	.long	.Ldebug_ranges0+0x150
	.byte	0x1
	.value	0x12d
	.byte	0x8
	.long	0x120d
	.uleb128 0x2a
	.long	0x17a3
	.long	.LLST24
	.long	.LVUS24
	.uleb128 0x2a
	.long	0x1796
	.long	.LLST25
	.long	.LVUS25
	.uleb128 0x2a
	.long	0x1789
	.long	.LLST26
	.long	.LVUS26
	.uleb128 0x2a
	.long	0x177d
	.long	.LLST27
	.long	.LVUS27
	.uleb128 0x2b
	.long	.Ldebug_ranges0+0x150
	.uleb128 0x2c
	.long	0x17b0
	.long	.LLST28
	.long	.LVUS28
	.uleb128 0x2c
	.long	0x17bd
	.long	.LLST29
	.long	.LVUS29
	.uleb128 0x2c
	.long	0x17c8
	.long	.LLST30
	.long	.LVUS30
	.uleb128 0x2c
	.long	0x17d3
	.long	.LLST31
	.long	.LVUS31
	.uleb128 0x2c
	.long	0x17e0
	.long	.LLST32
	.long	.LVUS32
	.uleb128 0x2e
	.quad	.LVL38
	.long	0x1973
	.long	0x1172
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.byte	0
	.uleb128 0x30
	.quad	.LVL43
	.long	0x1980
	.uleb128 0x30
	.quad	.LVL45
	.long	0x1980
	.uleb128 0x30
	.quad	.LVL47
	.long	0x1910
	.uleb128 0x2e
	.quad	.LVL48
	.long	0x191c
	.long	0x11be
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC9
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x7c
	.sleb128 0
	.byte	0
	.uleb128 0x30
	.quad	.LVL56
	.long	0x1980
	.uleb128 0x30
	.quad	.LVL66
	.long	0x1980
	.uleb128 0x2e
	.quad	.LVL69
	.long	0x1973
	.long	0x11f0
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.byte	0
	.uleb128 0x31
	.quad	.LVL74
	.long	0x191c
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC10
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x2e
	.quad	.LVL34
	.long	0x191c
	.long	0x122c
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC6
	.byte	0
	.uleb128 0x2e
	.quad	.LVL36
	.long	0x191c
	.long	0x124b
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC7
	.byte	0
	.uleb128 0x2e
	.quad	.LVL75
	.long	0x191c
	.long	0x126a
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC11
	.byte	0
	.uleb128 0x2e
	.quad	.LVL87
	.long	0x191c
	.long	0x1289
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC8
	.byte	0
	.uleb128 0x31
	.quad	.LVL109
	.long	0x191c
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC16
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x2e
	.quad	.LVL9
	.long	0x198d
	.long	0x12be
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.byte	0
	.uleb128 0x30
	.quad	.LVL25
	.long	0x1910
	.uleb128 0x2e
	.quad	.LVL27
	.long	0x191c
	.long	0x12ea
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC4
	.byte	0
	.uleb128 0x30
	.quad	.LVL31
	.long	0x1910
	.uleb128 0x2e
	.quad	.LVL32
	.long	0x191c
	.long	0x131f
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC5
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x5
	.byte	0x91
	.sleb128 -188
	.byte	0x94
	.byte	0x4
	.byte	0
	.uleb128 0x2e
	.quad	.LVL76
	.long	0x199a
	.long	0x1337
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x73
	.sleb128 0
	.byte	0
	.uleb128 0x2e
	.quad	.LVL77
	.long	0x19a7
	.long	0x134f
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x76
	.sleb128 0
	.byte	0
	.uleb128 0x2e
	.quad	.LVL81
	.long	0x19b4
	.long	0x1367
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.byte	0
	.uleb128 0x2e
	.quad	.LVL83
	.long	0x19c1
	.long	0x137f
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x8
	.byte	0x80
	.byte	0
	.uleb128 0x2e
	.quad	.LVL98
	.long	0x19a7
	.long	0x1397
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x76
	.sleb128 0
	.byte	0
	.uleb128 0x30
	.quad	.LVL111
	.long	0x1910
	.uleb128 0x2e
	.quad	.LVL112
	.long	0x191c
	.long	0x13d3
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC17
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x5
	.byte	0x91
	.sleb128 -188
	.byte	0x94
	.byte	0x4
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x58
	.uleb128 0x3
	.byte	0x7c
	.sleb128 0
	.byte	0x1f
	.byte	0
	.uleb128 0x31
	.quad	.LVL114
	.long	0x19ce
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7f
	.sleb128 0
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x34
	.long	0xac4
	.quad	.LBI60
	.value	.LVU339
	.quad	.LBB60
	.quad	.LBE60-.LBB60
	.byte	0x1
	.value	0x244
	.byte	0x2
	.long	0x1615
	.uleb128 0x35
	.quad	.LBB61
	.quad	.LBE61-.LBB61
	.uleb128 0x36
	.long	0xad2
	.uleb128 0x36
	.long	0xadd
	.uleb128 0x36
	.long	0xae8
	.uleb128 0x2c
	.long	0xaf3
	.long	.LLST33
	.long	.LVUS33
	.uleb128 0x2c
	.long	0xafe
	.long	.LLST34
	.long	.LVUS34
	.uleb128 0x2c
	.long	0xb09
	.long	.LLST35
	.long	.LVUS35
	.uleb128 0x2c
	.long	0xb14
	.long	.LLST36
	.long	.LVUS36
	.uleb128 0x36
	.long	0xb21
	.uleb128 0x36
	.long	0xb2e
	.uleb128 0x36
	.long	0xb3a
	.uleb128 0x2c
	.long	0xb46
	.long	.LLST37
	.long	.LVUS37
	.uleb128 0x2c
	.long	0xb53
	.long	.LLST38
	.long	.LVUS38
	.uleb128 0x2c
	.long	0xb5f
	.long	.LLST39
	.long	.LVUS39
	.uleb128 0x2c
	.long	0xb6b
	.long	.LLST40
	.long	.LVUS40
	.uleb128 0x2c
	.long	0xb77
	.long	.LLST41
	.long	.LVUS41
	.uleb128 0x2c
	.long	0xb83
	.long	.LLST42
	.long	.LVUS42
	.uleb128 0x2d
	.long	0xb90
	.uleb128 0x3
	.byte	0x91
	.sleb128 -74
	.uleb128 0x37
	.long	0xbe1
	.long	.Ldebug_ranges0+0x190
	.long	0x1510
	.uleb128 0x2c
	.long	0xbe2
	.long	.LLST43
	.long	.LVUS43
	.uleb128 0x38
	.long	0xbef
	.long	.Ldebug_ranges0+0x1c0
	.uleb128 0x2c
	.long	0xbf0
	.long	.LLST44
	.long	.LVUS44
	.uleb128 0x31
	.quad	.LVL153
	.long	0x19db
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x8
	.byte	0x3c
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x39
	.long	0xb9d
	.quad	.LBB65
	.quad	.LBE65-.LBB65
	.long	0x154e
	.uleb128 0x2c
	.long	0xba2
	.long	.LLST45
	.long	.LVUS45
	.uleb128 0x38
	.long	0xbaf
	.long	.Ldebug_ranges0+0x1f0
	.uleb128 0x2c
	.long	0xbb0
	.long	.LLST46
	.long	.LVUS46
	.byte	0
	.byte	0
	.uleb128 0x39
	.long	0xbbf
	.quad	.LBB68
	.quad	.LBE68-.LBB68
	.long	0x158c
	.uleb128 0x2c
	.long	0xbc4
	.long	.LLST47
	.long	.LVUS47
	.uleb128 0x38
	.long	0xbd1
	.long	.Ldebug_ranges0+0x220
	.uleb128 0x2c
	.long	0xbd2
	.long	.LLST48
	.long	.LVUS48
	.byte	0
	.byte	0
	.uleb128 0x2e
	.quad	.LVL123
	.long	0x18f7
	.long	0x15b5
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x9
	.byte	0x3
	.quad	.LC19
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x1
	.byte	0x30
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x1
	.byte	0x30
	.byte	0
	.uleb128 0x2e
	.quad	.LVL125
	.long	0x19e8
	.long	0x15de
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x31
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC20
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x1
	.byte	0x38
	.byte	0
	.uleb128 0x2e
	.quad	.LVL127
	.long	0x19f5
	.long	0x1606
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x5
	.byte	0x91
	.sleb128 -188
	.byte	0x94
	.byte	0x4
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x4
	.byte	0x91
	.sleb128 -184
	.byte	0x6
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x1
	.byte	0x33
	.byte	0
	.uleb128 0x30
	.quad	.LVL160
	.long	0x1a02
	.byte	0
	.byte	0
	.uleb128 0x2e
	.quad	.LVL4
	.long	0x191c
	.long	0x163a
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC1
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x2
	.byte	0x73
	.sleb128 0
	.byte	0
	.uleb128 0x30
	.quad	.LVL18
	.long	0x1910
	.uleb128 0x2e
	.quad	.LVL19
	.long	0x191c
	.long	0x166d
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x9
	.byte	0x3
	.quad	.LC22
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x3
	.byte	0x76
	.sleb128 0
	.byte	0x1f
	.byte	0
	.uleb128 0x2e
	.quad	.LVL29
	.long	0x1929
	.long	0x1685
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.byte	0
	.uleb128 0x30
	.quad	.LVL117
	.long	0x1910
	.uleb128 0x2e
	.quad	.LVL118
	.long	0x191c
	.long	0x16aa
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x2
	.byte	0x76
	.sleb128 0
	.byte	0
	.uleb128 0x2e
	.quad	.LVL120
	.long	0x1a0e
	.long	0x16c2
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.byte	0
	.uleb128 0x2e
	.quad	.LVL121
	.long	0x1a1b
	.long	0x16eb
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x1
	.byte	0x30
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x58
	.uleb128 0x1
	.byte	0x30
	.uleb128 0x2f
	.uleb128 0x1
	.byte	0x59
	.uleb128 0x3
	.byte	0x73
	.sleb128 112
	.byte	0
	.uleb128 0x30
	.quad	.LVL166
	.long	0x1a28
	.byte	0
	.uleb128 0x3a
	.long	.LASF184
	.byte	0x1
	.value	0x1bb
	.byte	0xc
	.long	0x57
	.byte	0x1
	.long	0x176b
	.uleb128 0x1d
	.string	"fd"
	.byte	0x1
	.value	0x1bb
	.byte	0x22
	.long	0x57
	.uleb128 0x1d
	.string	"dev"
	.byte	0x1
	.value	0x1bb
	.byte	0x3a
	.long	0xa0c
	.uleb128 0x1e
	.long	.LASF179
	.byte	0x1
	.value	0x1bd
	.byte	0x1e
	.long	0x47b
	.uleb128 0x1e
	.long	.LASF170
	.byte	0x1
	.value	0x1be
	.byte	0x1f
	.long	0x525
	.uleb128 0x1e
	.long	.LASF180
	.byte	0x1
	.value	0x1bf
	.byte	0x1b
	.long	0x4ec
	.uleb128 0x20
	.string	"ret"
	.byte	0x1
	.value	0x1c0
	.byte	0x6
	.long	0x57
	.uleb128 0x27
	.long	.LASF182
	.byte	0x1
	.value	0x1f7
	.byte	0x1
	.uleb128 0x27
	.long	.LASF183
	.byte	0x1
	.value	0x1f5
	.byte	0x1
	.byte	0
	.uleb128 0x3a
	.long	.LASF185
	.byte	0x1
	.value	0x155
	.byte	0xc
	.long	0x57
	.byte	0x1
	.long	0x17ee
	.uleb128 0x1d
	.string	"fd"
	.byte	0x1
	.value	0x155
	.byte	0x22
	.long	0x57
	.uleb128 0x1d
	.string	"res"
	.byte	0x1
	.value	0x155
	.byte	0x32
	.long	0x17ee
	.uleb128 0x3b
	.long	.LASF165
	.byte	0x1
	.value	0x155
	.byte	0x49
	.long	0x17f4
	.uleb128 0x1d
	.string	"dev"
	.byte	0x1
	.value	0x156
	.byte	0x1d
	.long	0xa0c
	.uleb128 0x20
	.string	"enc"
	.byte	0x1
	.value	0x158
	.byte	0x12
	.long	0x17fa
	.uleb128 0x20
	.string	"i"
	.byte	0x1
	.value	0x159
	.byte	0xf
	.long	0x6c
	.uleb128 0x20
	.string	"j"
	.byte	0x1
	.value	0x159
	.byte	0x12
	.long	0x6c
	.uleb128 0x1e
	.long	.LASF166
	.byte	0x1
	.value	0x15a
	.byte	0xa
	.long	0xf1
	.uleb128 0x1e
	.long	.LASF169
	.byte	0x1
	.value	0x15b
	.byte	0x16
	.long	0xa0c
	.byte	0
	.uleb128 0x3
	.byte	0x8
	.long	0x5f2
	.uleb128 0x3
	.byte	0x8
	.long	0x93b
	.uleb128 0x3
	.byte	0x8
	.long	0x7dd
	.uleb128 0x3a
	.long	.LASF186
	.byte	0x1
	.value	0x112
	.byte	0xc
	.long	0x57
	.byte	0x1
	.long	0x1853
	.uleb128 0x1d
	.string	"fd"
	.byte	0x1
	.value	0x112
	.byte	0x22
	.long	0x57
	.uleb128 0x1d
	.string	"res"
	.byte	0x1
	.value	0x112
	.byte	0x32
	.long	0x17ee
	.uleb128 0x3b
	.long	.LASF165
	.byte	0x1
	.value	0x112
	.byte	0x49
	.long	0x17f4
	.uleb128 0x1d
	.string	"dev"
	.byte	0x1
	.value	0x113
	.byte	0x1d
	.long	0xa0c
	.uleb128 0x20
	.string	"ret"
	.byte	0x1
	.value	0x115
	.byte	0x6
	.long	0x57
	.byte	0
	.uleb128 0x3c
	.long	.LASF187
	.byte	0x1
	.byte	0xbe
	.byte	0xc
	.long	0x57
	.byte	0x1
	.long	0x18aa
	.uleb128 0x3d
	.string	"fd"
	.byte	0x1
	.byte	0xbe
	.byte	0x20
	.long	0x57
	.uleb128 0x3e
	.string	"res"
	.byte	0x1
	.byte	0xc0
	.byte	0xe
	.long	0x17ee
	.uleb128 0x3f
	.long	.LASF165
	.byte	0x1
	.byte	0xc1
	.byte	0x14
	.long	0x17f4
	.uleb128 0x3e
	.string	"i"
	.byte	0x1
	.byte	0xc2
	.byte	0xf
	.long	0x6c
	.uleb128 0x3e
	.string	"dev"
	.byte	0x1
	.byte	0xc3
	.byte	0x16
	.long	0xa0c
	.uleb128 0x3e
	.string	"ret"
	.byte	0x1
	.byte	0xc4
	.byte	0x6
	.long	0x57
	.byte	0
	.uleb128 0x3c
	.long	.LASF188
	.byte	0x1
	.byte	0x53
	.byte	0xc
	.long	0x57
	.byte	0x1
	.long	0x18f7
	.uleb128 0x3d
	.string	"out"
	.byte	0x1
	.byte	0x53
	.byte	0x1e
	.long	0x475
	.uleb128 0x40
	.long	.LASF189
	.byte	0x1
	.byte	0x53
	.byte	0x2f
	.long	0x315
	.uleb128 0x3e
	.string	"fd"
	.byte	0x1
	.byte	0x55
	.byte	0x6
	.long	0x57
	.uleb128 0x3e
	.string	"ret"
	.byte	0x1
	.byte	0x55
	.byte	0xa
	.long	0x57
	.uleb128 0x3f
	.long	.LASF190
	.byte	0x1
	.byte	0x56
	.byte	0xb
	.long	0x121
	.byte	0
	.uleb128 0x41
	.long	.LASF191
	.long	.LASF193
	.byte	0x12
	.byte	0xc6
	.byte	0xc
	.uleb128 0x42
	.long	.LASF192
	.long	.LASF192
	.byte	0x13
	.value	0x240
	.byte	0x16
	.uleb128 0x41
	.long	.LASF194
	.long	.LASF194
	.byte	0x2
	.byte	0x25
	.byte	0xd
	.uleb128 0x42
	.long	.LASF195
	.long	.LASF195
	.byte	0x9
	.value	0x146
	.byte	0xc
	.uleb128 0x42
	.long	.LASF196
	.long	.LASF196
	.byte	0xc
	.value	0x161
	.byte	0xc
	.uleb128 0x41
	.long	.LASF197
	.long	.LASF197
	.byte	0x13
	.byte	0x79
	.byte	0xc
	.uleb128 0x42
	.long	.LASF198
	.long	.LASF198
	.byte	0x10
	.value	0x16f
	.byte	0xc
	.uleb128 0x41
	.long	.LASF199
	.long	.LASF200
	.byte	0x14
	.byte	0x3d
	.byte	0xf
	.uleb128 0x43
	.long	.LASF224
	.long	.LASF225
	.byte	0x16
	.byte	0
	.uleb128 0x42
	.long	.LASF201
	.long	.LASF201
	.byte	0x10
	.value	0x182
	.byte	0xc
	.uleb128 0x42
	.long	.LASF202
	.long	.LASF202
	.byte	0x10
	.value	0x1ad
	.byte	0x13
	.uleb128 0x42
	.long	.LASF203
	.long	.LASF203
	.byte	0x10
	.value	0x15a
	.byte	0xd
	.uleb128 0x42
	.long	.LASF204
	.long	.LASF204
	.byte	0x10
	.value	0x161
	.byte	0x16
	.uleb128 0x42
	.long	.LASF205
	.long	.LASF205
	.byte	0x15
	.value	0x233
	.byte	0xd
	.uleb128 0x42
	.long	.LASF206
	.long	.LASF206
	.byte	0x10
	.value	0x159
	.byte	0xd
	.uleb128 0x42
	.long	.LASF207
	.long	.LASF207
	.byte	0x10
	.value	0x1b8
	.byte	0x1c
	.uleb128 0x42
	.long	.LASF208
	.long	.LASF208
	.byte	0x15
	.value	0x21b
	.byte	0xe
	.uleb128 0x42
	.long	.LASF209
	.long	.LASF209
	.byte	0x10
	.value	0x156
	.byte	0xd
	.uleb128 0x42
	.long	.LASF210
	.long	.LASF210
	.byte	0xc
	.value	0x420
	.byte	0x11
	.uleb128 0x42
	.long	.LASF211
	.long	.LASF211
	.byte	0xc
	.value	0x16e
	.byte	0x10
	.uleb128 0x42
	.long	.LASF212
	.long	.LASF212
	.byte	0xc
	.value	0x168
	.byte	0x10
	.uleb128 0x44
	.string	"sin"
	.string	"sin"
	.byte	0x17
	.byte	0x40
	.byte	0x1
	.uleb128 0x42
	.long	.LASF213
	.long	.LASF213
	.byte	0x10
	.value	0x192
	.byte	0x17
	.uleb128 0x42
	.long	.LASF214
	.long	.LASF214
	.byte	0x10
	.value	0x197
	.byte	0x5
	.uleb128 0x45
	.long	.LASF226
	.long	.LASF226
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x13
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x21
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x4
	.byte	0x1
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0xb
	.byte	0x1
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x26
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x27
	.uleb128 0xa
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x28
	.uleb128 0xa
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.byte	0
	.byte	0
	.uleb128 0x29
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x2138
	.uleb128 0x5
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0x5
	.uleb128 0x57
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2a
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2b
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x55
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2c
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2d
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x2e
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2f
	.uleb128 0x410a
	.byte	0
	.uleb128 0x2
	.uleb128 0x18
	.uleb128 0x2111
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x30
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x31
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x32
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x2138
	.uleb128 0x5
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x57
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x33
	.uleb128 0xa
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.byte	0
	.byte	0
	.uleb128 0x34
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x2138
	.uleb128 0x5
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0x5
	.uleb128 0x57
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x35
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.byte	0
	.byte	0
	.uleb128 0x36
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x37
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x38
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x55
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x39
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3a
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3b
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3c
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3d
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3e
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3f
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x40
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x41
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x42
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x43
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x44
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x6e
	.uleb128 0x8
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x45
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LVUS0:
	.uleb128 0
	.uleb128 .LVU13
	.uleb128 .LVU13
	.uleb128 0
.LLST0:
	.quad	.LVL0-.Ltext0
	.quad	.LVL3-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL3-.Ltext0
	.quad	.LFE29-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LVUS1:
	.uleb128 0
	.uleb128 .LVU12
	.uleb128 .LVU12
	.uleb128 0
.LLST1:
	.quad	.LVL0-.Ltext0
	.quad	.LVL2-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL2-.Ltext0
	.quad	.LFE29-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	0
	.quad	0
.LVUS2:
	.uleb128 .LVU26
	.uleb128 .LVU44
	.uleb128 .LVU51
	.uleb128 .LVU61
	.uleb128 .LVU61
	.uleb128 .LVU62
	.uleb128 .LVU65
	.uleb128 .LVU68
	.uleb128 .LVU68
	.uleb128 .LVU76
	.uleb128 .LVU76
	.uleb128 .LVU81
	.uleb128 .LVU81
	.uleb128 .LVU324
	.uleb128 .LVU324
	.uleb128 .LVU326
	.uleb128 .LVU336
	.uleb128 .LVU338
	.uleb128 .LVU444
	.uleb128 .LVU446
	.uleb128 .LVU446
	.uleb128 0
.LLST2:
	.quad	.LVL8-.Ltext0
	.quad	.LVL13-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL16-.Ltext0
	.quad	.LVL20-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL20-.Ltext0
	.quad	.LVL21-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL23-.Ltext0
	.quad	.LVL24-.Ltext0
	.value	0x3
	.byte	0x9
	.byte	0xa1
	.byte	0x9f
	.quad	.LVL24-.Ltext0
	.quad	.LVL28-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL28-.Ltext0
	.quad	.LVL30-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL30-.Ltext0
	.quad	.LVL116-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL116-.Ltext0
	.quad	.LVL117-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL121-.Ltext0
	.quad	.LVL122-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL165-.Ltext0
	.quad	.LVL166-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL166-1-.Ltext0
	.quad	.LFE29-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	0
	.quad	0
.LVUS3:
	.uleb128 .LVU26
	.uleb128 .LVU44
	.uleb128 .LVU51
	.uleb128 .LVU60
	.uleb128 .LVU65
	.uleb128 .LVU352
.LLST3:
	.quad	.LVL8-.Ltext0
	.quad	.LVL13-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL16-.Ltext0
	.quad	.LVL19-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL23-.Ltext0
	.quad	.LVL126-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	0
	.quad	0
.LVUS4:
	.uleb128 .LVU11
	.uleb128 .LVU54
	.uleb128 .LVU62
	.uleb128 .LVU72
.LLST4:
	.quad	.LVL1-.Ltext0
	.quad	.LVL17-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL21-.Ltext0
	.quad	.LVL26-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LVUS5:
	.uleb128 .LVU322
	.uleb128 .LVU352
.LLST5:
	.quad	.LVL115-.Ltext0
	.quad	.LVL126-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LVUS6:
	.uleb128 .LVU15
	.uleb128 .LVU26
	.uleb128 .LVU44
	.uleb128 .LVU51
	.uleb128 .LVU62
	.uleb128 .LVU65
.LLST6:
	.quad	.LVL4-.Ltext0
	.quad	.LVL8-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL13-.Ltext0
	.quad	.LVL16-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL21-.Ltext0
	.quad	.LVL23-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LVUS7:
	.uleb128 .LVU15
	.uleb128 .LVU26
	.uleb128 .LVU44
	.uleb128 .LVU51
	.uleb128 .LVU62
	.uleb128 .LVU65
.LLST7:
	.quad	.LVL4-.Ltext0
	.quad	.LVL8-.Ltext0
	.value	0x6
	.byte	0xf2
	.long	.Ldebug_info0+3186
	.sleb128 0
	.quad	.LVL13-.Ltext0
	.quad	.LVL16-.Ltext0
	.value	0x6
	.byte	0xf2
	.long	.Ldebug_info0+3186
	.sleb128 0
	.quad	.LVL21-.Ltext0
	.quad	.LVL23-.Ltext0
	.value	0x6
	.byte	0xf2
	.long	.Ldebug_info0+3186
	.sleb128 0
	.quad	0
	.quad	0
.LVUS8:
	.uleb128 .LVU20
	.uleb128 .LVU24
	.uleb128 .LVU24
	.uleb128 .LVU26
	.uleb128 .LVU44
	.uleb128 .LVU47
	.uleb128 .LVU47
	.uleb128 .LVU51
	.uleb128 .LVU62
	.uleb128 .LVU65
.LLST8:
	.quad	.LVL6-.Ltext0
	.quad	.LVL7-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL7-1-.Ltext0
	.quad	.LVL8-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL13-.Ltext0
	.quad	.LVL14-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL14-1-.Ltext0
	.quad	.LVL16-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL21-.Ltext0
	.quad	.LVL23-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	0
	.quad	0
.LVUS9:
	.uleb128 .LVU49
	.uleb128 .LVU51
.LLST9:
	.quad	.LVL15-.Ltext0
	.quad	.LVL16-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	0
	.quad	0
.LVUS10:
	.uleb128 .LVU28
	.uleb128 .LVU44
	.uleb128 .LVU68
	.uleb128 .LVU76
	.uleb128 .LVU81
	.uleb128 .LVU318
.LLST10:
	.quad	.LVL8-.Ltext0
	.quad	.LVL13-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL24-.Ltext0
	.quad	.LVL28-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL30-.Ltext0
	.quad	.LVL114-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	0
	.quad	0
.LVUS11:
	.uleb128 .LVU36
	.uleb128 .LVU41
	.uleb128 .LVU41
	.uleb128 .LVU44
	.uleb128 .LVU68
	.uleb128 .LVU71
	.uleb128 .LVU71
	.uleb128 .LVU76
	.uleb128 .LVU81
	.uleb128 .LVU318
.LLST11:
	.quad	.LVL10-.Ltext0
	.quad	.LVL12-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL12-.Ltext0
	.quad	.LVL13-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL24-.Ltext0
	.quad	.LVL25-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL25-1-.Ltext0
	.quad	.LVL28-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL30-.Ltext0
	.quad	.LVL114-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	0
	.quad	0
.LVUS12:
	.uleb128 .LVU81
	.uleb128 .LVU83
	.uleb128 .LVU83
	.uleb128 .LVU224
	.uleb128 .LVU227
	.uleb128 .LVU231
	.uleb128 .LVU231
	.uleb128 .LVU316
.LLST12:
	.quad	.LVL30-.Ltext0
	.quad	.LVL31-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL31-1-.Ltext0
	.quad	.LVL80-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL82-.Ltext0
	.quad	.LVL83-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL83-1-.Ltext0
	.quad	.LVL113-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	0
	.quad	0
.LVUS13:
	.uleb128 .LVU38
	.uleb128 .LVU44
	.uleb128 .LVU81
	.uleb128 .LVU222
	.uleb128 .LVU222
	.uleb128 .LVU223
	.uleb128 .LVU223
	.uleb128 .LVU224
	.uleb128 .LVU224
	.uleb128 .LVU316
.LLST13:
	.quad	.LVL11-.Ltext0
	.quad	.LVL13-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL30-.Ltext0
	.quad	.LVL78-.Ltext0
	.value	0x3
	.byte	0x91
	.sleb128 -188
	.quad	.LVL78-.Ltext0
	.quad	.LVL79-.Ltext0
	.value	0x3
	.byte	0x91
	.sleb128 -184
	.quad	.LVL79-.Ltext0
	.quad	.LVL80-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL80-.Ltext0
	.quad	.LVL113-.Ltext0
	.value	0x3
	.byte	0x91
	.sleb128 -188
	.quad	0
	.quad	0
.LVUS14:
	.uleb128 .LVU85
	.uleb128 .LVU221
	.uleb128 .LVU232
	.uleb128 .LVU233
	.uleb128 .LVU233
	.uleb128 .LVU316
.LLST14:
	.quad	.LVL33-.Ltext0
	.quad	.LVL77-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL84-.Ltext0
	.quad	.LVL85-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL85-.Ltext0
	.quad	.LVL113-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LVUS15:
	.uleb128 .LVU87
	.uleb128 .LVU89
	.uleb128 .LVU91
	.uleb128 .LVU94
	.uleb128 .LVU215
	.uleb128 .LVU218
	.uleb128 .LVU284
	.uleb128 .LVU291
	.uleb128 .LVU308
	.uleb128 .LVU316
.LLST15:
	.quad	.LVL34-.Ltext0
	.quad	.LVL35-.Ltext0
	.value	0x3
	.byte	0x9
	.byte	0xfe
	.byte	0x9f
	.quad	.LVL36-.Ltext0
	.quad	.LVL37-.Ltext0
	.value	0x3
	.byte	0x9
	.byte	0xf2
	.byte	0x9f
	.quad	.LVL75-.Ltext0
	.quad	.LVL75-.Ltext0
	.value	0x3
	.byte	0x9
	.byte	0xfe
	.byte	0x9f
	.quad	.LVL97-.Ltext0
	.quad	.LVL99-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL109-.Ltext0
	.quad	.LVL113-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LVUS16:
	.uleb128 .LVU85
	.uleb128 .LVU87
	.uleb128 .LVU89
	.uleb128 .LVU91
	.uleb128 .LVU94
	.uleb128 .LVU215
	.uleb128 .LVU238
	.uleb128 .LVU284
	.uleb128 .LVU291
	.uleb128 .LVU308
.LLST16:
	.quad	.LVL33-.Ltext0
	.quad	.LVL34-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL35-.Ltext0
	.quad	.LVL36-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL37-.Ltext0
	.quad	.LVL75-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL86-.Ltext0
	.quad	.LVL97-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL99-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LVUS17:
	.uleb128 .LVU85
	.uleb128 .LVU87
	.uleb128 .LVU89
	.uleb128 .LVU91
	.uleb128 .LVU94
	.uleb128 .LVU215
	.uleb128 .LVU238
	.uleb128 .LVU284
	.uleb128 .LVU291
	.uleb128 .LVU308
.LLST17:
	.quad	.LVL33-.Ltext0
	.quad	.LVL34-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL35-.Ltext0
	.quad	.LVL36-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL37-.Ltext0
	.quad	.LVL75-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL86-.Ltext0
	.quad	.LVL97-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL99-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	0
	.quad	0
.LVUS18:
	.uleb128 .LVU85
	.uleb128 .LVU87
	.uleb128 .LVU89
	.uleb128 .LVU91
	.uleb128 .LVU94
	.uleb128 .LVU215
	.uleb128 .LVU238
	.uleb128 .LVU284
	.uleb128 .LVU291
	.uleb128 .LVU308
.LLST18:
	.quad	.LVL33-.Ltext0
	.quad	.LVL34-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL35-.Ltext0
	.quad	.LVL36-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL37-.Ltext0
	.quad	.LVL75-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL86-.Ltext0
	.quad	.LVL97-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL99-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	0
	.quad	0
.LVUS19:
	.uleb128 .LVU85
	.uleb128 .LVU87
	.uleb128 .LVU89
	.uleb128 .LVU91
	.uleb128 .LVU94
	.uleb128 .LVU215
	.uleb128 .LVU238
	.uleb128 .LVU284
	.uleb128 .LVU291
	.uleb128 .LVU308
.LLST19:
	.quad	.LVL33-.Ltext0
	.quad	.LVL34-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL35-.Ltext0
	.quad	.LVL36-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL37-.Ltext0
	.quad	.LVL75-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL86-.Ltext0
	.quad	.LVL97-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL99-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	0
	.quad	0
.LVUS20:
	.uleb128 .LVU146
	.uleb128 .LVU195
	.uleb128 .LVU211
	.uleb128 .LVU215
	.uleb128 .LVU262
	.uleb128 .LVU281
	.uleb128 .LVU281
	.uleb128 .LVU284
	.uleb128 .LVU291
	.uleb128 .LVU305
	.uleb128 .LVU305
	.uleb128 .LVU308
.LLST20:
	.quad	.LVL57-.Ltext0
	.quad	.LVL65-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL74-.Ltext0
	.quad	.LVL75-.Ltext0
	.value	0x3
	.byte	0x9
	.byte	0xfe
	.byte	0x9f
	.quad	.LVL89-.Ltext0
	.quad	.LVL96-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL96-.Ltext0
	.quad	.LVL97-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL99-.Ltext0
	.quad	.LVL108-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL108-.Ltext0
	.quad	.LVL109-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LVUS21:
	.uleb128 .LVU149
	.uleb128 .LVU193
	.uleb128 .LVU262
	.uleb128 .LVU281
	.uleb128 .LVU291
	.uleb128 .LVU305
.LLST21:
	.quad	.LVL57-.Ltext0
	.quad	.LVL64-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL89-.Ltext0
	.quad	.LVL96-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL99-.Ltext0
	.quad	.LVL108-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LVUS22:
	.uleb128 .LVU149
	.uleb128 .LVU193
	.uleb128 .LVU262
	.uleb128 .LVU281
	.uleb128 .LVU291
	.uleb128 .LVU305
.LLST22:
	.quad	.LVL57-.Ltext0
	.quad	.LVL64-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL89-.Ltext0
	.quad	.LVL96-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL99-.Ltext0
	.quad	.LVL108-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	0
	.quad	0
.LVUS23:
	.uleb128 .LVU163
	.uleb128 .LVU170
	.uleb128 .LVU176
	.uleb128 .LVU181
	.uleb128 .LVU184
	.uleb128 .LVU188
	.uleb128 .LVU262
	.uleb128 .LVU265
	.uleb128 .LVU269
	.uleb128 .LVU271
	.uleb128 .LVU275
	.uleb128 .LVU281
	.uleb128 .LVU291
	.uleb128 .LVU293
	.uleb128 .LVU297
	.uleb128 .LVU299
	.uleb128 .LVU304
	.uleb128 .LVU305
.LLST23:
	.quad	.LVL58-.Ltext0
	.quad	.LVL59-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL60-.Ltext0
	.quad	.LVL61-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL62-.Ltext0
	.quad	.LVL63-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL89-.Ltext0
	.quad	.LVL90-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL92-.Ltext0
	.quad	.LVL93-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL95-.Ltext0
	.quad	.LVL96-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL99-.Ltext0
	.quad	.LVL100-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL102-.Ltext0
	.quad	.LVL104-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL107-.Ltext0
	.quad	.LVL108-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LVUS24:
	.uleb128 .LVU94
	.uleb128 .LVU146
	.uleb128 .LVU195
	.uleb128 .LVU211
	.uleb128 .LVU253
	.uleb128 .LVU262
.LLST24:
	.quad	.LVL37-.Ltext0
	.quad	.LVL57-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL65-.Ltext0
	.quad	.LVL74-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL87-.Ltext0
	.quad	.LVL89-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LVUS25:
	.uleb128 .LVU94
	.uleb128 .LVU146
	.uleb128 .LVU195
	.uleb128 .LVU211
	.uleb128 .LVU253
	.uleb128 .LVU262
.LLST25:
	.quad	.LVL37-.Ltext0
	.quad	.LVL57-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL65-.Ltext0
	.quad	.LVL74-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL87-.Ltext0
	.quad	.LVL89-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	0
	.quad	0
.LVUS26:
	.uleb128 .LVU94
	.uleb128 .LVU146
	.uleb128 .LVU195
	.uleb128 .LVU211
	.uleb128 .LVU253
	.uleb128 .LVU262
.LLST26:
	.quad	.LVL37-.Ltext0
	.quad	.LVL57-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL65-.Ltext0
	.quad	.LVL74-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL87-.Ltext0
	.quad	.LVL89-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	0
	.quad	0
.LVUS27:
	.uleb128 .LVU94
	.uleb128 .LVU146
	.uleb128 .LVU195
	.uleb128 .LVU211
	.uleb128 .LVU253
	.uleb128 .LVU262
.LLST27:
	.quad	.LVL37-.Ltext0
	.quad	.LVL57-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL65-.Ltext0
	.quad	.LVL74-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL87-.Ltext0
	.quad	.LVL89-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	0
	.quad	0
.LVUS28:
	.uleb128 .LVU96
	.uleb128 .LVU112
	.uleb128 .LVU113
	.uleb128 .LVU116
	.uleb128 .LVU119
	.uleb128 .LVU121
	.uleb128 .LVU123
	.uleb128 .LVU143
	.uleb128 .LVU195
	.uleb128 .LVU196
	.uleb128 .LVU201
	.uleb128 .LVU207
	.uleb128 .LVU207
	.uleb128 .LVU209
.LLST28:
	.quad	.LVL38-.Ltext0
	.quad	.LVL43-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL44-.Ltext0
	.quad	.LVL45-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL46-.Ltext0
	.quad	.LVL47-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL49-.Ltext0
	.quad	.LVL56-1-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL65-.Ltext0
	.quad	.LVL66-1-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL70-.Ltext0
	.quad	.LVL72-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL72-.Ltext0
	.quad	.LVL73-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	0
	.quad	0
.LVUS29:
	.uleb128 .LVU119
	.uleb128 .LVU146
	.uleb128 .LVU195
	.uleb128 .LVU209
	.uleb128 .LVU261
	.uleb128 .LVU262
.LLST29:
	.quad	.LVL46-.Ltext0
	.quad	.LVL57-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL65-.Ltext0
	.quad	.LVL73-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL88-.Ltext0
	.quad	.LVL89-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	0
	.quad	0
.LVUS30:
	.uleb128 .LVU123
	.uleb128 .LVU125
	.uleb128 .LVU125
	.uleb128 .LVU128
	.uleb128 .LVU128
	.uleb128 .LVU143
	.uleb128 .LVU203
	.uleb128 .LVU209
.LLST30:
	.quad	.LVL49-.Ltext0
	.quad	.LVL50-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL50-.Ltext0
	.quad	.LVL51-.Ltext0
	.value	0x3
	.byte	0x72
	.sleb128 1
	.byte	0x9f
	.quad	.LVL51-.Ltext0
	.quad	.LVL56-1-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL71-.Ltext0
	.quad	.LVL73-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	0
	.quad	0
.LVUS31:
	.uleb128 .LVU102
	.uleb128 .LVU111
	.uleb128 .LVU113
	.uleb128 .LVU119
	.uleb128 .LVU123
	.uleb128 .LVU125
	.uleb128 .LVU133
	.uleb128 .LVU146
.LLST31:
	.quad	.LVL39-.Ltext0
	.quad	.LVL42-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL44-.Ltext0
	.quad	.LVL46-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL49-.Ltext0
	.quad	.LVL50-.Ltext0
	.value	0x1
	.byte	0x5d
	.quad	.LVL52-.Ltext0
	.quad	.LVL57-.Ltext0
	.value	0x1
	.byte	0x5d
	.quad	0
	.quad	0
.LVUS32:
	.uleb128 .LVU104
	.uleb128 .LVU111
	.uleb128 .LVU113
	.uleb128 .LVU116
	.uleb128 .LVU134
	.uleb128 .LVU138
	.uleb128 .LVU138
	.uleb128 .LVU142
.LLST32:
	.quad	.LVL40-.Ltext0
	.quad	.LVL42-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL44-.Ltext0
	.quad	.LVL45-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL52-.Ltext0
	.quad	.LVL53-.Ltext0
	.value	0x9
	.byte	0x3
	.quad	modeset_list
	.quad	.LVL53-.Ltext0
	.quad	.LVL55-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LVUS33:
	.uleb128 .LVU349
	.uleb128 .LVU352
.LLST33:
	.quad	.LVL124-.Ltext0
	.quad	.LVL126-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	0
	.quad	0
.LVUS34:
	.uleb128 .LVU421
	.uleb128 .LVU439
	.uleb128 .LVU441
	.uleb128 .LVU444
.LLST34:
	.quad	.LVL155-.Ltext0
	.quad	.LVL162-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL164-.Ltext0
	.quad	.LVL165-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	0
	.quad	0
.LVUS35:
	.uleb128 .LVU424
	.uleb128 .LVU426
	.uleb128 .LVU426
	.uleb128 .LVU439
.LLST35:
	.quad	.LVL157-.Ltext0
	.quad	.LVL158-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL158-.Ltext0
	.quad	.LVL162-.Ltext0
	.value	0x1
	.byte	0x5d
	.quad	0
	.quad	0
.LVUS36:
	.uleb128 .LVU429
	.uleb128 .LVU439
.LLST36:
	.quad	.LVL159-.Ltext0
	.quad	.LVL162-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	0
	.quad	0
.LVUS37:
	.uleb128 .LVU348
	.uleb128 .LVU350
	.uleb128 .LVU350
	.uleb128 .LVU444
.LLST37:
	.quad	.LVL124-.Ltext0
	.quad	.LVL125-1-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL125-1-.Ltext0
	.quad	.LVL165-.Ltext0
	.value	0x3
	.byte	0x91
	.sleb128 -188
	.quad	0
	.quad	0
.LVUS38:
	.uleb128 .LVU357
	.uleb128 .LVU360
	.uleb128 .LVU360
	.uleb128 .LVU363
	.uleb128 .LVU363
	.uleb128 .LVU366
	.uleb128 .LVU370
	.uleb128 .LVU385
	.uleb128 .LVU394
	.uleb128 .LVU397
	.uleb128 .LVU407
	.uleb128 .LVU410
.LLST38:
	.quad	.LVL128-.Ltext0
	.quad	.LVL129-.Ltext0
	.value	0x10
	.byte	0xa
	.value	0x100
	.byte	0x91
	.sleb128 -73
	.byte	0x94
	.byte	0x1
	.byte	0x8
	.byte	0x38
	.byte	0x24
	.byte	0x8
	.byte	0x38
	.byte	0x26
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL129-.Ltext0
	.quad	.LVL131-.Ltext0
	.value	0x7
	.byte	0xa
	.value	0x100
	.byte	0x70
	.sleb128 0
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL131-.Ltext0
	.quad	.LVL133-.Ltext0
	.value	0x10
	.byte	0xa
	.value	0x100
	.byte	0x91
	.sleb128 -73
	.byte	0x94
	.byte	0x1
	.byte	0x8
	.byte	0x38
	.byte	0x24
	.byte	0x8
	.byte	0x38
	.byte	0x26
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL135-.Ltext0
	.quad	.LVL137-.Ltext0
	.value	0x10
	.byte	0xa
	.value	0x100
	.byte	0x91
	.sleb128 -73
	.byte	0x94
	.byte	0x1
	.byte	0x8
	.byte	0x38
	.byte	0x24
	.byte	0x8
	.byte	0x38
	.byte	0x26
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL142-.Ltext0
	.quad	.LVL143-.Ltext0
	.value	0x10
	.byte	0xa
	.value	0x100
	.byte	0x91
	.sleb128 -73
	.byte	0x94
	.byte	0x1
	.byte	0x8
	.byte	0x38
	.byte	0x24
	.byte	0x8
	.byte	0x38
	.byte	0x26
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL148-.Ltext0
	.quad	.LVL149-.Ltext0
	.value	0x10
	.byte	0xa
	.value	0x100
	.byte	0x91
	.sleb128 -73
	.byte	0x94
	.byte	0x1
	.byte	0x8
	.byte	0x38
	.byte	0x24
	.byte	0x8
	.byte	0x38
	.byte	0x26
	.byte	0x1c
	.byte	0x9f
	.quad	0
	.quad	0
.LVUS39:
	.uleb128 .LVU358
	.uleb128 .LVU363
	.uleb128 .LVU363
	.uleb128 .LVU366
	.uleb128 .LVU370
	.uleb128 .LVU372
	.uleb128 .LVU372
	.uleb128 .LVU385
	.uleb128 .LVU394
	.uleb128 .LVU397
	.uleb128 .LVU407
	.uleb128 .LVU410
.LLST39:
	.quad	.LVL128-.Ltext0
	.quad	.LVL131-.Ltext0
	.value	0x10
	.byte	0xa
	.value	0x100
	.byte	0x91
	.sleb128 -72
	.byte	0x94
	.byte	0x1
	.byte	0x8
	.byte	0x38
	.byte	0x24
	.byte	0x8
	.byte	0x38
	.byte	0x26
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL131-.Ltext0
	.quad	.LVL133-.Ltext0
	.value	0x7
	.byte	0xa
	.value	0x100
	.byte	0x70
	.sleb128 0
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL135-.Ltext0
	.quad	.LVL136-.Ltext0
	.value	0x7
	.byte	0xa
	.value	0x100
	.byte	0x70
	.sleb128 0
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL136-.Ltext0
	.quad	.LVL137-.Ltext0
	.value	0x10
	.byte	0xa
	.value	0x100
	.byte	0x91
	.sleb128 -72
	.byte	0x94
	.byte	0x1
	.byte	0x8
	.byte	0x38
	.byte	0x24
	.byte	0x8
	.byte	0x38
	.byte	0x26
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL142-.Ltext0
	.quad	.LVL143-.Ltext0
	.value	0x10
	.byte	0xa
	.value	0x100
	.byte	0x91
	.sleb128 -72
	.byte	0x94
	.byte	0x1
	.byte	0x8
	.byte	0x38
	.byte	0x24
	.byte	0x8
	.byte	0x38
	.byte	0x26
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL148-.Ltext0
	.quad	.LVL149-.Ltext0
	.value	0x10
	.byte	0xa
	.value	0x100
	.byte	0x91
	.sleb128 -72
	.byte	0x94
	.byte	0x1
	.byte	0x8
	.byte	0x38
	.byte	0x24
	.byte	0x8
	.byte	0x38
	.byte	0x26
	.byte	0x1c
	.byte	0x9f
	.quad	0
	.quad	0
.LVUS40:
	.uleb128 .LVU352
	.uleb128 .LVU444
.LLST40:
	.quad	.LVL126-.Ltext0
	.quad	.LVL165-.Ltext0
	.value	0x3
	.byte	0x91
	.sleb128 -152
	.quad	0
	.quad	0
.LVUS41:
	.uleb128 .LVU352
	.uleb128 .LVU444
.LLST41:
	.quad	.LVL126-.Ltext0
	.quad	.LVL165-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	0
	.quad	0
.LVUS42:
	.uleb128 .LVU352
	.uleb128 .LVU368
	.uleb128 .LVU368
	.uleb128 .LVU370
	.uleb128 .LVU370
	.uleb128 .LVU421
	.uleb128 .LVU421
	.uleb128 .LVU444
.LLST42:
	.quad	.LVL126-.Ltext0
	.quad	.LVL134-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL134-.Ltext0
	.quad	.LVL135-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL135-.Ltext0
	.quad	.LVL155-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL155-.Ltext0
	.quad	.LVL165-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LVUS43:
	.uleb128 .LVU410
	.uleb128 .LVU421
.LLST43:
	.quad	.LVL149-.Ltext0
	.quad	.LVL155-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	0
	.quad	0
.LVUS44:
	.uleb128 .LVU410
	.uleb128 .LVU413
	.uleb128 .LVU413
	.uleb128 .LVU414
	.uleb128 .LVU414
	.uleb128 .LVU421
.LLST44:
	.quad	.LVL149-.Ltext0
	.quad	.LVL151-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL151-.Ltext0
	.quad	.LVL152-.Ltext0
	.value	0x4
	.byte	0xa
	.value	0x1fe
	.byte	0x9f
	.quad	.LVL152-.Ltext0
	.quad	.LVL155-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LVUS45:
	.uleb128 .LVU385
	.uleb128 .LVU394
.LLST45:
	.quad	.LVL137-.Ltext0
	.quad	.LVL142-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LVUS46:
	.uleb128 .LVU385
	.uleb128 .LVU387
	.uleb128 .LVU387
	.uleb128 .LVU388
	.uleb128 .LVU388
	.uleb128 .LVU394
.LLST46:
	.quad	.LVL137-.Ltext0
	.quad	.LVL139-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL139-.Ltext0
	.quad	.LVL140-.Ltext0
	.value	0x3
	.byte	0x8
	.byte	0x5a
	.byte	0x9f
	.quad	.LVL140-.Ltext0
	.quad	.LVL142-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
.LVUS47:
	.uleb128 .LVU397
	.uleb128 .LVU407
.LLST47:
	.quad	.LVL143-.Ltext0
	.quad	.LVL148-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LVUS48:
	.uleb128 .LVU397
	.uleb128 .LVU400
	.uleb128 .LVU400
	.uleb128 .LVU401
	.uleb128 .LVU401
	.uleb128 .LVU407
.LLST48:
	.quad	.LVL143-.Ltext0
	.quad	.LVL145-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL145-.Ltext0
	.quad	.LVL146-.Ltext0
	.value	0x4
	.byte	0xa
	.value	0x136
	.byte	0x9f
	.quad	.LVL146-.Ltext0
	.quad	.LVL148-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
	.section	.debug_aranges,"",@progbits
	.long	0x2c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x8
	.byte	0
	.value	0
	.value	0
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.quad	0
	.quad	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.quad	.LBB20-.Ltext0
	.quad	.LBE20-.Ltext0
	.quad	.LBB55-.Ltext0
	.quad	.LBE55-.Ltext0
	.quad	.LBB56-.Ltext0
	.quad	.LBE56-.Ltext0
	.quad	.LBB57-.Ltext0
	.quad	.LBE57-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB25-.Ltext0
	.quad	.LBE25-.Ltext0
	.quad	.LBB58-.Ltext0
	.quad	.LBE58-.Ltext0
	.quad	.LBB59-.Ltext0
	.quad	.LBE59-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB27-.Ltext0
	.quad	.LBE27-.Ltext0
	.quad	.LBB48-.Ltext0
	.quad	.LBE48-.Ltext0
	.quad	.LBB49-.Ltext0
	.quad	.LBE49-.Ltext0
	.quad	.LBB50-.Ltext0
	.quad	.LBE50-.Ltext0
	.quad	.LBB51-.Ltext0
	.quad	.LBE51-.Ltext0
	.quad	.LBB52-.Ltext0
	.quad	.LBE52-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB29-.Ltext0
	.quad	.LBE29-.Ltext0
	.quad	.LBB38-.Ltext0
	.quad	.LBE38-.Ltext0
	.quad	.LBB41-.Ltext0
	.quad	.LBE41-.Ltext0
	.quad	.LBB42-.Ltext0
	.quad	.LBE42-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB34-.Ltext0
	.quad	.LBE34-.Ltext0
	.quad	.LBB39-.Ltext0
	.quad	.LBE39-.Ltext0
	.quad	.LBB40-.Ltext0
	.quad	.LBE40-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB62-.Ltext0
	.quad	.LBE62-.Ltext0
	.quad	.LBB71-.Ltext0
	.quad	.LBE71-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB63-.Ltext0
	.quad	.LBE63-.Ltext0
	.quad	.LBB64-.Ltext0
	.quad	.LBE64-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB66-.Ltext0
	.quad	.LBE66-.Ltext0
	.quad	.LBB67-.Ltext0
	.quad	.LBE67-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB69-.Ltext0
	.quad	.LBE69-.Ltext0
	.quad	.LBB70-.Ltext0
	.quad	.LBE70-.Ltext0
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF114:
	.string	"vtotal"
.LASF17:
	.string	"size_t"
.LASF95:
	.string	"count_connectors"
.LASF120:
	.string	"drmModeModeInfoPtr"
.LASF54:
	.string	"_IO_codecvt"
.LASF99:
	.string	"min_width"
.LASF188:
	.string	"modeset_open"
.LASF131:
	.string	"possible_crtcs"
.LASF18:
	.string	"int32_t"
.LASF34:
	.string	"_IO_save_end"
.LASF211:
	.string	"write"
.LASF145:
	.string	"_drmModeConnector"
.LASF108:
	.string	"hsync_end"
.LASF27:
	.string	"_IO_write_base"
.LASF193:
	.string	"open"
.LASF217:
	.string	"/home/kitty/Documents/projects/cybergame"
.LASF43:
	.string	"_lock"
.LASF123:
	.string	"buffer_id"
.LASF91:
	.string	"_drmModeRes"
.LASF109:
	.string	"htotal"
.LASF214:
	.string	"drmModeSetCrtc"
.LASF32:
	.string	"_IO_save_base"
.LASF165:
	.string	"conn"
.LASF170:
	.string	"dreq"
.LASF36:
	.string	"_chain"
.LASF78:
	.string	"__u32"
.LASF40:
	.string	"_cur_column"
.LASF59:
	.string	"sys_nerr"
.LASF121:
	.string	"_drmModeCrtc"
.LASF9:
	.string	"__uint8_t"
.LASF61:
	.string	"_sys_nerr"
.LASF72:
	.string	"__environ"
.LASF13:
	.string	"long int"
.LASF150:
	.string	"mmWidth"
.LASF173:
	.string	"mouse"
.LASF225:
	.string	"__builtin_memset"
.LASF122:
	.string	"crtc_id"
.LASF113:
	.string	"vsync_end"
.LASF53:
	.string	"_IO_marker"
.LASF143:
	.string	"DRM_MODE_SUBPIXEL_NONE"
.LASF222:
	.string	"main"
.LASF90:
	.string	"drm_mode_destroy_dumb"
.LASF144:
	.string	"drmModeSubPixel"
.LASF7:
	.string	"signed char"
.LASF175:
	.string	"btny"
.LASF19:
	.string	"uint8_t"
.LASF80:
	.string	"_IO_FILE"
.LASF67:
	.string	"__timezone"
.LASF55:
	.string	"_IO_wide_data"
.LASF216:
	.string	"modeset.c"
.LASF79:
	.string	"__u64"
.LASF138:
	.string	"DRM_MODE_SUBPIXEL_UNKNOWN"
.LASF73:
	.string	"environ"
.LASF3:
	.string	"unsigned char"
.LASF164:
	.string	"stride"
.LASF47:
	.string	"_freeres_list"
.LASF88:
	.string	"drm_mode_map_dumb"
.LASF65:
	.string	"__tzname"
.LASF199:
	.string	"mmap64"
.LASF133:
	.string	"drmModeEncoder"
.LASF2:
	.string	"char"
.LASF190:
	.string	"has_dumb"
.LASF172:
	.string	"fede"
.LASF140:
	.string	"DRM_MODE_SUBPIXEL_HORIZONTAL_BGR"
.LASF154:
	.string	"modes"
.LASF218:
	.string	"_IO_lock_t"
.LASF10:
	.string	"__uint16_t"
.LASF111:
	.string	"vdisplay"
.LASF194:
	.string	"__errno_location"
.LASF70:
	.string	"timezone"
.LASF84:
	.string	"flags"
.LASF24:
	.string	"_IO_read_ptr"
.LASF168:
	.string	"prop"
.LASF56:
	.string	"stdin"
.LASF200:
	.string	"mmap"
.LASF60:
	.string	"sys_errlist"
.LASF103:
	.string	"drmModeRes"
.LASF35:
	.string	"_markers"
.LASF149:
	.string	"connection"
.LASF100:
	.string	"max_width"
.LASF155:
	.string	"count_props"
.LASF189:
	.string	"node"
.LASF136:
	.string	"DRM_MODE_UNKNOWNCONNECTION"
.LASF0:
	.string	"program_invocation_name"
.LASF44:
	.string	"_offset"
.LASF92:
	.string	"count_fbs"
.LASF75:
	.string	"optind"
.LASF116:
	.string	"vrefresh"
.LASF178:
	.string	"card"
.LASF14:
	.string	"__uint64_t"
.LASF179:
	.string	"creq"
.LASF6:
	.string	"long unsigned int"
.LASF38:
	.string	"_flags2"
.LASF26:
	.string	"_IO_read_base"
.LASF51:
	.string	"_unused2"
.LASF134:
	.string	"DRM_MODE_CONNECTED"
.LASF105:
	.string	"clock"
.LASF128:
	.string	"_drmModeEncoder"
.LASF176:
	.string	"argc"
.LASF147:
	.string	"connector_type"
.LASF180:
	.string	"mreq"
.LASF39:
	.string	"_old_offset"
.LASF177:
	.string	"argv"
.LASF110:
	.string	"hskew"
.LASF213:
	.string	"drmModeGetCrtc"
.LASF12:
	.string	"__uint32_t"
.LASF139:
	.string	"DRM_MODE_SUBPIXEL_HORIZONTAL_RGB"
.LASF226:
	.string	"__stack_chk_fail"
.LASF63:
	.string	"long long int"
.LASF101:
	.string	"min_height"
.LASF94:
	.string	"crtcs"
.LASF83:
	.string	"width"
.LASF204:
	.string	"drmModeGetResources"
.LASF81:
	.string	"drm_mode_create_dumb"
.LASF160:
	.string	"double"
.LASF29:
	.string	"_IO_write_end"
.LASF126:
	.string	"gamma_size"
.LASF223:
	.string	"out_close"
.LASF159:
	.string	"float"
.LASF212:
	.string	"read"
.LASF30:
	.string	"_IO_buf_base"
.LASF5:
	.string	"unsigned int"
.LASF142:
	.string	"DRM_MODE_SUBPIXEL_VERTICAL_BGR"
.LASF219:
	.string	"modeset_list"
.LASF186:
	.string	"modeset_setup_dev"
.LASF68:
	.string	"tzname"
.LASF49:
	.string	"__pad5"
.LASF182:
	.string	"err_destroy"
.LASF130:
	.string	"encoder_type"
.LASF23:
	.string	"_flags"
.LASF124:
	.string	"mode_valid"
.LASF89:
	.string	"offset"
.LASF174:
	.string	"btnx"
.LASF50:
	.string	"_mode"
.LASF206:
	.string	"drmModeFreeConnector"
.LASF45:
	.string	"_codecvt"
.LASF146:
	.string	"connector_id"
.LASF117:
	.string	"type"
.LASF97:
	.string	"count_encoders"
.LASF52:
	.string	"FILE"
.LASF96:
	.string	"connectors"
.LASF85:
	.string	"handle"
.LASF207:
	.string	"drmModeGetConnector"
.LASF183:
	.string	"err_fb"
.LASF220:
	.string	"modeset_cleanup"
.LASF107:
	.string	"hsync_start"
.LASF77:
	.string	"optopt"
.LASF203:
	.string	"drmModeFreeEncoder"
.LASF87:
	.string	"size"
.LASF98:
	.string	"encoders"
.LASF64:
	.string	"long long unsigned int"
.LASF112:
	.string	"vsync_start"
.LASF161:
	.string	"signgam"
.LASF20:
	.string	"uint16_t"
.LASF15:
	.string	"__off_t"
.LASF1:
	.string	"program_invocation_short_name"
.LASF48:
	.string	"_freeres_buf"
.LASF104:
	.string	"_drmModeModeInfo"
.LASF153:
	.string	"count_modes"
.LASF184:
	.string	"modeset_create_fb"
.LASF76:
	.string	"opterr"
.LASF102:
	.string	"max_height"
.LASF106:
	.string	"hdisplay"
.LASF224:
	.string	"memset"
.LASF152:
	.string	"subpixel"
.LASF192:
	.string	"drmGetCap"
.LASF33:
	.string	"_IO_backup_base"
.LASF221:
	.string	"modeset_draw"
.LASF42:
	.string	"_shortbuf"
.LASF132:
	.string	"possible_clones"
.LASF181:
	.string	"out_return"
.LASF171:
	.string	"blink"
.LASF208:
	.string	"malloc"
.LASF16:
	.string	"__off64_t"
.LASF215:
	.string	"GNU C17 8.2.1 20181127 -mtune=generic -march=x86-64 -g -O1"
.LASF209:
	.string	"drmModeFreeResources"
.LASF141:
	.string	"DRM_MODE_SUBPIXEL_VERTICAL_RGB"
.LASF31:
	.string	"_IO_buf_end"
.LASF118:
	.string	"name"
.LASF195:
	.string	"fprintf"
.LASF197:
	.string	"drmIoctl"
.LASF162:
	.string	"modeset_dev"
.LASF58:
	.string	"stderr"
.LASF8:
	.string	"short int"
.LASF185:
	.string	"modeset_find_crtc"
.LASF202:
	.string	"drmModeGetEncoder"
.LASF22:
	.string	"uint64_t"
.LASF125:
	.string	"mode"
.LASF135:
	.string	"DRM_MODE_DISCONNECTED"
.LASF41:
	.string	"_vtable_offset"
.LASF187:
	.string	"modeset_prepare"
.LASF62:
	.string	"_sys_errlist"
.LASF169:
	.string	"iter"
.LASF148:
	.string	"connector_type_id"
.LASF66:
	.string	"__daylight"
.LASF129:
	.string	"encoder_id"
.LASF115:
	.string	"vscan"
.LASF25:
	.string	"_IO_read_end"
.LASF71:
	.string	"getdate_err"
.LASF127:
	.string	"drmModeCrtc"
.LASF166:
	.string	"crtc"
.LASF210:
	.string	"syscall"
.LASF158:
	.string	"drmModeConnector"
.LASF119:
	.string	"drmModeModeInfo"
.LASF21:
	.string	"uint32_t"
.LASF156:
	.string	"props"
.LASF37:
	.string	"_fileno"
.LASF46:
	.string	"_wide_data"
.LASF74:
	.string	"optarg"
.LASF205:
	.string	"free"
.LASF167:
	.string	"saved_crtc"
.LASF4:
	.string	"short unsigned int"
.LASF57:
	.string	"stdout"
.LASF86:
	.string	"pitch"
.LASF201:
	.string	"drmModeRmFB"
.LASF28:
	.string	"_IO_write_ptr"
.LASF93:
	.string	"count_crtcs"
.LASF11:
	.string	"__int32_t"
.LASF69:
	.string	"daylight"
.LASF191:
	.string	"open64"
.LASF137:
	.string	"drmModeConnection"
.LASF163:
	.string	"next"
.LASF196:
	.string	"close"
.LASF198:
	.string	"drmModeAddFB"
.LASF157:
	.string	"prop_values"
.LASF151:
	.string	"mmHeight"
.LASF82:
	.string	"height"
	.ident	"GCC: (GNU) 8.2.1 20181127"
	.section	.note.GNU-stack,"",@progbits
