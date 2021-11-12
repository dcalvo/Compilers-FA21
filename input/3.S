/* 2 vregs with storage allocated */
	.section .rodata
s_readint_fmt: .string "%ld"
s_writeint_fmt: .string "%ld\n"
	.section .bss
	.align 8
s_readbuf: .space 8
	.section .text
	.globl main
main:
	subq $24, %rsp
	movq $s_readint_fmt, %rdi   /* readi vr1 */
	leaq 8(%rsp), %rsi
	call scanf
	movq 8(%rsp), %r10          /* mov vr0, vr1 */
	movq %r10, 0(%rsp)
	addq $24, %rsp
	movq $0, %rax
	ret
