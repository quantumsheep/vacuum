#include "http.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct buffer_t Buffer;
struct buffer_t
{
    char *bytes;
    size_t len;
    size_t capacity;
};

static Buffer *buffer_init()
{
    Buffer *buffer = (Buffer *)malloc(sizeof(Buffer));

    *buffer = (Buffer){
        .bytes = NULL,
        .len = 0,
    };

    return buffer;
}

static size_t write_buffer(void *ptr, size_t size, size_t nmemb, Buffer *buffer)
{
    size_t nbytes = size * nmemb;

    buffer->bytes = realloc(buffer->bytes, sizeof(char) * (buffer->len + nbytes + 1));

    memcpy(buffer->bytes + buffer->len, ptr, nbytes);
    buffer->len += nbytes;

    buffer->bytes[buffer->len] = '\0';

    return nbytes;
}

HttpResponse http_get(const char *url)
{
    CURL *curl = curl_easy_init();

    HttpResponse response = (HttpResponse){
        .content_type = NULL,
        .buffer = NULL,
        .buffer_size = 0,
    };

    Buffer *buffer = buffer_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
    curl_easy_setopt(curl, CURLOPT_PRIVATE, buffer);

    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
    curl_easy_setopt(curl, CURLOPT_FILETIME, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "crawler");
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_UNRESTRICTED_AUTH, 1L);
    curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
    curl_easy_setopt(curl, CURLOPT_EXPECT_100_TIMEOUT_MS, 0L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "Request failed for '%s': %s\n", url, curl_easy_strerror(res));
    }
    else
    {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status);
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &response.content_type);
    }

    curl_easy_cleanup(curl);

    response.buffer = buffer->bytes;
    response.buffer_size = buffer->len;

    free(buffer);

    return response;
}
