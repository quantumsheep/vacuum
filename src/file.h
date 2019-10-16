#ifndef FILE_H
#define FILE_H

#include <stdio.h>

long file_length(FILE *f);
char *file_read(const char *path);

#endif
