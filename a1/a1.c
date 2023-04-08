#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

typedef struct{
    char SECT_NAME[14];
    int SECT_TYPE;
    int SECT_OFFSET;
    int SECT_SIZE;
}SECTION_HEADER;

typedef struct{
    int VERSION;
    int NO_OF_SECTIONS;
    SECTION_HEADER* SECTION_HEADERS;
    int HEADER_SIZE;
    char MAGIC[4];
}HEADER;



void listare(char* path, int size_smaller, char* name_starts_with, int recursiv)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    struct stat statbuf;
    dir = opendir(path);
    if(dir == NULL) 
    {
        printf("ERROR\ninvalid directory path\n");
        return;
    }
    while((entry = readdir(dir)) != NULL) 
    {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
        {
            if((strlen(name_starts_with) != 0 && strstr(entry->d_name, name_starts_with) == entry->d_name) || name_starts_with[0] == '\0')
            {
                char fullPath[512];
                snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
                if(lstat(fullPath, &statbuf) == 0) 
                {
                    if(size_smaller == 0 || (statbuf.st_size < size_smaller && !S_ISDIR(statbuf.st_mode)))
                        printf("%s\n", fullPath);
                    if(S_ISDIR(statbuf.st_mode) && recursiv == 1) 
                    {
                        listare(fullPath,size_smaller,name_starts_with,recursiv);
                    }
               }
            }     
        }
    }
    closedir(dir);
}

void parsare(int file)
{
    HEADER h;
    lseek(file,-4,SEEK_END);
    read(file,h.MAGIC,4);
    h.MAGIC[4] = '\0';
    lseek(file,-6,SEEK_END);
    read(file,&h.HEADER_SIZE,2);
    if(strcmp(h.MAGIC,"Xrhm") != 0)
    {
        printf("ERROR\n");
        printf("wrong magic\n");
        return;
    }
    lseek(file,0,SEEK_SET);
    read(file,&h.VERSION,4);
    if(h.VERSION < 98 || h.VERSION > 208)
    {
        printf("ERROR\n");
        printf("wrong version\n");
        return;
    }
    lseek(file,0,SEEK_CUR);
    read(file,&h.NO_OF_SECTIONS,1);
    if(h.NO_OF_SECTIONS < 6 || h.NO_OF_SECTIONS > 15)
    {
        printf("%d \n",h.NO_OF_SECTIONS);
        printf("ERROR\n");
        printf("wrong sect_nr");
        return;
    }
    h.SECTION_HEADERS = (SECTION_HEADER*) malloc(h.NO_OF_SECTIONS*sizeof(SECTION_HEADER*));
    for(int i = 0; i < h.NO_OF_SECTIONS; i++)
    {
        read(file,h.SECTION_HEADERS[i].SECT_NAME,14);
        h.SECTION_HEADERS[i].SECT_NAME[14] = '\0';
        read(file,&h.SECTION_HEADERS[i].SECT_TYPE,1);
        if(h.SECTION_HEADERS[i].SECT_TYPE != 50 || h.SECTION_HEADERS[i].SECT_TYPE != 92 || h.SECTION_HEADERS[i].SECT_TYPE != 33 || h.SECTION_HEADERS[i].SECT_TYPE != 32)
        {
            printf("ERROR\n");
            printf("wrong sect_types\n");
            return;
        }
        read(file,&h.SECTION_HEADERS[i].SECT_OFFSET,4);
        read(file,&h.SECTION_HEADERS[i].SECT_SIZE,4);
    }
    printf("SUCCESS\n");
    printf("version=%d\n",h.VERSION);
    printf("nr_sections=%d\n",h.NO_OF_SECTIONS);
    for(int i = 0; i < h.NO_OF_SECTIONS; i++)
    {
        printf("section%d: %s %d %d\n",i+1,h.SECTION_HEADERS[i].SECT_NAME,h.SECTION_HEADERS[i].SECT_TYPE,h.SECTION_HEADERS[i].SECT_SIZE);
    }
    free(h.SECTION_HEADERS);
}

int main(int argc, char **argv)
{
    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0)
        {
            printf("65628\n");
        }
        else if(strcmp(argv[1], "list") == 0)
        {
            int recursiv = 0;
            int size_smaller = 0;
            char* name_starts_with = "";
            char* path;
            for(int i = 2;i < argc;i++)
            {
                if(strstr(argv[i], "path=") != NULL)
                    path = argv[i] + 5;
                if(strstr(argv[i], "recursive") != NULL)
                    recursiv = 1;
                if(strstr(argv[i], "size_smaller=") != NULL)
                    size_smaller = atoi(argv[i] + 13);
                if(strstr(argv[i], "name_starts_with=") != NULL)
                    name_starts_with = argv[i] + 17;
            }
            if(opendir(path)!=NULL)
            { 
                printf("SUCCESS\n");
            }
            listare(path, size_smaller, name_starts_with, recursiv);
        }
        else if(strcmp(argv[1], "parse") == 0)
        {
            if(strstr(argv[2], "path=") != NULL)
            {
                char* path = argv[2] + 5;
                int file = open(path,O_RDONLY);
                if(file >= 0)
                {
                    parsare(file);
                    close(file);
                }
                else
                {
                    printf("ERROR\n");
                }
            }
        }
    }
    return 0;
}
