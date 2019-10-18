#include "crawl.h"

#include "config.h"
#include "file.h"
#include "http.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Vector *get_urls(const char *page)
{
    Vector *urls = vector_init();

    char *start = NULL;
    while ((start = strstr(page, "https://")) || (start = strstr(page, "http://")))
    {
        char *end = strchr(start, '"');

        size_t len = end - start;

        char *url = calloc(sizeof(char), len + 1);
        strncpy(url, start, len);

        vector_push_ref(urls, url);

        page = end;
    }

    return urls;
}

Vector *crawl(const char *url)
{
    char *res = http_get(url);

    FILE *target = fopen("fix.txt", "w");
    if (target == NULL)
    {
        printf("Failed to create the file.\n");
        return NULL;
    }

    fputs(res, target);
    fclose(target);

    Vector *visited = get_urls(res);

    return visited;
}
