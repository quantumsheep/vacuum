#include "../config.h"

#include <stdio.h>

static Vector *parse_array(Vector *tokens, int *index, int *has_error)
{
    Vector *array = vector_init();
    int i = *index;

    ConfigToken *token = (ConfigToken *)vector_get(tokens, i);

    if (token == NULL || token->type != TOKEN_OPENING_ARRAY)
    {
        printf("Missing '(' for listing actions line %d\n", token->line);
        *has_error = 1;
    }
    else
    {

        do
        {
            *index = ++i;
            token = (ConfigToken *)vector_get(tokens, i);

            if (token == NULL || token->type != TOKEN_STRING)
            {
                printf("Unexpected token '%s' on line %d - expected a string.\n", token->value, token->line);
                *has_error = 1;
                break;
            }

            vector_push_string(array, token->value);

            *index = ++i;
            token = (ConfigToken *)vector_get(tokens, i);

            if (token && token->type != TOKEN_CLOSING_ARRAY && token->type != TOKEN_ARRAY_SEPARATOR)
            {
                printf("Unexpected token '%s' on line %d - expected ',' or ')'.\n", token->value, token->line);
                *has_error = 1;
                break;
            }
        } while (token && token->type != TOKEN_CLOSING_ARRAY);

        *index = ++i;
    }

    return array;
}

static Map *parse_options(Vector *tokens, int *index, int *has_error)
{
    Map *options = map_init();
    int i = *index;

    ConfigToken *token = (ConfigToken *)vector_get(tokens, i);

    while (token && token->type == TOKEN_OPENING_OPTION)
    {
        ConfigToken *key = (ConfigToken *)vector_get(tokens, i + 1);
        ConfigToken *arrow = (ConfigToken *)vector_get(tokens, i + 2);
        ConfigToken *value = (ConfigToken *)vector_get(tokens, i + 3);
        ConfigToken *end = (ConfigToken *)vector_get(tokens, i + 4);

        if (key == NULL || key->type != TOKEN_STRING)
        {
            printf("Missing key in option line %d\n", token->line);
            *has_error = 1;
            break;
        }
        else if (arrow == NULL || arrow->type != TOKEN_OPTION_ARROW)
        {
            printf("Missing arrow in option line %d\n", token->type);
            *has_error = 1;
            break;
        }
        else if (value == NULL || (value->type != TOKEN_STRING && value->type != TOKEN_OPENING_ARRAY))
        {
            printf("Missing value in option line %d\n", token->line);
            *has_error = 1;
            break;
        }
        else if (end == NULL || (end->type != TOKEN_CLOSING_OPTION && value->type != TOKEN_OPENING_ARRAY))
        {
            printf("Missing '}' in option line %d\n", token->line);
            *has_error = 1;
            break;
        }

        if (value->type == TOKEN_STRING)
        {
            map_set_ref(options, key->value, config_option_init_string(value->value));
            i += 5;
        }
        else if (value->type == TOKEN_OPENING_ARRAY)
        {
            i += 3;

            Vector *values = parse_array(tokens, &i, has_error);

            if (*has_error)
            {
                break;
            }

            map_set_ref(options, key->value, config_option_init_array(values));

            i++;
        }

        *index = i;
        token = (ConfigToken *)vector_get(tokens, i);
    }

    return options;
}

static void parse_action(Map *actions, Vector *tokens, int *index, int *has_error)
{
    int i = *index;
    ConfigToken *token = (ConfigToken *)vector_get(tokens, i);

    i++;

    Map *properties = parse_options(tokens, &i, has_error);
    Map *options = NULL;

    if (*has_error)
        return;

    ConfigToken *next = (ConfigToken *)vector_get(tokens, i);

    if (next && next->type == TOKEN_SEPARATOR)
    {
        i++;
        options = parse_options(tokens, &i, has_error);

        if (*has_error)
            return;
    }

    ConfigOption *name = map_get_value(properties, "name");
    ConfigOption *url = map_get_value(properties, "url");

    if (name == NULL)
    {
        printf("Missing 'name' property for action line %d.\n", token->line);
        *has_error = 1;
    }
    else if (url == NULL)
    {
        printf("Missing 'url' property for action line %d.\n", token->line);
        *has_error = 1;
    }
    else
    {
        map_set_ref(actions, name->value.str, config_action_init(url->value.str, options));
    }

    *index = i;
}

static void parse_task(Map *tasks, Vector *tokens, int *index, int *has_error)
{
    int i = *index;
    ConfigToken *token = (ConfigToken *)vector_get(tokens, i);

    i++;

    Map *properties = parse_options(tokens, &i, has_error);
    Vector *actions = NULL;

    if (*has_error)
        return;

    ConfigToken *next = (ConfigToken *)vector_get(tokens, i);

    if (next && next->type == TOKEN_SEPARATOR)
    {
        i++;
        actions = parse_array(tokens, &i, has_error);

        if (*has_error)
            return;
    }

    ConfigOption *name = map_get_value(properties, "name");
    ConfigOption *hours = map_get_value(properties, "hour");
    ConfigOption *minutes = map_get_value(properties, "minute");
    ConfigOption *seconds = map_get_value(properties, "second");

    if (name == NULL)
    {
        printf("Missing 'name' property for task line %d.\n", token->line);
        *has_error = 1;
    }
    else if (hours == NULL && minutes == NULL && seconds == NULL)
    {
        printf("Missing time property for task line %d. 'hour', 'minute' and/or 'second' properties required.\n", token->line);
        *has_error = 1;
    }
    else
    {
        int hours_i = 0;
        int minutes_i = 0;
        int seconds_i = 0;

        if (hours != NULL)
        {
            hours_i = atoi(hours->value.str);
        }

        if (minutes != NULL)
        {
            minutes_i = atoi(minutes->value.str);
        }

        if (seconds != NULL)
        {
            seconds_i = atoi(seconds->value.str);
        }

        map_set_ref(tasks, name->value.str, config_task_init(hours_i, minutes_i, seconds_i, actions));
        map_free(properties, MAP_FREE_REFERENCE);
    }

    *index = i;
}

Config config_parse(Vector *tokens, int *has_error)
{
    Config config;

    config.actions = map_init();
    config.tasks = map_init();

    for (size_t i = 0; i < tokens->length; i++)
    {
        ConfigToken *token = (ConfigToken *)vector_get(tokens, i);

        if (token == NULL)
        {
            continue;
        }

        if (token->type == TOKEN_ACTION)
        {
            parse_action(config.actions, tokens, &i, has_error);
            i--;
        }
        else if (token->type == TOKEN_TASK)
        {
            parse_task(config.tasks, tokens, &i, has_error);
            i--;
        }
        else
        {
            printf("Unexpected token '%s' on line %d - expected '==' or '='.\n", token->value, token->line);
            *has_error = 1;
        }

        if (*has_error)
            break;
    }

    return config;
}
