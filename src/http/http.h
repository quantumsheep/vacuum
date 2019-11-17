#ifndef HTTP_H
#define HTTP_H

#define CURL_STATICLIB
#include <curl/curl.h>

#include "../datatypes/map.h"

typedef struct http_response_t HttpResponse;
struct http_response_t
{
    int status;
    char *content_type;
    char *buffer;
    size_t buffer_size;
};

HttpResponse http_get(const char *url);

#endif
