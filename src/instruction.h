#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_
#include <stdlib.h>
#include <stdio.h>

typedef enum { EXEC, READ, WRITE, V, P, PRINT } opcode_t;

typedef struct instruction {
    opcode_t op;
    int value;
	char *sem;
} instruction_t;

instruction_t *inst_read(char *string);

#endif // INSTRUCTION_H_
