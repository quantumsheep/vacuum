#include "../config.h"

#include <stdlib.h>
#include <string.h>

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
