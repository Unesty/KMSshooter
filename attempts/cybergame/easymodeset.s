cardpath:
.string "/dev/dri/card0"
wrgc:
.string "bad card\n"
.globl _start
.type _start,@function
_start:
mov $2,%rax
lea cardpath(%rip),%rdi
mov $524290,%rsi
syscall
mov $72,%rax
mov $3,%rdi
mov $2,%rsi
mov $1,%rdx
syscall
mov $16,%rax
mov $3,%rdi
mov $-1072667636,%rsi
#subq $136,%rsp
leaq (%rsp),%rdx
syscall
cmpq $0,(%rsp)
je bad_card
mov $16,%rax
mov $3,%rdi
mov $-1069521760,%rsi
syscall
mov %rax,%rdi
mov $60,%rax
syscall
bad_card:
mov $1,%rax
mov $1,%rdi
leaq wrgc(%rip),%rsi
mov $10,%rdx
syscall
mov $60,%rax
syscall
