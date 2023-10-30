  .globl main
main:
  mov $40, %rax
  push %rax
  mov $3, %rax
  pop %rdi
  cmp %rdi, %rax
  sete %al
  movzb %al, %rax
  retq
