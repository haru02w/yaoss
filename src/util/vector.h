#pragma once

#include "byte.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define VEC_GET(vec, index, type) (((type *)vec.data)[index])
struct vector {
    size_t length;
    size_t capacity;
    size_t data_size;
    byte *data;
};

static inline struct vector vector_create(size_t data_size)
{
    return (struct vector) { .data_size = data_size };
}

static inline void vector_destroy(struct vector *vec) { free(vec->data); }

void vector_push_back(struct vector *vec, void *data)
{
    if (vec->length + 1 > vec->capacity) {
        vec->capacity = vec->capacity <= 0 ? 1 : vec->capacity * 2;
        void *new = realloc(vec->data, vec->data_size * vec->capacity);
        assert(new != NULL);
        vec->data = (byte *)new;
    }
    memcpy(&vec->data[vec->length * vec->data_size], data, vec->data_size);
    vec->length++;
}

static inline void vector_pop_back(struct vector *vec) { vec->length--; }

static inline void *vector_get(struct vector *vec, size_t index)
{
    return &vec->data[index * vec->data_size];
}
