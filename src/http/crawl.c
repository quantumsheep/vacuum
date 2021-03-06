#include "crawl.h"

#include "../config/config.h"
#include "../parallel/file_writing.h"
#include "../utils/file.h"
#include "../utils/log.h"
#include "http.h"
#include "url.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int is_valid_url_char(char c)
{
    int is_alphanum = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
    int is_special_char = (c == '.') || (c == '%') || (c == '#') || (c == ':') || (c == '/') || (c == ';');
    int is_query_char = (c == '?') || (c == '&') || (c == '=') || (c == '+');

    return c != '\0' && (is_alphanum || is_special_char || is_query_char);
}

static char *find_str(const char *str, const Vector *needles)
{
    while (*str != '\0')
    {
        for (size_t i = 0; i < needles->length; i++)
        {
            char *needle = (char *)vector_get(needles, i);
            if (strncmp(str, needle, strlen(needle)) == 0)
            {
                return (char *)str;
            }
        }

        str++;
    }

    return NULL;
}

static Vector *get_urls(const URL *current, const char *page)
{
    Vector *urls = vector_init();

    Vector *needles = vector_init();
    vector_push_ref(needles, "https://");
    vector_push_ref(needles, "http://");

    vector_push_ref(needles, "\'https://");
    vector_push_ref(needles, "\'http://");
    vector_push_ref(needles, "'/");
    vector_push_ref(needles, "'./");
    vector_push_ref(needles, "'..");

    vector_push_ref(needles, "\"https://");
    vector_push_ref(needles, "\"http://");
    vector_push_ref(needles, "\"/");
    vector_push_ref(needles, "\"./");
    vector_push_ref(needles, "\"..");

    char *start = NULL;
    while ((start = find_str(page, needles)))
    {
        char *end = NULL;

        if (start[0] == '"' || start[0] == '\'')
        {
            if (start[2] != '\0' && !is_valid_url_char(start[2]))
            {
                page++;
                continue;
            }

            end = strchr(start + 1, start[0]);
            start++;
        }
        else
        {
            for (end = start; is_valid_url_char(*end); end++)
            {
                continue;
            }
        }

        size_t len = end - start;

        char *url = NULL;

        int relative = *start == '.';
        if (*start == '/')
        {
            size_t url_len = len +
                             strlen(current->protocol) +
                             strlen(current->host) +
                             (relative ? (strlen(current->path) + 1) : 0);

            url = (char *)calloc(sizeof(char), url_len + 1);
            strcat(url, current->protocol);
            strcat(url, current->host);

            if (relative)
            {
                strcat(url, current->path);

                if (current->path[strlen(current->path) - 1] != '/')
                {
                    strcat(url, "/");
                }
            }
        }
        else
        {
            url = (char *)calloc(sizeof(char), len + 1);
        }

        strncat(url, start, len);

        vector_push_ref(urls, url);

        page = end;
    }

    vector_free(needles, VECTOR_KEEP_REFERENCE);

    return urls;
}

static int url_already_visited(const Vector *visited, const char *url)
{
    for (size_t i = 0; i < visited->length; i++)
    {
        if (strcmp(vector_get_string(visited, i), url) == 0)
        {
            return 1;
        }
    }

    return 0;
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
            char *part = (char *)calloc(sizeof(char), size + 1);
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

static void save_response(const URL *url, const char *buffer, const char *prefix)
{
    char *encoded_query = NULL;

    if (url->query != NULL)
        encoded_query = url_query_encode(url->query);

    char *file_path = (char *)calloc(sizeof(char), strlen(prefix) + 1 + strlen(url->fullpath) + (encoded_query != NULL ? strlen(encoded_query) : 0) + 1);
    strcat(file_path, prefix);
    strcat(file_path, "/");
    strcat(file_path, url->fullpath);

    if (encoded_query != NULL)
        strcat(file_path, encoded_query);

    mkdir_recurse(file_path, 0);

    if (!parallel_file_write(file_path, "w", "%s", buffer))
    {
        printf("Failed to write the file: '%s' (%s).\n", file_path, strerror(errno));
    }

    if (encoded_query != NULL)
        free(encoded_query);

    free(file_path);
}

static void crawl_recursive(const char *url, CrawlConfig config, Vector *visited)
{
    log_print_timed("[Depth %d] Crawled %s\n", config.max_depth, url);

    HttpResponse res = http_get(url);
    vector_push_string(visited, url);

    if (res.buffer == NULL || (config.max_buffer > 0 && res.buffer_size > config.max_buffer))
        return;

    URL *parsed_url = url_parse(url);

    if (config.types == NULL || vector_includes_string_n(config.types, res.content_type, (size_t)(strchr(res.content_type, ';') - res.content_type)))
    {
        save_response(parsed_url, res.buffer, config.storage_directory);
    }

    Vector *urls = get_urls(parsed_url, res.buffer);
    url_free(parsed_url);
    free(res.buffer);

    if (config.max_depth > 0)
    {
        config.max_depth--;

        for (size_t i = 0; i < urls->length; i++)
        {
            char *url = vector_get_string(urls, i);
            if (!url_already_visited(visited, url))
            {
                crawl_recursive(url, config, visited);
            }
        }
    }

    vector_free(urls, VECTOR_KEEP_REFERENCE);
}

Vector *crawl(const char *url, CrawlConfig config)
{
    Vector *visited = vector_init();
    crawl_recursive(url, config, visited);

    return visited;
}
