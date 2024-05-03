#include <stdio.h>
#include <stdlib.h>
#include "process/instruction.h"

const char *tostring(opcode_t op){
    switch(op){
        case EXEC: return "exec";
        case READ: return "read";
        case WRITE: return "write";
        case P: return "P";
        case V: return "V";
        case PRINT: return "print";
        default: return "deu ruim";
    }
}

int main(void)
{
    instruction_t *inst;
    char *str = "P(semafarovermelho)\0";
    inst = inst_read(str);
    printf("%s %d %s\n", tostring(inst->op), inst->value, inst->sem);

    free(inst);
    return EXIT_SUCCESS;
}
