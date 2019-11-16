#include "config.h"
#include "crawl.h"
#include "datatypes/vector.h"
#include "file.h"
#include "log.h"
#include "scheduler.h"

#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

static Config config;
static Vector *workers = NULL;

static int is_number(const char *str)
{
    while (*str != '\0' && isdigit(*str))
        str++;

    return *str == '\0';
}

static CrawlConfig generate_crawl_config(const ConfigAction *action)
{
    ConfigOption *max_depth_option = (ConfigOption *)map_get_value(action->options, "max-depth");
    ConfigOption *versioning_option = (ConfigOption *)map_get_value(action->options, "versioning");
    ConfigOption *type_option = (ConfigOption *)map_get_value(action->options, "type");

    static char base_prefix[] = "data";

    CrawlConfig config = (CrawlConfig){
        .max_depth = 0,
        .versioning = 0,
        .types = NULL,

        .storage_directory = NULL,
    };

    if (versioning_option != NULL)
    {
        if (versioning_option->type == CONFIG_OPTION_STRING)
        {
            config.versioning = strcmp(versioning_option->value.str, "on") == 0;
        }
        else
        {
            printf("'versioning' option must be a string (on / off).");
        }
    }

    if (type_option != NULL)
    {
        if (type_option->type == CONFIG_OPTION_ARRAY)
        {
            config.types = type_option->value.arr;
        }
        else
        {
            printf("'type' option must be an array.");
        }
    }

    if (max_depth_option != NULL)
    {
        if (max_depth_option->type == CONFIG_OPTION_STRING && is_number(max_depth_option->value.str))
        {
            config.max_depth = atoi(max_depth_option->value.str);
        }
        else
        {
            puts("Option 'max-depth' must be a valid number.");
        }
    }

    if (config.versioning)
    {
        time_t timer;
        struct tm *tm_info;

        time(&timer);
        tm_info = localtime(&timer);

        config.storage_directory = (char *)calloc(sizeof(char), (sizeof(base_prefix) - 1) + 11 + 1);
        strcat(config.storage_directory, base_prefix);
        strftime((config.storage_directory + sizeof(base_prefix) - 1), 12, "-%Y-%m-%d", tm_info);
    }
    else
    {
        config.storage_directory = (char *)malloc(sizeof(base_prefix));
        memcpy(config.storage_directory, base_prefix, sizeof(base_prefix));
    }

    return config;
}

void run_action(const char *name)
{
    MapNode *node = map_get(config.actions, name);

    if (node == NULL)
        return;

    ConfigAction *action = (ConfigAction *)node->value;
    CrawlConfig config = generate_crawl_config(action);

    log_print_timed("[%s] Crawling %s\n", name, action->url);

    Vector *visited = crawl(action->url, config);
    vector_free(visited, VECTOR_FREE_REFERENCE);

    free(config.storage_directory);
}

static void on_sigint_exit()
{
    for (size_t i = 0; i < workers->length; i++)
    {
        Worker *worker = (Worker *)vector_get(workers, i);
        worker_stop(worker);
    }
}

int main()
{
    log_init("log.txt");

    log_print_timed("Loading configuration file: %s\n", "config.sconf");

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
    signal(SIGINT, on_sigint_exit);

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
            Job *job = job_init((void (*)(void *))run_action, action);

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
