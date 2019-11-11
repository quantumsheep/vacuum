#include "log.h"

#include "parallel/file_writing.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

void log_print_timed(const char *fmt, ...)
{
    time_t timer;
    char formated_date[26];
    struct tm *tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(formated_date, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    char *new_fmt = (char *)calloc(sizeof(char), sizeof(formated_date) + strlen(fmt) + 1 + 3);
    sprintf(new_fmt, "[%s] %s", formated_date, fmt);

    va_list args;
    va_start(args, fmt);

    parallel_file_vwrite(log_path, "a", new_fmt, args);

    va_end(args);

    free(new_fmt);
}
