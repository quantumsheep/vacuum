#include "file_writing.h"

#include "../datatypes/map.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Map *actives = NULL; // Map<char *, pthread_mutex_t *>

static pthread_mutex_t *file_mutex(const char *path)
{
    static pthread_mutex_t loading;

    pthread_mutex_lock(&loading);

    if (actives == NULL)
        actives = map_init();

    pthread_mutex_t *mutex = (pthread_mutex_t *)map_get_value(actives, path);

    if (mutex == NULL)
    {
        mutex = malloc(sizeof(pthread_mutex_t));
        if (pthread_mutex_init(mutex, NULL) == 0)
        {
            map_set_ref(actives, path, mutex);
        }
        else
        {
            free(mutex);
            mutex = NULL;
        }
    }

    pthread_mutex_lock(&loading);

    return mutex;
}

int parallel_file_vwrite(const char *path, const char *mode, const char *fmt, va_list list)
{
    pthread_mutex_t *lock = file_mutex(path);

    int passed = 0;

    pthread_mutex_lock(lock);

    FILE *f = fopen(path, mode);
    if (f != NULL)
    {
        vfprintf(f, fmt, list);
        fclose(f);

        passed = 1;
    }

    pthread_mutex_unlock(lock);

    return passed;
}

int parallel_file_write(const char *path, const char *mode, const char *fmt, ...)
{
    va_list list;
    return parallel_file_vwrite(path, mode, fmt, list);
}
