#include "crawl.h"

#include "config.h"
#include "file.h"
#include "http.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <curl/curl.h>

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

typedef struct url_t URL;
struct url_t
{
    char *host;
    char *path;
    char *page;
    char *query;

    char *fullpath;
};

static URL *parse_url(const char *url)
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

static void free_url(URL *url)
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

static int mkdir_bypass_exists(const char *path)
{
    if (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == -1)
    {
        int err = errno;

        if (err != EEXIST)
        {
            return err;
        }
    }

    return 0;
}

static int mkdir_recurse(const char *path, int take_last)
{
    const char *start = path;
    size_t size = 0;

    while (*path != '\0')
    {
        if (*path == '/')
        {
            char *part = (char *)calloc(sizeof(char), size);
            strncpy(part, start, size);

            int err = mkdir_bypass_exists(part);
            if (err != 0)
                return err;

            free(part);
        }

        size++;
        path++;
    }

    if (take_last)
    {
        return mkdir_bypass_exists(start);
    }

    return 0;
}

static void save_response(const char *url, const char *buffer)
{
    URL *parts = parse_url(url);

    const char prefix[] = "data/";
    char *file_path = (char *)calloc(sizeof(char), strlen(parts->fullpath) + sizeof(prefix) + 1);
    strcat(file_path, prefix);
    strcat(file_path, parts->fullpath);

    mkdir_recurse(file_path, 0);
    FILE *target = fopen(file_path, "w");
    if (target == NULL)
    {
        printf("Failed to create the file: '%s'.\n", file_path);
        return;
    }

    fputs(buffer, target);
    fclose(target);

    free(file_path);
}

Vector *crawl(const char *url, int max_depth, Vector *visited)
{
    printf("%d - %s\n", max_depth, url);

    char *res = http_get(url);
    save_response(url, res);

    if (visited == NULL)
    {
        visited = vector_init();
    }

    vector_push_string(visited, url);

    if (max_depth > 0)
    {
        max_depth--;

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
