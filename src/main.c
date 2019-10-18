#include <stdio.h>
#include <string.h>
#include "crawl.h"
#include "config.h"
#include "file.h"
#define lengthMax 1000

int main()
{
    int has_error = 0;
    Config config = config_load("config.sconf", &has_error);

    if (has_error)
    {
       return 1;
    }

    FILE *file = crawl("https://qtmsheep.com");


    char ** tab = httpsTabs(file,lengthMax);
    	
    
    for (int i = 0; i < 10; ++i)
    {
    	printf("LES URLS = %s\n", tab[i]);
    }

    
    tab_url_free(tab);
    fclose(file);

    config_free(config);

    return 0;
}
