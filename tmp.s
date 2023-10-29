
.equ max_len, 30
.section .data
hello_world: .asciz "Hello World!\n"

.global _start
.section .text
_start:
  movq $1, %rax # sys_write
  movq $1, %rdi # stdout
  leaq hello_world, %rsi
  movq $max_len, %rdx

  syscall

  movq $60, %rax
  xorq %rdi, %rdi
  syscall

