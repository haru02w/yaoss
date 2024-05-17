#include "instruction.h"
#include <stdio.h>
#include <stdlib.h>

/// @brief Function that converts a string containing "opcode time", "V(sem)" or
/// "P(sem)" into an instruction_t struct.
///
/// @param string String read from the program archive
/// @return Pointer to instruction_t struct
instruction_t *inst_read(char *string)
{
    // Return NULL if string is empty
    if (string == NULL) {
        printf("Empty string\n");
        return NULL;
    }
    int i;
    int code = 0;
    // Read character by character until ' ' or '(' that represents the end of
    // every opcode in the archive and calculate a "hash code"
    for (i = 0; string[i] != ' ' && string[i] != '('; i++) {
        code = (code + string[i]) % 10;
    }
    i++;
    instruction_t *inst = (instruction_t *)malloc(sizeof(instruction_t));
    switch (code) {
    case 0: { // P()
        inst->op = P;
        int j;
        for (j = i; string[j] != ')' && j - i < 9; j++) {
            inst->sem[j - i] = string[j];
        }
        inst->sem[j - i] = '\0';
        inst->value = 200;
        break;
    }
    case 1: { // EXEC
        inst->op = EXEC;
        inst->value = atoi(&(string[i]));
        break;
    }
    case 2: { // READ
        inst->op = READ;
        inst->value = atoi(&(string[i]));
        break;
    }
    case 5: { // WRITE
        inst->op = WRITE;
        inst->value = atoi(&(string[i]));
        break;
    }
    case 6: { // V()
        inst->op = V;
        int j;
        for (j = i; string[j] != ')' && j - i < 9; j++) {
            inst->sem[j - i] = string[j];
        }
        inst->sem[j - i] = '\0';
        inst->value = 200;
        break;
    }
    case 7: { // PRINT
        inst->op = PRINT;
        inst->value = atoi(&(string[i]));
        break;
    }
    default:
        puts("Opcode error");
        free(inst);
        return NULL;
    }

    return inst;
}
