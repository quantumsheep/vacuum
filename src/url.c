#include "url.h"

#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

URL *url_init()
{
    URL *url = malloc(sizeof(URL));

    *url = (URL){
        .host = NULL,
        .path = NULL,
        .page = NULL,
        .query = NULL,

        .fullpath = NULL,
    };

    return url;
}

static char *substr(const char *str, size_t size)
{
    char *sub = (char *)calloc(sizeof(char), size + 1);
    strncpy(sub, str, size);

    return sub;
}

URL *url_parse(const char *url)
{
    URL *parts = url_init();

    const char http[] = "http://";
    const char https[] = "https://";

    if (strncmp(url, http, sizeof(http) - 1) == 0)
    {
        url += sizeof(http) - 1;
    }
    else if (strncmp(url, https, sizeof(https) - 1) == 0)
    {
        url += sizeof(https) - 1;
    }

    const char *path = strchr(url, '/');

    parts->host = substr(url, path != NULL ? (path - url) : strlen(url));

    if (path != NULL)
    {
        const char *query = strchr(path, '?');
        parts->path = substr(path, parts->query != NULL ? (parts->query - path) : strlen(path));

        if (query != NULL)
        {
            parts->query = (char *)calloc(sizeof(char), strlen(query) + 1);
            strcpy(parts->query, query);
        }
    }

    puts(parts->host);
    puts(parts->path);
    puts(parts->query);

    if (parts->path == NULL || (strcmp(parts->path, "/") == 0))
    {
        const char base[] = "index.html";
        parts->page = (char *)calloc(sizeof(char), sizeof(base) + 1);
        strcpy(parts->page, base);
    }
    else
    {
        char *last = strrchr(parts->path, '/');
        last[0] = '\0';

        parts->page = last + 1;
    }

    size_t size = 0;

    if (parts->host != NULL)
        size += strlen(parts->host);

    if (parts->path != NULL)
        size += strlen(parts->path);

    if (parts->page != NULL)
        size += strlen(parts->page);

    parts->fullpath = (char *)calloc(sizeof(char), size + 2);

    if (parts->host != NULL)
        strcat(parts->fullpath, parts->host);

    if (parts->path != NULL)
        strcat(parts->fullpath, parts->path);

    if (parts->page != NULL)
    {
        strcat(parts->fullpath, "/");
        strcat(parts->fullpath, parts->page);
    }

    return parts;
}

void url_free(URL *url)
{
    if (url->host != NULL)
        free(url->host);

    if (url->path != NULL)
    {
        free(url->path);
    }
    else
    {
        free(url->page);
    }

    if (url->query != NULL)
        free(url->query);

    if (url->fullpath != NULL)
        free(url->fullpath);

    free(url);
}
