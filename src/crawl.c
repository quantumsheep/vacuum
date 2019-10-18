#include "http.h"

#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "file.h"
#include "config.h"


FILE *crawl(const char *url)
{

	FILE *f1 ;
	char * fic = "fic.txt";
    f1 = fopen(fic, "w+");
    if (f1 == NULL)
    {
    	printf("ECHEC !!!!!!\n");
    	return NULL;	
    }
    
    fputs(http_get(url),f1);
    fclose(f1);
    return f1;

}


char **httpsTabs(FILE * file, int lengthMax){

    file = fopen("fic.txt", "r+");
    char **tabChar = malloc(sizeof(char*)*lengthMax);
    for (int i = 0; i < lengthMax; ++i)
    {
        tabChar[i] = malloc(sizeof(char)*lengthMax);
    }
    if (file == NULL)
    {
        printf("ECHEC D'OUVETURE DE FICHIER!!!!!!\n");
        return 0;   
    }
    int p=0;
    while (fgetc(file)!=EOF)
    {
        fgets(tabChar[p],lengthMax,file);
        printf("%s",tabChar [p]);
        p++;
        
    }
    //tabChar[lengthMax][lengthMax] ='\0';
    char **tabRes = malloc(sizeof(char*)*10);
    for (int i = 0; i < 10; ++i)
    {
        tabRes[i] = malloc(sizeof(char)*50);
    }
    char tab[50];
    int cpt = 0;
    int cpt2 =0;
    int c =0;
    for (int i = 0; i < lengthMax; ++i)
    {
        for (int j = 0; j < lengthMax; ++j)
        {
            if ((tabChar[i][j] == 'h' && tabChar[i][j+1] == 't' && tabChar[i][j+2] == 't' && tabChar[i][j+3] == 'p' && tabChar[i][j+4] == ':') 
                || (tabChar[i][j] == 'h' && tabChar[i][j+1] == 't' && tabChar[i][j+2] == 't' && tabChar[i][j+3] == 'p' && tabChar[i][j+4] == 's'))
            {
                c = j;
                cpt2 = 0;
                while(tabChar[i][c] != '"'){
                    tab[cpt2] = tabChar[i][c];
                    c++;
                    cpt2++;
                }
                tab[cpt2] = '\0';
                strcpy(tabRes[cpt], tab);
                cpt++;   
            }

        }

    }
    printf("file_length = %ld\n", file_length(file));
    tab_url_free(tabChar);
    return tabRes;
}
