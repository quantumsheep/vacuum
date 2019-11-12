#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct vector_t Vector;
struct vector_t
{
    size_t length;
    size_t capacity;

    void **values;
};

Vector *vector_init();
void *vector_get(const Vector *vec, size_t index);
char *vector_get_string(const Vector *vec, size_t index);
int *vector_get_int(const Vector *vec, size_t index);

size_t vector_push_ref(Vector *vec, void *ptr);
size_t vector_push(Vector *vec, const void *value, size_t size);
size_t vector_push_string(Vector *vec, const char *value);
size_t vector_push_int(Vector *vec, int value);

void vector_concat(Vector *destination, const Vector *source);

int vector_includes_string(const Vector *vec, const char *str);
int vector_includes_string_n(const Vector *vec, const char *str, size_t size);

typedef enum vector_delete_mode_t VectorDeleteMode;
enum vector_delete_mode_t
{
    VECTOR_KEEP_REFERENCE,
    VECTOR_FREE_REFERENCE,
};

void vector_remove(Vector *vec, size_t index, VectorDeleteMode mode);

void vector_free(Vector *vec, VectorDeleteMode mode);

#endif
