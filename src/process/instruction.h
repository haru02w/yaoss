#ifndef PROCESS_INSTRUCTION_H_
#define PROCESS_INSTRUCTION_H_
#include <stdio.h>
#include <stdlib.h>

typedef enum { EXEC, READ, WRITE, V, P, PRINT } opcode_t;

typedef struct instruction {
    opcode_t op;
    int value;
    char sem[10];
} instruction_t;

instruction_t *inst_read(char *string);

#endif // PROCESS_INSTRUCTION_H_
