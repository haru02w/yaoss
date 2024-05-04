#include "process.h"
#include "../util/list.h"
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
    process->semaphore = list_create(comp_sem);
    process->instruction = list_create(comp_inst);

    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("arquivo não encontrado\n");
        list_free(process->semaphore);
        list_free(process->instruction);
        free(process);
        return NULL;
    }

    char buffer[50];
    fgets(buffer, sizeof buffer, fp);
    strcpy(process->nome, buffer);

    fgets(buffer, sizeof buffer, fp);
    process->seg = atoi(buffer);

    fgets(buffer, sizeof buffer, fp);
    process->priority = atoi(buffer);

    fgets(buffer, sizeof buffer, fp);
    process->seg_size = atoi(buffer);

    fgets(buffer, sizeof buffer, fp);
    // FIXME: O ultimo semaforo armazena um \n

    for (int i = 0; buffer[i] != '\0'; i++) {
        char buff[50];
        sscanf(&(buffer[i]), "%[^ ]s", buff);

        int len = strlen(buff);
        char *aux = malloc(len + 1);
        strcpy(aux, buff);
        list_add(process->semaphore, (void *)aux);
        i = i + len;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        // caso a linha esteja vazia
        if (!strlen(buffer))
            continue;

        // caso a linha estaja em branco
        if (!strcmp(buffer, "\n") || !strcmp(buffer, "\r\n"))
            continue;

        list_add(process->instruction, (void *)inst_read(buffer));
    }

    // TODO: Get pid(process id)

    return process;
}
