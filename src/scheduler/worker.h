#ifndef SCHEDULER_WORKER_H
#define SCHEDULER_WORKER_H

#include "../datatypes/vector.h"
#include "job.h"

typedef struct worker_t Worker;
struct worker_t
{
    pthread_t thread;
    int running;

    Vector *jobs;
    unsigned int interval;
};

Worker *worker_init(unsigned int interval);
void worker_free(Worker *worker);

void worker_run(Worker *worker);
void worker_stop(Worker *worker);
int worker_wait(Worker *worker);

void worker_add(Worker *worker, Job *job);

#endif
