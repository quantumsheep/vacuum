#include "url.h"

#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

URL *url_parse(const char *url)
{
    CURLU *cut = curl_url();
    CURLUcode rc = curl_url_set(cut, CURLUPART_URL, url, 0);

    if (rc)
    {
        return NULL;
    }

    URL *parts = malloc(sizeof(URL));

    curl_url_get(cut, CURLUPART_HOST, &parts->host, 0);
    curl_url_get(cut, CURLUPART_PATH, &parts->path, 0);
    curl_url_get(cut, CURLUPART_QUERY, &parts->query, 0);
    curl_url_cleanup(cut);

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
        curl_free(url->host);

    if (url->path != NULL)
    {
        curl_free(url->path);
    }
    else
    {
        free(url->page);
    }

    if (url->query != NULL)
        curl_free(url->query);

    if (url->fullpath != NULL)
        curl_free(url->fullpath);

    free(url);
}
