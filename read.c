#include <stdio.h>
#include <stdlib.h> /* atoi */

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "The number of arguments is incorrect.\n");
        return 1;
    }

    printf(".globl main\n");
    printf("main:\n");
    printf("\tmovq $%d,%%rax\n", atoi(argv[1]));
    printf("\tret\n");
    return 0;
}
