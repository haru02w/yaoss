#include "process.h"
#include "../util/vector.h"
#include "instruction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int comp_sem(const void *a, const void *b)
{
    char *sema = (char *)a;
    char *semb = (char *)b;
    return strcmp(sema, semb);
}

int comp_inst(const void *a, const void *b) { return 0; }

pdata_t *program_init(const char *path)
{
    if (!path) {
        printf("erro, sem nome de arquivo\n");
        return NULL;
    }

    pdata_t *process = malloc(sizeof *process);
    process->semaphore = vector_create(sizeof(char *));
    process->instruction = vector_create(sizeof(instruction_t));

    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("arquivo não encontrado\n");
        vector_destroy(&(process->semaphore));
        vector_destroy(&(process->instruction));
        free(process);
        return NULL;
    }

    char buffer[50];

    // get nome
    fgets(buffer, sizeof buffer, fp);
    strcpy(process->nome, buffer);

    // get segmento
    fgets(buffer, sizeof buffer, fp);
    process->seg = atoi(buffer);

    // get prioridade
    fgets(buffer, sizeof buffer, fp);
    process->priority = atoi(buffer);

    // get tamanho do segmento
    fgets(buffer, sizeof buffer, fp);
    process->seg_size = atoi(buffer);

    // get lista de semaforos
    fgets(buffer, sizeof buffer, fp);
    // FIXME: O ultimo semaforo armazena um \n

    for (int i = 0; buffer[i] != '\0'; i++) {
        char buff[50];
        sscanf(&(buffer[i]), "%[^ ]s", buff);

        int len = strlen(buff);
        char *aux = malloc(len + 1);
        strcpy(aux, buff);
        vector_push_back(&(process->semaphore), &aux);
        i = i + len;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        // caso a linha esteja vazia
        if (!strlen(buffer))
            continue;

        // caso a linha estaja em branco
        if (!strcmp(buffer, "\n") || !strcmp(buffer, "\r\n"))
            continue;

        instruction_t *inst_aux = inst_read(buffer);
        vector_push_back(&(process->instruction), inst_aux);
        free(inst_aux);
    }

    process->pc = 0;

    // TODO: Get pid(process id)

    return process;
}

void program_destroy(pdata_t *program)
{
    for (int i = 0; i < program->semaphore.length; i++) {
        free(VEC_GET(program->semaphore, i, char *));
    }
    vector_destroy(&(program->semaphore));
    vector_destroy(&(program->instruction));
}
