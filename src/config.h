#ifndef CONFIG_H
#define CONFIG_H

typedef struct config_action_t ConfigAction;
struct config_action_t
{
    char *url;

    Map *options;
};

typedef struct config_task_t ConfigTask;
struct config_task_t
{
    int hours;
    int minutes;
    int seconds;

    ConfigAction **actions;
};

typedef struct config_t Config;
struct config_t
{
    Map *actions;
    Map *tasks;
};

Map *config_parse(const char *path);

#endif
