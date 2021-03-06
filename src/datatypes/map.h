#ifndef MAP_H
#define MAP_H

#include <stdlib.h>

typedef struct map_node_t MapNode;
struct map_node_t
{
    char *key;
    void *value;

    MapNode *prev;
    MapNode *next;
};

typedef struct map_t Map;
struct map_t
{
    MapNode *first;
};

Map *map_init();

MapNode *map_get(Map *map, const char *key);
void *map_get_value(Map *map, const char *key);
char *map_get_string(Map *map, const char *key);
int *map_get_int(Map *map, const char *key);

void map_set_ref(Map *map, const char *key, void *ptr);
void map_set(Map *map, const char *key, const void *value, size_t size);
void map_set_string(Map *map, const char *key, const char *value);
void map_set_int(Map *map, const char *key, int value);

enum map_delete_mode_t
{
    MAP_KEEP_REFERENCE,
    MAP_FREE_REFERENCE,
};
typedef enum map_delete_mode_t MapDeleteMode;

void map_remove(Map *map, const char *key, MapDeleteMode mode);

void map_free(Map *map, MapDeleteMode mode);

#endif
