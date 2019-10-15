#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct vector_t Vector;
struct vector_t
{
    size_t length;
    size_t capacity;

    char **values;
};

Vector *vector_init();
int vector_push(Vector *vec, const char *value);

#endif
