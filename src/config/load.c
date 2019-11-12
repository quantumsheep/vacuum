#include "../config.h"

#include <stdio.h>

Config config_load(const char *path, int *has_error)
{
    Config config;

    Vector *tokens = config_tokenize(path, has_error);

    if (!(*has_error))
    {
        config = config_parse(tokens, has_error);
    }

    vector_free(tokens, VECTOR_FREE_REFERENCE);

    if (!(*has_error))
    {
        for (MapNode *node = config.actions->first; node != NULL; node = node->next)
        {
            ConfigAction *action = (ConfigAction *)node->value;

            printf("Action '%s':\n", node->key);
            printf("  [url]: %s\n", action->url);
            printf("  options:\n");

            for (MapNode *option_node = action->options->first; option_node != NULL; option_node = option_node->next)
            {
                ConfigOption *option = (ConfigOption *)option_node->value;

                if (option->type == CONFIG_OPTION_STRING)
                {
                    printf("    [%s]: '%s'\n", option_node->key, option->value.str);
                }
                else if (option->type == CONFIG_OPTION_ARRAY)
                {
                    printf("    [%s]:\n", option_node->key);

                    for (size_t i = 0; i < option->value.arr->length; i++)
                    {
                        printf("      [%lu]: '%s'\n", i, vector_get_string(option->value.arr, i));
                    }
                }
            }
        }

        for (MapNode *node = config.tasks->first; node != NULL; node = node->next)
        {
            ConfigTask *task = (ConfigTask *)node->value;

            printf("Task '%s':\n", node->key);
            printf("  [hours]: %d\n", task->hours);
            printf("  [minutes]: %d\n", task->minutes);
            printf("  [seconds]: %d\n", task->seconds);
            printf("  actions:\n");

            for (size_t i = 0; i < task->actions->length; i++)
            {
                char *action = vector_get_string(task->actions, i);
                printf("    - '%s'\n", action);
            }
        }
    }

    return config;
}
