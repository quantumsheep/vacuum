#include "job.h"

#include <stdlib.h>

Job *job_init(void (*f)(void *), void *args)
{
    Job *job = (Job *)malloc(sizeof(Job));

    job->running = 0;

    job->f = f;
    job->args = args;

    return job;
}

void job_free(Job *job)
{
    if (job == NULL)
        return;

    free(job);
}

void job_run(Job *job)
{
    job->f(job->args);
}
