#include "http.h"

#include <curl/curl.h>

const char *http_get(const char *url)
{
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "Request failed for '%s': %s\n", url, curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
}
