#include "process.h"
#include "../core/kernel.h"
#include "../util/vector.h"
#include "instruction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void program_init(const char *path)
{
    if (!path) {
        printf("erro, sem nome de arquivo\n");
        return;
    }

    pdata_t *process = malloc(sizeof *process);
    process->status = NEW;
    process->semaphore = vector_create(sizeof(char *));
    struct vector code = vector_create(sizeof(instruction_t));

    FILE *fp = fopen(path, "r");

    if (!fp) {
        printf("arquivo não encontrado\n");
        vector_destroy(&process->semaphore);
        vector_destroy(&code);
        free(process);
        return;
    }

    char buffer[50];

    // get nome
    fgets(buffer, sizeof buffer, fp);
    strcpy(process->name, buffer);

    // get segmento
    fgets(buffer, sizeof buffer, fp);
    process->seg_id = atoi(buffer);

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
        vector_push_back(&process->semaphore, &aux);
        i = i + len;
    }

    int rtime = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        // caso a linha esteja vazia
        if (!strlen(buffer))
            continue;

        // caso a linha estaja em branco
        if (!strcmp(buffer, "\n") || !strcmp(buffer, "\r\n"))
            continue;

        instruction_t *inst_aux = inst_read(buffer);
        rtime = rtime + inst_aux->value;
        vector_push_back(&code, inst_aux);
        free(inst_aux);
    }

    process->remainig_time = rtime;
    process->pc = 0;
    process->quantum_time = QT / process->priority;

    process->pid = get_next_pid();

    struct memory_request request = { process, &code };

    syscall(MEM_LOAD_REQ, &request);

    free(process);
    vector_destroy(&code);
}

void program_destroy(pdata_t *program)
{
    for (int i = 0; i < program->semaphore.length; i++) {
        free(VEC_GET(program->semaphore, i, char *));
    }
    vector_destroy(&(program->semaphore));
}
