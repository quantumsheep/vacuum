#include <stdio.h>
#include "crawl.h"
#include "config.h"

int main()
{
    int has_error = 0;
    Config config = config_load("config.sconf", &has_error);

    if (has_error)
    {
        return 1;
    }

    crawl("https://qtmsheep.com");

    config_free(config);

    return 0;
}
