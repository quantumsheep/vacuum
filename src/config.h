#ifndef CONFIG_H
#define CONFIG_H

#include "datatypes/map.h"
#include "datatypes/vector.h"

enum config_option_type_t
{
    CONFIG_OPTION_STRING,
    CONFIG_OPTION_ARRAY,
};
typedef enum config_option_type_t ConfigOptionType;

typedef union config_option_value_t ConfigOptionValue;
union config_option_value_t {
    char *str;
    Vector *arr;
};

typedef struct config_option_t ConfigOption;
struct config_option_t
{
    ConfigOptionType type;
    ConfigOptionValue value;
};

typedef struct config_action_t ConfigAction;
struct config_action_t
{
    char *url;

    Map *options; // Map<ConfigOption>
};

typedef struct config_task_t ConfigTask;
struct config_task_t
{
    int hours;
    int minutes;
    int seconds;

    Vector *actions; // Vector<char *>
};

typedef struct config_t Config;
struct config_t
{
    Map *actions; // Map<ConfigAction>
    Map *tasks;   // Map<ConfigTask>
};

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
typedef enum config_token_type_t ConfigTokenType;

typedef struct config_token_item_t ConfigToken;
struct config_token_item_t
{
    ConfigTokenType type;
    char *value;
    int line;
};

ConfigOption *config_option_init(ConfigOptionType type);
ConfigOption *config_option_init_string(const char *s);
ConfigOption *config_option_init_array(Vector *vec);

ConfigAction *config_action_init(const char *url, Map *options);
ConfigTask *config_task_init(int hours, int minutes, int seconds, Vector *actions);

Config config_load(const char *path, int *has_error);
Config config_parse(Vector *tokens, int *has_error);

ConfigToken *config_create_token(ConfigTokenType type, char *value, int line, int copy_has_ref);
Vector *config_tokenize(const char *path, int *has_error);

void config_action_free(ConfigAction *action);
void config_task_free(ConfigTask *task);
void config_free(Config config);

#endif
