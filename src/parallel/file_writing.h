#ifndef PARALLEL_FILE_WRITING_H
#define PARALLEL_FILE_WRITING_H

#include <stdarg.h>

int parallel_file_vwrite(const char *path, const char *mode, const char *fmt, va_list list);
int parallel_file_write(const char *path, const char *mode, const char *fmt, ...);

#endif
