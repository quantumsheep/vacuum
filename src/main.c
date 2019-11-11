#include "config.h"
#include "crawl.h"
#include "datatypes/vector.h"
#include "file.h"
#include "scheduler.h"

#include <ctype.h>
#include <stdio.h>

static Config config;
static Vector *workers = NULL;

static int is_number(const char *str)
{
    while (*str != '\0' && isdigit(*str))
        str++;

    return *str == '\0';
}

void run_action(const char *name)
{
    MapNode *node = map_get(config.actions, name);

    if (node == NULL)
        return;

    ConfigAction *action = (ConfigAction *)node->value;

    ConfigOption *max_depth_option = (ConfigOption *)map_get_value(action->options, "max-depth");
    ConfigOption *versioning_option = (ConfigOption *)map_get_value(action->options, "versioning");

    CrawlConfig config;
    config.max_depth = 0;

    if (max_depth_option != NULL)
    {
        if (max_depth_option->type == CONFIG_OPTION_ARRAY || !is_number(max_depth_option->value.str))
        {
            puts("Option 'max-depth' must be a valid number.");
        }
        else
        {
            config.max_depth = atoi(max_depth_option->value.str);
        }
    }

    config.versioning = (versioning_option != NULL) &&
                        (versioning_option->type == CONFIG_OPTION_STRING) &&
                        (strcmp(versioning_option->value.str, "on") == 0);

    Vector *visited = crawl(action->url, config, NULL);
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
