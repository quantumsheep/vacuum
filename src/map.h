#ifndef MAP_H
#define MAP_H

typedef struct map_node_t MapNode;
struct map_node_t
{
    char *key;
    char *value;

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
char *map_get_value(Map *map, const char *key);

void map_set(Map *map, const char *key, const char *value);
void map_remove(Map *map, const char *key);

#endif
