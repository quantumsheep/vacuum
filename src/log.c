#include "log.h"

#include "parallel/file_writing.h"

#include <stdarg.h>
#include <stdio.h>

static const char *log_path = NULL;

void log_init(const char *path)
{
    log_path = path;
}

void log_print(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    parallel_file_vwrite(log_path, "a", fmt, args);

    va_end(args);
}
