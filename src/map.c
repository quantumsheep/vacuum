#include "map.h"

#include <stdlib.h>
#include <string.h>

Map *map_init()
{
    Map *map = malloc(sizeof(Map));

    map->first = NULL;

    return map;
}

MapNode *map_get(Map *map, const char *key)
{
    MapNode *node = map->first;

    while (node != NULL)
    {
        if (strcmp(node->key, key) == 0)
        {
            break;
        }

        node = node->next;
    }

    return node;
}

char *map_get_value(Map *map, const char *key)
{
    MapNode *node = map_get(map, key);

    if (node != NULL)
    {
        return node->value;
    }

    return NULL;
}

void map_set(Map *map, const char *key, const char *value)
{
    MapNode *node = map_get(map, key);

    if (node == NULL)
    {
        node = malloc(sizeof(MapNode));

        node->key = calloc(sizeof(char), strlen(key) + 1);
        strcpy(node->key, key);

        node->next = map->first;
        node->prev = NULL;

        map->first->prev = node;
        map->first = node;
    }
    else if (node->value != NULL)
    {
        free(node->value);
    }

    node->value = calloc(sizeof(char), strlen(value) + 1);
    strcpy(node->value, value);
}

void map_remove(Map *map, const char *key)
{
    MapNode *node = map_get(map, key);

    if (node != NULL)
    {
        if (map->first == node)
        {
            map->first = node->next;
        }
        else if (node->prev != NULL)
        {
            node->prev->next = node->next;
        }

        if (node->next != NULL)
        {
            node->next->prev = node->prev;
        }

        if (node->key != NULL)
        {
            free(node->key);
        }

        if (node->value != NULL)
        {
            free(node->value);
        }

        free(node);
    }
}
