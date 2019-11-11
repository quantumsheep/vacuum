#ifndef PARALLEL_FILE_WRITING_H
#define PARALLEL_FILE_WRITING_H

int parallel_file_append(const char *path, const char *data);
int parallel_file_write(const char *path, const char *data);

#endif
