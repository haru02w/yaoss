#include "instruction.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Converte uma string no formato "opcode time" ou "semOP(sem)" para um
 * ponteiro de instruction_t
 *
 * @param string Uma string lida no arquivo do programa
 * @return instruction_t*
 */
instruction_t *inst_read(char *string)
{
    if (string == NULL) {
        printf("string Vazia\n");
        return NULL;
    }
    int i;
    int code = 0;
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
        break;
    }
    case 1: {
        inst->op = EXEC;
        inst->value = atoi(&(string[i]));
        break;
    }
    case 2: {
        inst->op = READ;
        inst->value = atoi(&(string[i]));
        break;
    }
    case 5: {
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
        break;
    }
    case 7: {
        inst->op = PRINT;
        inst->value = atoi(&(string[i]));
        break;
    }
    default:
        puts("ERRO de opcode");
    }

    return inst;
}
