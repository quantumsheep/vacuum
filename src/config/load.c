#include "../config.h"

#include <stdio.h>

Config config_load(const char *path)
{
    Vector *tokens = config_tokenize(path);

    for (int i = 0; i < tokens->length; i++)
    {
        ConfigToken *token = (ConfigToken *)vector_get(tokens, i);
        printf("line %d - type %d: '%s'\n", token->line, token->type, token->value);
    }

    return config_parse(tokens);
}
