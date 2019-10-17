#include "map.h"

#include <string.h>

Map *map_init()
{
    Map *map = (Map *)malloc(sizeof(Map));

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

void *map_get_value(Map *map, const char *key)
{
    MapNode *node = map_get(map, key);

    if (node != NULL)
    {
        return node->value;
    }

    return NULL;
}

char *map_get_string(Map *map, const char *key)
{
    return (char *)map_get_value(map, key);
}

int *map_get_int(Map *map, const char *key)
{
    return (int *)map_get_value(map, key);
}

void map_set_ref(Map *map, const char *key, void *ptr)
{
    MapNode *node = map_get(map, key);

    if (node == NULL)
    {
        node = (MapNode *)malloc(sizeof(MapNode));

        node->key = (char *)calloc(sizeof(char), strlen(key) + 1);
        strcpy(node->key, key);

        node->next = map->first;
        node->prev = NULL;

        if (map->first != NULL)
        {
            map->first->prev = node;
        }

        map->first = node;
    }
    else if (node->value != NULL)
    {
        free(node->value);
    }

    node->value = ptr;
}

void map_set(Map *map, const char *key, const void *value, size_t size)
{
    void *ptr = (void *)calloc(sizeof(void), size);
    memcpy(ptr, value, size);

    return map_set_ref(map, key, ptr);
}

void map_set_string(Map *map, const char *key, const char *value)
{
    map_set(map, key, value, sizeof(char) * (strlen(value) + 1));
}

void map_set_int(Map *map, const char *key, int value)
{
    map_set(map, key, &value, sizeof(int));
}

void map_remove(Map *map, const char *key, MapDeleteMode mode)
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

        if (mode == MAP_FREE_REFERENCE && node->value != NULL)
        {
            free(node->value);
        }

        free(node);
    }
}

void map_free(Map *map, MapDeleteMode mode)
{
    if (map == NULL)
        return;

    MapNode *node = map->first;

    while (node != NULL)
    {
        MapNode *next = node->next;

        if (mode == MAP_FREE_REFERENCE && node->value != NULL)
        {
            free(node->value);
        }

        if (node->key != NULL)
        {
            free(node->key);
        }

        free(node);

        node = next;
    }

    free(map);
}
