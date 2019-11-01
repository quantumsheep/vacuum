#include "config.h"
#include "crawl.h"
#include "datatypes/vector.h"
#include "file.h"
#include "scheduler.h"

#include <stdio.h>

static Config config;
static Vector *workers = NULL;

void run_action(const char *name)
{
    MapNode *node = map_get(config.actions, name);

    if (node == NULL)
        return;

    ConfigAction *action = (ConfigAction *)node->value;
    char *max_depth_str = map_get_string(action->options, "max-depth");

    int max_depth = (max_depth_str != NULL) ? atoi(max_depth_str) : 1;

    Vector *visited = crawl(action->url, max_depth, NULL);
    vector_free(visited, VECTOR_FREE_REFERENCE);
}

void on_exit()
{
    for (size_t i = 0; i < workers->length; i++)
    {
        Worker *worker = (Worker *)vector_get(workers, i);
        worker_stop(worker);
    }
}

int main()
{
    int has_error = 0;
    config = config_load("config.sconf", &has_error);

    if (has_error)
    {
        return 1;
    }

    /**
     * Launch all the actions one by one
     */
    // for (MapNode *node = config.actions->first; node != NULL; node = node->next)
    // {
    //     run_action(node->key);
    // }

    workers = vector_init();
    signal(SIGINT, on_exit);

    /**
     * Start all the tasks
     */
    for (MapNode *node = config.tasks->first; node != NULL; node = node->next)
    {
        ConfigTask *task = (ConfigTask *)node->value;

        if (task->actions->length == 0)
            continue;

        Worker *worker = worker_init((task->hours * 3600) + (task->minutes * 60) + task->seconds);

        for (size_t i = 0; i < task->actions->length; i++)
        {
            char *action = vector_get(task->actions, i);
            Job *job = job_init((void *)run_action, action);

            worker_add(worker, job);
        }

        vector_push_ref(workers, worker);
        worker_run(worker);
    }

    for (size_t i = 0; i < workers->length; i++)
    {
        Worker *worker = (Worker *)vector_get(workers, i);
        worker_wait(worker);
        worker_free(worker);
    }

    vector_free(workers, VECTOR_KEEP_REFERENCE);

    config_free(config);

    return 0;
}
