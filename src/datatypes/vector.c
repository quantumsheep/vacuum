#include "vector.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Vector *vector_init()
{
    Vector *vec = (Vector *)malloc(sizeof(Vector));
    vec->capacity = 32;
    vec->length = 0;

    vec->values = (void **)malloc(sizeof(void *) * vec->capacity);
    memset(vec->values, 0, vec->capacity);

    return vec;
}

void *vector_get(const Vector *vec, size_t index)
{
    if (index < vec->length)
    {
        return vec->values[index];
    }

    return NULL;
}

char *vector_get_string(const Vector *vec, size_t index)
{
    return (char *)vector_get(vec, index);
}

int *vector_get_int(const Vector *vec, size_t index)
{
    return (int *)vector_get(vec, index);
}

size_t vector_push_ref(Vector *vec, void *ptr)
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

size_t vector_push(Vector *vec, const void *value, size_t size)
{
    void *ptr = (void *)calloc(sizeof(uint8_t), size);
    memcpy(ptr, value, size);

    return vector_push_ref(vec, ptr);
}

size_t vector_push_string(Vector *vec, const char *value)
{
    return vector_push(vec, value, sizeof(char) * (strlen(value) + 1));
}

size_t vector_push_int(Vector *vec, int value)
{
    return vector_push(vec, &value, sizeof(int));
}

void vector_concat(Vector *destination, const Vector *source)
{
    for (size_t i = 0; i < source->length; i++)
    {
        vector_push_ref(destination, vector_get(source, i));
    }
}

int vector_includes_string(const Vector *vec, const char *str)
{
    for (size_t i = 0; i < vec->length; i++)
    {
        const char *target = (const char *)vector_get(vec, i);
        if (strcmp(target, str) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int vector_includes_string_n(const Vector *vec, const char *str, size_t size)
{
    for (size_t i = 0; i < vec->length; i++)
    {
        const char *target = (const char *)vector_get(vec, i);
        if (strncmp(target, str, size) == 0)
        {
            return 1;
        }
    }

    return 0;
}

void vector_remove(Vector *vec, size_t index, VectorDeleteMode mode)
{
    if (index < vec->length)
    {
        if (mode == VECTOR_FREE_REFERENCE && vec->values[index] != NULL)
        {
            free(vec->values[index]);
        }

        for (size_t i = index; i < (vec->length - 1); i++)
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
        for (size_t i = 0; i < vec->length; i++)
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
