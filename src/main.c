#include "process/instruction.h"
#include "process/process.h"
#include <stdio.h>
#include <stdlib.h>

const char *tostring(opcode_t op)
{
    switch (op) {
    case EXEC:
        return "exec";
    case READ:
        return "read";
    case WRITE:
        return "write";
    case P:
        return "P";
    case V:
        return "V";
    case PRINT:
        return "print";
    default:
        return "deu ruim";
    }
}

int main(void)
{
    const char *path = "sint2";
    pdata_t *process = program_init(path);

    printf("%s", process->nome);
    printf("%d\n", process->seg);
    printf("%d\n", process->priority);
    printf("%d\n", process->seg_size);

    while (process->semaphore->size != 0) {
        // printf("--debug semaphore list size = %lu --\n",
        // process->semaphore->size);
        char *aux = (char *)process->semaphore->head->data;
        printf("%s ", aux);
        list_remove(process->semaphore, (void *)aux);
        free(aux);
    }
    printf("\n");
    list_free(process->semaphore);

    while (process->instruction->size != 0) {
        // printf("--debug instruction list size = %lu --\n",
        // process->instruction->size);
        instruction_t *inst = (instruction_t *)process->instruction->head->data;
        if ((inst->op == P) || (inst->op == V)) {
            printf("%s(%s)\n", tostring(inst->op), inst->sem);
        } else {
            printf("%s %d\n", tostring(inst->op), inst->value);
        }
        list_remove(process->instruction, NULL);
        free(inst);
    }
    list_free(process->instruction);

    free(process);
    return EXIT_SUCCESS;
}
