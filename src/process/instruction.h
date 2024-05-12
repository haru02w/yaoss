#pragma once
#include <stddef.h>

/// @brief Enum for instruction opcode
typedef enum { EXEC, READ, WRITE, V, P, PRINT } opcode_t;

/// @brief Struct for instructions of a process
typedef struct instruction {
    // Opcode of instruction
    opcode_t op;

    // Time necessary for executing an instruction
    size_t value;

    // Semaphore used (if used)
    char sem[10];
} instruction_t;

/// @brief Function that converts a string containing "opcode time", "V(sem)" or
/// "P(sem)" into an instruction_t struct.
///
/// @param string String read from the program archive
/// @return Pointer to instruction_t struct
instruction_t *inst_read(char *string);
