#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

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
    int VERSION;
    int NO_OF_SECTIONS;
    int HEADER_SIZE;
    int SECT_TYPE;
    int SECT_OFFSET;
    int SECT_SIZE;
    char SECT_NAME[14];
    char MAGIC[4];

    lseek(file,-6,SEEK_END);
    read(file,&HEADER_SIZE,2);
    read(file,MAGIC,4);
    MAGIC[4] = '\0';
    if(strcmp(MAGIC,"Xrhm") != 0)
    {
        printf("ERROR\n");
        printf("wrong magic\n");
        exit(EXIT_FAILURE);
    }
    lseek(file,-HEADER_SIZE,SEEK_CUR);
    read(file,&VERSION,4);
    if(VERSION < 98 || VERSION > 208)
    {
        printf("ERROR\n");
        printf("wrong version\n");
        exit(EXIT_FAILURE);
    }
    read(file,&NO_OF_SECTIONS,1);
    if(NO_OF_SECTIONS < 6 || NO_OF_SECTIONS > 15)
    {
        printf("ERROR\n");
        printf("wrong sect_nr");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < NO_OF_SECTIONS; i++)
    {
        read(file,SECT_NAME,14);
        SECT_NAME[14] = '\0';
        read(file,&SECT_TYPE,1);
        if((SECT_TYPE != 50 && SECT_TYPE != 92) && (SECT_TYPE != 33 && SECT_TYPE != 32))
        {
            printf("ERROR\n");
            printf("wrong sect_types\n");
            exit(EXIT_FAILURE);
        }
        read(file,&SECT_OFFSET,4);
        read(file,&SECT_SIZE,4);
    }
    printf("SUCCESS\n");
    printf("version=%d\n",VERSION);
    printf("nr_sections=%d\n",NO_OF_SECTIONS);
    lseek(file,-HEADER_SIZE,SEEK_END);
    lseek(file,5,SEEK_CUR);
    for(int i = 0; i < NO_OF_SECTIONS; i++)
    {
        read(file,SECT_NAME,14);
        SECT_NAME[14] = '\0';
        read(file,&SECT_TYPE,1);
        read(file,&SECT_OFFSET,4);
        read(file,&SECT_SIZE,4);
        printf("section%d: %s %d %d\n",i+1,SECT_NAME,SECT_TYPE,SECT_SIZE);
    } 
}

void extragere(int file, int section, int line)
{
    int VERSION;
    int NO_OF_SECTIONS;
    int HEADER_SIZE;
    int SECT_TYPE;
    int SECT_OFFSET;
    int SECT_SIZE;
    char SECT_NAME[14];
    char MAGIC[4];

    lseek(file,-6,SEEK_END);
    read(file,&HEADER_SIZE,2);
    read(file,MAGIC,4);
    MAGIC[4] = '\0';
    if(strcmp(MAGIC,"Xrhm") != 0)
    {
        printf("ERROR\n");
        printf("invalid file\n");
        exit(EXIT_FAILURE);
    }
    lseek(file,-HEADER_SIZE,SEEK_CUR);
    read(file,&VERSION,4);
    if(VERSION < 98 || VERSION > 208)
    {
        printf("ERROR\n");
        printf("invalid file\n");
        exit(EXIT_FAILURE);
    }
    read(file,&NO_OF_SECTIONS,1);
    if(NO_OF_SECTIONS < 6 || NO_OF_SECTIONS > 15)
    {
        printf("ERROR\n");
        printf("invalid file");
        exit(EXIT_FAILURE);
    }
    if(NO_OF_SECTIONS < section)
    {
        printf("ERROR\n");
        printf("invalid section");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < NO_OF_SECTIONS; i++)
    {
        read(file,SECT_NAME,14);
        SECT_NAME[14] = '\0';
        read(file,&SECT_TYPE,1);
        if((SECT_TYPE != 50 && SECT_TYPE != 92) && (SECT_TYPE != 33 && SECT_TYPE != 32))
        {
            printf("ERROR\n");
            printf("invalid file\n");
            exit(EXIT_FAILURE);
        }
        read(file,&SECT_OFFSET,4);
        read(file,&SECT_SIZE,4);
    }
    lseek(file,-HEADER_SIZE,SEEK_END);
    lseek(file,5,SEEK_CUR);
    for(int i = 0; i < NO_OF_SECTIONS; i++)
    {
        if(i==section-1)
        {
            read(file,SECT_NAME,14);
            SECT_NAME[14] = '\0';
            read(file,&SECT_TYPE,1);
            read(file,&SECT_OFFSET,4);
            read(file,&SECT_SIZE,4);
            if(SECT_SIZE==0)
            {
                printf("ERROR\n");
                printf("invalid line\n");
                exit(EXIT_FAILURE);
            }
            lseek(file, SECT_OFFSET, SEEK_SET);
            char* b=(char*)malloc(sizeof(char)*SECT_SIZE);
            int bytesRead;
            int lineCount = 0;
            while ((bytesRead = read(file, b, sizeof(b))) > 0) 
            {
                for (int i = 0; i < bytesRead; i++) 
                {
                    if (b[i] == '\n') 
                    {
                        lineCount++;
                        if (lineCount == line) 
                        {
                            b[i] = '\0';    
                            printf("SUCCESS\n");
                            for (int j = i - 1; j >= 0; j--)
                                printf("%c", b[j]);
                            printf("\n");
                            close(file);
                            return;
                        }
                    }
                }
            }
            if (bytesRead == -1) 
            {
                perror("ERROR");
            } 
            else 
            {
                printf("ERROR\ninvalid line\n");
            }
            close(file);
            free(b);
        }
    } 
}

int fisier_parsare(char* path)
{
    int file = open(path,O_RDONLY);
    if(file == -1)
    {
        perror("Could not open input file");
        return -1;
    }
    int VERSION;
    int NO_OF_SECTIONS;
    int HEADER_SIZE;
    int SECT_TYPE;
    int SECT_OFFSET;
    int SECT_SIZE;
    char SECT_NAME[14];
    char MAGIC[4];
    lseek(file,-6,SEEK_END);
    read(file,&HEADER_SIZE,2);
    read(file,MAGIC,4);
    MAGIC[4] = '\0';
    if(strcmp(MAGIC,"Xrhm") != 0)
    {
        return 0;
    }
    lseek(file,-HEADER_SIZE,SEEK_CUR);
    read(file,&VERSION,4);
    if(VERSION < 98 || VERSION > 208)
    {
        return 0;
    }
    read(file,&NO_OF_SECTIONS,1);
    if(NO_OF_SECTIONS < 6 || NO_OF_SECTIONS > 15)
    {
        return 0;
    }
    for(int i = 0; i < NO_OF_SECTIONS; i++)
    {
        read(file,SECT_NAME,14);
        SECT_NAME[14] = '\0';
        read(file,&SECT_TYPE,1);
        if((SECT_TYPE != 50 && SECT_TYPE != 92) && (SECT_TYPE != 33 && SECT_TYPE != 32))
        {
            return 0;
        }
        read(file,&SECT_OFFSET,4);
        read(file,&SECT_SIZE,4);
        if(SECT_SIZE > 1411)
        {
            return 0;
        }
    }
    return 1;
}

int findall(char* path, int s)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    struct stat statbuf;
    dir = opendir(path);
    if(dir == NULL) 
    {
        perror("Could not open directory");
        return 0;
    }
    if (s==0)
    {
        printf("SUCCESS\n");
        s=1;
    }
    while((entry = readdir(dir)) != NULL) 
    {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
        {
            char fullPath[512];
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) 
            {
                if(S_ISREG(statbuf.st_mode) && fisier_parsare(fullPath) == 1)
                    printf("%s\n", fullPath);
                if(S_ISDIR(statbuf.st_mode)) 
                {
                    findall(fullPath,s);
                }
            }
        }
    }
    closedir(dir);
    return 1;
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
                    printf("ERROR\nCould not open input file\n");
                }
            }
        }
        else if(strcmp(argv[1], "extract") == 0)
        {
            int line = 0;
            int section = 0;
            char* path;
            for(int i = 2;i < argc;i++)
            {
                if(strstr(argv[i], "path=") != NULL)
                    path = argv[i] + 5;
                if(strstr(argv[i], "section=") != NULL)
                    section = atoi(argv[i] + 8);
                if(strstr(argv[i], "line=") != NULL)
                    line = atoi(argv[i] + 5);
            }
            int file = open(path,O_RDONLY);
            if(file >= 0)
            {
                extragere(file,section,line);
                close(file);
            }
            else
            {
                printf("ERROR\nCould not open input file\n");
            }
        }
        else if(strcmp(argv[1], "findall") == 0)
        {
            char* path;
            if(strstr(argv[2], "path=") != NULL)
            {
                path = argv[2] + 5;
                if(findall(path,0) == 0)
                    printf("ERROR\ninvalid directory path\n");
            }
        }
    }
    return 0;
}
