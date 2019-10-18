#include "crawl.h"

#include "config.h"
#include "file.h"
#include "http.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_valid_url_char(char c)
{
    int is_alphanum = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
    int is_special_char = (c == '.') || (c == '%') || (c == '#') || (c == ':') || (c == '/') || (c == ';');
    int is_query_char = (c == '?') || (c == '&') || (c == '=') || (c == '+');

    return c != '\0' && (is_alphanum || is_special_char || is_query_char);
}

static Vector *get_urls(const char *page)
{
    Vector *urls = vector_init();

    char *start = NULL;
    while ((start = strstr(page, "https://")) || (start = strstr(page, "http://")))
    {
        char *end = NULL;

        if (start != page)
        {
            char before = *(start - 1);

            if (before == '"' || before == '\'')
            {
                end = strchr(start, before);
            }
        }

        if (end == NULL)
        {
            for (end = start; is_valid_url_char(*end); end++)
            {
                continue;
            }
        }

        size_t len = end - start;

        char *url = calloc(sizeof(char), len + 1);
        strncpy(url, start, len);

        vector_push_ref(urls, url);

        page = end;
    }

    return urls;
}

static int url_already_visited(const Vector *visited, const char *url)
{
    for (int i = 0; i < visited->length; i++)
    {
        if (strcmp(vector_get_string(visited, i), url) == 0)
        {
            return 1;
        }
    }

    return 0;
}

Vector *crawl(const char *url, int max_depth, Vector *visited)
{
    max_depth--;
    printf("%d - %s\n", max_depth, url);

    char *res = http_get(url);

    // FILE *target = fopen("fix.txt", "w");
    // if (target == NULL)
    // {
    //     printf("Failed to create the file.\n");
    //     return NULL;
    // }

    // fputs(res, target);
    // fclose(target);

    if (visited == NULL)
    {
        visited = vector_init();
    }

    vector_push_string(visited, url);

    if (max_depth > 0)
    {
        Vector *urls = get_urls(res);

        for (int i = 0; i < urls->length; i++)
        {
            char *url = vector_get_string(urls, i);

            if (!url_already_visited(visited, url))
            {
                crawl(url, max_depth, visited);
            }
        }
    }

    return visited;
}
