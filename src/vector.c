#include "vector.h"

Vector *vector_init()
{
    Vector *vec = malloc(sizeof(Vector));
    vec->capacity = 32;
    vec->length = 0;

    vec->values = malloc(sizeof(char *) * vec->capacity);

    return vec;
}

int vector_push(Vector *vec, const char *value)
{
    if (vec->length == vec->capacity)
    {
        vec->capacity *= 2;
        vec->values = realloc(vec->values, sizeof(char *) * vec->capacity);
    }

    vec->values[vec->length] = calloc(sizeof(char), strlen(value) + 1);
    strcpy(vec->values[vec->length], value);

    vec->length++;
}
