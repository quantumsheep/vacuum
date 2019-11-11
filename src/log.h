#ifndef LOG_H
#define LOG_H

void log_init(const char *path);

void log_print(const char *fmt, ...);
void log_print_timed(const char *fmt, ...);

#endif
