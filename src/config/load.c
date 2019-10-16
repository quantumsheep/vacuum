#include "../config.h"

#include <stdio.h>

Config config_load(const char *path)
{
    Vector *tokens = config_tokenize(path);
    Config config = config_parse(tokens);

    {
        MapNode *node = config.actions->first;

        while (node)
        {
            printf("Action '%s':\n", node->key);

            ConfigAction *action = (ConfigAction *)node->value;

            printf("  [url]: %s\n", action->url);
            printf("  options:\n");

            MapNode *option = action->options->first;

            while (option)
            {
                char *value = (char *)option->value;

                printf("    [%s]: %s\n", option->key, value);

                option = option->next;
            }

            node = node->next;
        }
    }

    {
        MapNode *node = config.tasks->first;

        while (node)
        {
            printf("Task '%s':\n", node->key);

            ConfigTask *task = (ConfigTask *)node->value;

            printf("  [hours]: %d\n", task->hours);
            printf("  [minutes]: %d\n", task->minutes);
            printf("  [seconds]: %d\n", task->seconds);
            printf("  actions:\n");

            for (int i = 0; i < task->actions->length; i++)
            {
                char *action = vector_get_string(task->actions, i);

                printf("    - '%s'\n", action);
            }

            node = node->next;
        }
    }

    return config;
}
