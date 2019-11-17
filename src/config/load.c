#include "config.h"

#include <stdio.h>

Config *config_load(const char *path, int *has_error)
{
    Config *config = NULL;

    Vector *tokens = config_tokenize(path, has_error);

    if (!(*has_error))
    {
        config = config_parse(tokens, has_error);
    }

    vector_free(tokens, VECTOR_FREE_REFERENCE);

    return config;
}
