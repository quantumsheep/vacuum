#include "vector.h"

#include <string.h>
#include <stdlib.h>

Vector *vector_init()
{
    Vector *vec = (Vector *)malloc(sizeof(Vector));
    vec->capacity = 32;
    vec->length = 0;

    vec->values = (void **)malloc(sizeof(void *) * vec->capacity);
    memset(vec->values, 0, vec->capacity);

    return vec;
}

void *vector_get(Vector *vec, unsigned int index)
{
    if (index < vec->length)
    {
        return vec->values[index];
    }

    return NULL;
}

char *vector_get_string(Vector *vec, unsigned int index)
{
    return (char *)vector_get(vec, index);
}

int *vector_get_int(Vector *vec, unsigned int index)
{
    return (int *)vector_get(vec, index);
}

unsigned int vector_push_ref(Vector *vec, void *ptr)
{
    if (vec->length == vec->capacity)
    {
        vec->capacity *= 2;
        vec->values = realloc(vec->values, sizeof(void *) * vec->capacity);
        memset(vec->values + vec->length, 0, vec->length);
    }

    vec->values[vec->length] = ptr;

    return vec->length++;
}

unsigned int vector_push(Vector *vec, const void *value, size_t size)
{
    void *ptr = (void *)calloc(sizeof(void), size);
    memcpy(ptr, value, size);

    return vector_push_ref(vec, ptr);
}

unsigned int vector_push_string(Vector *vec, const char *value)
{
    return vector_push(vec, value, sizeof(char) * (strlen(value) + 1));
}

unsigned int vector_push_int(Vector *vec, int value)
{
    return vector_push(vec, &value, sizeof(int));
}

void vector_remove(Vector *vec, unsigned int index, VectorDeleteMode mode)
{
    if (index < vec->length)
    {
        if (mode == VECTOR_FREE_REFERENCE && vec->values[index] != NULL)
        {
            free(vec->values[index]);
        }

        for (int i = index; i < (vec->length - 1); i++)
        {
            vec->values[i] = vec->values[i + 1];
        }

        vec->values[--vec->length] = NULL;
    }
}

void vector_free(Vector *vec, VectorDeleteMode mode)
{
    if (vec == NULL)
        return;

    if (mode == VECTOR_FREE_REFERENCE)
    {
        for (int i = 0; i < vec->length; i++)
        {
            void *value = vector_get(vec, i);

            if (value != NULL)
            {
                free(value);
            }
        }
    }

    free(vec->values);
    free(vec);
}
