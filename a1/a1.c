#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct{
    char[20] SECT_NAME;
    int SECT_TYPE;
    int SECT_OFFSET;
    int SECT_SIZE;
}SECTION_HEADER;

typedef struct{
    int VERSION;
    int NO_OF_SECTIONS=NO_OF_SECTIONS*sizeof(SECTION_HEADER);
    SECTION_HEADER* SECTION_HEADERS;
    int HEADER_SIZE;
    char* MAGIC;
}HEADER;

typedef struct{
    HEADER* HEADER;
    int BODY;
}SF;

int parsare(int file)
{
    HEADER h;
    unsigned char buff[24];
    read(file,buff,4);
    h.VERSION = (int) buff;
    return -1;
}

int main(int argc, char **argv)
{
    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0)
        {
            printf("65628\n");
        }
        /*
        else if(strcmp(argv[1], "list") == 0)
        {
            if(strcmp(argv[2], "recursiv") == 0)
            {

            }
        }
        else if(strcmp(argv[1], "parse") == 0)
        {
            if(strstr(argv[2], "path=") != NULL)
            {
                char* path;
                strcpy(path,argv[2]);
                strcpy(path,path+5);
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
        */
    }
    return 0;
}
