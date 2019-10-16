#include "../config.h"

#include <stdio.h>

Config config_load(const char *path)
{
    Vector *tokens = config_tokenize(path);
    return config_parse(tokens);
}
