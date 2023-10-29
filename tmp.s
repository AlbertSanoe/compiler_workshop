  .globl main
main:
  mov $4, %rax
  push %rax
  mov $3, %rax
  pop %rdi
  cmp %rdi, %rax
  setne %al
  movzb %al, %rax
  retq
