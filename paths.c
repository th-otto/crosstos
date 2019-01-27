#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include "paths.h"

char* cwd = "./";

char* search(char* path)
{
    bool   match = false;
    struct dirent *de;

    char*  search_path = malloc(4000);

    char*  cmp = path;

    if((path[0] == '/') || (path[0] == '\\'))
    {
        strcpy(search_path, "/");
    }
    else
    {
        strcpy(search_path, cwd);
    }

    while(*cmp)
    {
        DIR* dir = opendir (search_path);

        while((*cmp == '/') || (*cmp == '\\'))
        {
            cmp++;
        }

        if(dir)
        {
            while ((de = readdir (dir)))
            {
                char c1, c2;
                int  i = 0;

                match = true;

//                printf("COMPARE %s, %s\n", cmp, de->d_name);
                
                for(;;)
                {
                    c1 = toupper(cmp[i]);
                    c2 = toupper(de->d_name[i]);

                    if(!c2 && ((c1 == '/') || (c1 == '\\')))
                    {
                        break;
                    }

                    if(!c1 && !c2)
                    {
                        break;
                    }

                    if(c1 != c2)
                    {
                        match = false;
                        break;
                    }

                    i++;
 
                }

                if(match)
                {
                    strcat(search_path, "/");
                    strcat(search_path, de->d_name);
                    cmp += i;
                    break;
                }

                /*  compare file name in directory entry with your name  */
            }

            closedir(dir);

            if(!match)
            {
                break;
            }
        }

        if(!match)
        {
            break;
        }
    }

    if(!match)
    {
        if(search_path)
        {
            free(search_path);

            search_path = NULL;
        }
    }

    return search_path;
}

char* path_cwd(void)
{
    return cwd;
}

void path_close(char* path)
{
    if(path)
    {
        free(path);
    }
}

char* path_open(char* fname, bool exist)
{
    char* result = NULL;

    if(exist)
    {
        result = search(fname);

    }
    else
    {
        char* search_path = NULL;

        if(strstr("/", fname) || strstr("\\", fname))
        {
            char* s = strdup(fname);

            if(s)
            {
                int r = strlen(fname);

                if(r)
                {
                    r--;

                    while(r >= 0)
                    {
                        if((s[r] == '\\') || (s[r] == '/'))
                        {
                            s[r] = '\0';
                            fname = &s[r+1];
                            break;
                        }

                        r--;
                    }
                }
            }

            search_path = search(s);
            free(s);

            if(search_path)
            {
                result = malloc(4000);
                
                if(result)
                {
                    strcpy(result, search_path);
                    strcat(result, "/");
                    strcat(result, fname);

                    free(search_path);
                }
            }

        }
        else
        {
            result = strdup(fname);
        }
    }

    return result;
}
