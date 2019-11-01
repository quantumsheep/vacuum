#ifndef SCHEDULER_JOB_H
#define SCHEDULER_JOB_H

#include <pthread.h>

typedef struct job_t Job;
struct job_t
{
    pthread_t thread;
    int running;

    void (*f)(void *);
    void *args;
};

Job *job_init(void (*f)(void *), void *args);
void job_free(Job *job);

void job_run(Job *job);

#endif
