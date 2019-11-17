#include "config.h"

void config_action_free(ConfigAction *action)
{
    if (action == NULL)
        return;

    map_free(action->options, MAP_FREE_REFERENCE);

    if (action->url != NULL)
    {
        free(action->url);
    }

    free(action);
}

void config_task_free(ConfigTask *task)
{
    if (task == NULL)
        return;

    vector_free(task->actions, VECTOR_FREE_REFERENCE);

    free(task);
}

void config_free(Config *config)
{
    {
        MapNode *node = config->actions->first;

        while (node != NULL)
        {
            config_action_free((ConfigAction *)node->value);
            node->value = NULL;

            node = node->next;
        }

        map_free(config->actions, MAP_KEEP_REFERENCE);
    }

    {
        MapNode *node = config->tasks->first;

        while (node != NULL)
        {
            config_task_free((ConfigTask *)node->value);
            node->value = NULL;

            node = node->next;
        }

        map_free(config->tasks, MAP_KEEP_REFERENCE);
    }

    free(config);
}
