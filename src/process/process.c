#include "process.h"
#include "../core/kernel.h"
#include "../util/vector.h"
#include "instruction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief Function that reads a program file in the directory
/// @param path Directory to the program file
void program_init(const char *path)
{
    // If path is NULL, return
    if (!path) {
        return;
    }

    pdata_t *process = malloc(sizeof *process);
    process->status = NEW;
    process->semaphore = vector_create(sizeof(char *));
    struct vector code = vector_create(sizeof(instruction_t));

    FILE *fp = fopen(path, "r");

    // If file not found, print error message, free allocated memory, and return
    if (!fp) {
        free(process);
        return;
    }

    // From here, the program file is read and put into the process data struct
    char buffer[255];

    // Get name
    fgets(buffer, sizeof buffer, fp);
    buffer[strlen(buffer) - 1] = '\0';
    strcpy(process->name, buffer);

    // Get segment id
    fgets(buffer, sizeof buffer, fp);
    process->seg_id = atoi(buffer);

    // Get priority
    fgets(buffer, sizeof buffer, fp);
    process->priority = atoi(buffer);

    // Get segment size
    fgets(buffer, sizeof buffer, fp);
    process->seg_size = atoi(buffer);

    // Get semaphore list
    fgets(buffer, sizeof buffer, fp);

    // FIXME: Does the last semaphore store a \n?
    // Read semaphore list
    for (int i = 0; buffer[i] != '\0'; i++) {
        char buff[255];
        sscanf(&(buffer[i]), "%[^ \n]s", buff);

        int len = strlen(buff);
        char *aux = malloc(len + 1);
        strcpy(aux, buff);
        vector_push_back(&process->semaphore, &aux);
        semaphore_add(aux);
        i += len;
    }

    // Read the process list
    int rtime = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        // If the line is empty
        if (!strlen(buffer))
            continue;

        // If the line is blank
        if (!strcmp(buffer, "\n") || !strcmp(buffer, "\r\n"))
            continue;

        instruction_t *inst_aux = inst_read(buffer);
        rtime = rtime + inst_aux->value;
        vector_push_back(&code, inst_aux);
        free(inst_aux);
    }

    process->remaining_time = rtime;
    process->maximum_time = rtime;
    process->pc = 0;
    process->code_size = code.length;
    process->quantum_time = QT / process->priority;

    process->pid = get_next_pid();

    struct memory_request request = { process, &code };

    syscall(MEM_LOAD_REQ, &request);
}

/// @brief Function to free the memory allocated for the program
/// @param program Pointer to the program to be destroyed
void program_destroy(pdata_t *program)
{
    // Free each char pointer in the vector of semaphores
    for (size_t i = 0; i < program->semaphore.length; i++) {
        free(VEC_GET(program->semaphore, i, char *));
    }
    vector_destroy(&(program->semaphore));
    free(program);
}
