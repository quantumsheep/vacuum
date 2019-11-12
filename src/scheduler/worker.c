#include "worker.h"

Worker *worker_init(unsigned int interval)
{
    Worker *worker = (Worker *)malloc(sizeof(Worker));

    worker->running = 0;

    worker->jobs = vector_init();
    worker->interval = interval;

    return worker;
}

void worker_free(Worker *worker)
{
    if (worker == NULL)
        return;

    for (size_t i = 0; i < worker->jobs->length; i++)
    {
        Job *job = (Job *)vector_get(worker->jobs, i);
        job_free(job);
    }

    vector_free(worker->jobs, VECTOR_KEEP_REFERENCE);

    free(worker);
}

static void worker_thread(volatile Worker *worker)
{
    time_t start = time(NULL);

    while (worker->running)
    {
        time_t now = time(NULL);

        if ((now - start) >= worker->interval)
        {
            for (size_t i = 0; i < worker->jobs->length; i++)
            {
                Job *job = (Job *)vector_get(worker->jobs, i);
                job_run(job);
            }

            start = time(NULL);
        }
    }
}

void worker_run(Worker *worker)
{
    worker->running = 1;
    pthread_create(&worker->thread, NULL, (void *)worker_thread, (void *)worker);
}

void worker_stop(Worker *worker)
{
    worker->running = 0;
}

int worker_wait(Worker *worker)
{
    return pthread_join(worker->thread, NULL);
}

void worker_add(Worker *worker, Job *job)
{
    vector_push_ref(worker->jobs, job);
}
