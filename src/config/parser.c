#include "../config.h"

Config config_parse(Vector *tokens)
{
    Config config;

    config.actions = map_init();
    config.tasks = map_init();

    return config;
}
