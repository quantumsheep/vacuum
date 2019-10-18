#include "config.h"
#include "crawl.h"
#include "datatypes/vector.h"
#include "file.h"

#include <stdio.h>

int main()
{
    int has_error = 0;
    Config config = config_load("config.sconf", &has_error);

    if (has_error)
    {
        return 1;
    }

    int max_depth = 3;
    Vector *visited = crawl("https://qtmsheep.com", max_depth, NULL);

    for (int i = 0; i < visited->length; i++)
    {
        printf("LES URLS = %s\n", vector_get_string(visited, i));
    }

    vector_free(visited, VECTOR_FREE_REFERENCE);
    config_free(config);

    return 0;
}
