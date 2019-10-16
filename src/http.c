#include "http.h"

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct buffer_t
{
    char *bytes;
    size_t len;
    size_t capacity;
};

static struct buffer_t init_buffer()
{
    struct buffer_t buffer;

    buffer.bytes = calloc(sizeof(char), 0 + 1);
    buffer.len = 0;

    return buffer;
}

size_t write_buffer(void *ptr, size_t size, size_t nmemb, struct buffer_t *buffer)
{
    size_t nbytes = size * nmemb;

    buffer->bytes = realloc(buffer->bytes, sizeof(char) * (buffer->len + nbytes + 1));

    memcpy(buffer->bytes + buffer->len, ptr, nbytes);
    buffer->len += nbytes;

    return nbytes;
}

char *http_get(const char *url)
{
    CURL *curl = curl_easy_init();
    struct buffer_t buffer = init_buffer();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "Request failed for '%s': %s\n", url, curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);

    return buffer.bytes;
}
