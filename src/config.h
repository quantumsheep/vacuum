#ifndef CONFIG_H
#define CONFIG_H

#include "map.h"
#include "vector.h"

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

    Vector *actions;
};

typedef struct config_t Config;
struct config_t
{
    Map *actions;
    Map *tasks;
};

typedef enum config_token_type_t ConfigTokenType;
enum config_token_type_t
{
    TOKEN_ACTION,          // =
    TOKEN_TASK,            // ==
    TOKEN_ARRAY_SEPARATOR, // ,
    TOKEN_OPENING_ARRAY,   // (
    TOKEN_CLOSING_ARRAY,   // )
    TOKEN_OPENING_OPTION,  // {
    TOKEN_CLOSING_OPTION,  // }
    TOKEN_OPTION_ARROW,    // ->
    TOKEN_SEPARATOR,       // +
    TOKEN_STRING,          // any string
};

typedef struct config_token_item_t ConfigToken;
struct config_token_item_t
{
    ConfigTokenType type;
    char *value;
    int line;
};

ConfigAction *config_action_init(const char *url, Map *options);
ConfigTask *config_task_init(int hours, int minutes, int seconds, Vector *actions);

Config config_load(const char *path);
Config config_parse(Vector *tokens);

ConfigToken *config_create_token(ConfigTokenType type, char *value, int line, int copy_has_ref);
Vector *config_tokenize(const char *path);

void config_action_free(ConfigAction *action);
void config_task_free(ConfigTask *task);
void config_free(Config config);

#endif
