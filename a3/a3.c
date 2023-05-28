#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#define REQ_PIPE_NAME "REQ_PIPE_65628"
#define RESP_PIPE_NAME "RESP_PIPE_65628"

void readf(int req_fd, char* request)
{
    int i=0;
    char litera;
    while(read(req_fd,&litera,1)>0 && litera!='$')
    {
        request[i] = litera;
        i++;
    }
    request[i] = '\0';
}

void writeInt(int fd, unsigned int n)
{
	if (write(fd, &n, sizeof(n)) == -1)
    {
		printf("you can't write");
		close(fd);
	}
}


void writeChar(int fd, char* cuvant)
{
	if (write(fd, cuvant, strlen(cuvant)) == -1) 
    {
		printf("you can't write");
		close(fd);
	}
	write(fd,"$",sizeof(unsigned char));
}

int main() 
{
    // creează pipe-ul cu nume "RESP PIPE 65628"
    if (mkfifo(RESP_PIPE_NAME, 0600) != 0) 
    {
        printf("ERROR\ncannot create the response pipe\n");
        return 1;
    }
    // deschide pipe-ul de citire "REQ PIPE 65628"
    int req_fd = open(REQ_PIPE_NAME, O_RDONLY);
    if (req_fd == -1) 
    {
        printf("ERROR\ncannot open the request pipe\n");
        return 1;
    }
    // deschide pipe-ul de scriere "RESP PIPE 65628"
    int resp_fd = open(RESP_PIPE_NAME, O_WRONLY);
    if( resp_fd == -1)
    {
        printf("ERROR\ncannot open the response pipe\n");
        return 1;
    }
    if(write(resp_fd,"BEGIN",strlen("BEGIN")) != -1)
        printf("SUCCES\n");
    else 
    {
        printf("ERROR\ncannot write into responde pipe\n");
        return 1;
    }
    char* request;
    while(1)
    {
        readf(req_fd,request);
        if(strcmp(request,"VARIANT") == 0)
        {
            writeChar(resp_fd,"VARIANT");
            writeInt(resp_fd,65628);
            writeChar(resp_fd,"VALUE");
        }
        if(strcmp(request,"CREATE_SHM") == 0)
        {
            int n;
            read(req_fd,&n,sizeof(n));
            int shmFd = shm_open("/ceMJzbj2", O_CREAT | O_RDWR, 0664);
            ftruncate(shmFd, n);
            writeChar(resp_fd,"CREATE_SHM");
            if(shmFd < 0) 
            {
                writeChar(resp_fd,"ERROR");
                return 1;
            }
            else
            {
                writeChar(resp_fd,"SUCCESS");
            }
        }
        if(strcmp(request, "EXIT")==0)
        {
		    unlink(RESP_PIPE_NAME);
		    close(req_fd);
		    close(resp_fd);
		    return 0;
	    }
    }
    // închide pipe-urile
    close(req_fd);
    close(resp_fd);
    unlink(RESP_PIPE_NAME);

    return 0;
}
