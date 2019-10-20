#include "../config.h"

#include <stdlib.h>
#include <string.h>

ConfigOption *config_option_init(ConfigOptionType type)
{
    ConfigOption *option = malloc(sizeof(ConfigOption));

    option->type = type;
    option->value.str = NULL;

    return option;
}

ConfigOption *config_option_init_string(const char *s)
{
    ConfigOption *option = config_option_init(CONFIG_OPTION_STRING);

    option->value.str = (char *)calloc(sizeof(char), strlen(s) + 1);
    strcpy(option->value.str, s);

    return option;
}

ConfigOption *config_option_init_array(Vector *vec)
{
    ConfigOption *option = config_option_init(CONFIG_OPTION_ARRAY);

    option->value.arr = vec;

    return option;
}

ConfigAction *config_action_init(const char *url, Map *options)
{
    ConfigAction *action = (ConfigAction *)malloc(sizeof(ConfigAction));

    action->options = options;

    action->url = (char *)calloc(sizeof(char), strlen(url) + 1);
    strcpy(action->url, url);

    return action;
}

ConfigTask *config_task_init(int hours, int minutes, int seconds, Vector *actions)
{
    ConfigTask *task = (ConfigTask *)malloc(sizeof(ConfigTask));

    task->hours = hours;
    task->minutes = minutes;
    task->seconds = seconds;
    task->actions = actions;

    return task;
}
