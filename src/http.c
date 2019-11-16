#include "http.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

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

static size_t write_buffer(void *ptr, size_t size, size_t nmemb, struct buffer_t *buffer)
{
    size_t nbytes = size * nmemb;

    buffer->bytes = realloc(buffer->bytes, sizeof(char) * (buffer->len + nbytes + 1));

    memcpy(buffer->bytes + buffer->len, ptr, nbytes);
    buffer->len += nbytes;

    return nbytes;
}

HttpResponse http_get(const char *url)
{
    CURL *curl = curl_easy_init();

    HttpResponse response = (HttpResponse){
        .content_type = NULL,
        .buffer = NULL,
    };

    struct buffer_t buffer = init_buffer();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);

    curl_easy_setopt(curl, CURLINFO_RESPONSE_CODE, &response.status);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "Request failed for '%s': %s\n", url, curl_easy_strerror(res));
    }
    else
    {
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &response.content_type);
    }

    curl_easy_cleanup(curl);

    response.buffer = buffer.bytes;
    return response;
}
