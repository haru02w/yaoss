#pragma once

#include "byte.h"
#include <stddef.h>

#define VEC_GET(vec, index, type) (((type *)vec.data)[index])
struct vector {
    size_t length;
    size_t capacity;
    size_t data_size;
    byte *data;
};

struct vector vector_create(size_t data_size);

void vector_destroy(struct vector *vec);

void vector_push_back(struct vector *vec, void *data);

void vector_pop_back(struct vector *vec);

void *vector_get(struct vector *vec, size_t index);
