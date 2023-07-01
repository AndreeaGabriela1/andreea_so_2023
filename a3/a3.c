#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#define FIFO_WRITE "RESP_PIPE_65628"
#define FIFO_READ "REQ_PIPE_65628"

int main() 
{
    int fd_write = -1, fd_read = -1;
    int shm;
    // Create response pipe
    if (mkfifo(FIFO_WRITE, 0600) != 0) 
    {
        perror("ERROR\ncannot create the response pipe\n");
        return 1;
    }
    // Open request pipe
    fd_read = open(FIFO_READ, O_RDONLY);
    if (fd_read == -1) 
    {
        printf("ERROR\n");
        perror("cannot open the request pipe");
        return 1;
    }
    // Open response pipe
    fd_write = open(FIFO_WRITE, O_WRONLY);
    if (fd_write == -1) 
    {
        printf("ERROR\n");
        perror("Cannot open the response pipe");
        return 1;
    }
    char Message[5] = "BEGIN";
    if (write(fd_write, Message, 5) == -1) 
    {
        printf("ERROR\n");
        perror("cannot write the message");
        close(fd_write);
        return 1;
    }
    write(fd_write,"#",sizeof(unsigned char));
    printf("SUCCES\n");
    char s[250];
    unsigned int n = 65628;
    int i = 0; unsigned char c;
	while(read(fd_read, &c, 1) && c!='#')
	{
        s[i] = c;
        i++;
	}
	s[i] = '\0';
    if(strcmp(s, "VARIANT")==0)
	{
		write(fd_write, "VARIANT", strlen("VARIANT"));
		write(fd_write,"#",sizeof(unsigned char));
		write(fd_write, &n, sizeof(n));
	    write(fd_write, "VALUE", strlen("VALUE"));
		write(fd_write,"#",sizeof(unsigned char));
	}
    n = 0;
    if(strcmp(s, "CREATE_SHM")==0)
    {
        read(fd_read, &n, sizeof(unsigned int));
        shm = shm_open("/ceMJzbj2", O_CREAT | O_RDWR, 0664);
        if(shm < 0)
        {
            write(fd_write, "CREATE_SHM", strlen("CREATE_SHM"));
		    write(fd_write,"#",sizeof(unsigned char));
            write(fd_write, "ERROR", strlen("ERROR"));
		    write(fd_write,"#",sizeof(unsigned char));
        }
        else
        {
            write(fd_write, "CREATE_SHM", strlen("CREATE_SHM"));
		    write(fd_write,"#",sizeof(unsigned char));
            write(fd_write, "SUCCESS", strlen("SUCCESS"));
		    write(fd_write,"#",sizeof(unsigned char));
        }
    }
    if(strcmp(s, "WRITE_TO_SHM")==0)
    {
        int o=0;
        int v=0;
        read(fd_read, &o, sizeof(int));
        read(fd_read, &v, sizeof(int));
        if(o<0 || o>n || (o+sizeof(v)>n))
        {
            write(fd_write, "WRITE_TO_SHM", strlen("WRITE_TO_SHM"));
		    write(fd_write,"#",sizeof(unsigned char));
            write(fd_write, "ERROR", strlen("ERROR"));
		    write(fd_write,"#",sizeof(unsigned char));
        }
        else
        {
            lseek(shm,o,SEEK_SET);
            write(shm,&v,sizeof(v));
            write(fd_write, "WRITE_TO_SHM", strlen("WRITE_TO_SHM"));
		    write(fd_write,"#",sizeof(unsigned char));
            write(fd_write, "SUCCESS", strlen("SUCCESS"));
		    write(fd_write,"#",sizeof(unsigned char));
        }
    }
    if(strcmp(s, "EXIT")==0)
	{
        unlink(FIFO_WRITE);
        close(fd_write);
        return 0;
	}
    // Close pipes
    close(fd_write);
    close(fd_read);
    // Delete response pipe
    unlink(FIFO_WRITE);
    return 0;
}
