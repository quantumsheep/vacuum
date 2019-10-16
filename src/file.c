#include "file.h"

#include <stdlib.h>

long file_length(FILE *f)
{
    long origin = ftell(f);

    fseek(f, 0, SEEK_END);
    long len = ftell(f);

    fseek(f, origin, SEEK_SET);

    return len;
}

char *file_read(const char *path)
{
    FILE *f = fopen(path, "r");
    long len = file_length(f);

    char *out = (char *)calloc(sizeof(char), len + 1);

    fread(out, sizeof(char), len, f);

    fclose(f);

    return out;
}
