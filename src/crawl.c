#include "http.h"

#include <stdio.h>

void crawl(const char *url)
{
    puts(http_get(url));
}
