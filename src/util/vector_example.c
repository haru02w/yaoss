/*
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    struct vector vec = vector_create(sizeof(int));
    int i = 3;
    vector_push_back(&vec, &i);
    i = 5;
    vector_push_back(&vec, &i);
    i = 7;
    vector_push_back(&vec, &i);

    for (size_t i = 0; i < vec.length; i++) {
        printf("%zu, %d\n", i, VEC_GET(vec, i, int));
        // or
        printf("%zu, %d\n", i, *(int *)vector_get(&vec, i));
    }

    return EXIT_SUCCESS;
}
*/
