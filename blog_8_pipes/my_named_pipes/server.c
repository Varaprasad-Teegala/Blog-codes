

//   Ramesh 

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h> 
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

pthread_t t1,t2;
sem_t s1,s2;
void * my_read()
{
	char rx[BUFSIZ+1];
        int fd;
        while(1)
        {
		sem_wait(&s1);

                fd = open("myfifo",O_RDONLY);
                read(fd,rx,BUFSIZ);
		printf("From Suresh : %s\n",rx);
		memset(rx,'\0',strlen(rx));
                close(fd);

		sem_post(&s2);
        }
}

void * my_write()
{
	char tx[1000];
	int fd;
        while(1)
        {
		sem_wait(&s2);

		fd = open("myfifo",O_WRONLY);
                fgets(tx,1000,stdin);
                write(fd,tx,strlen(tx));
		memset(tx,'\0',strlen(tx));
		close(fd);

		sem_post(&s1);
        }
}


int main()
{
	char rx[BUFSIZ+1];
        int fd;
	char tx[1000];
	int p = mknod("myfifo",S_IFIFO|0666,0);
	if((p == -1) && (errno !=EEXIST))
	{
		perror("mknode creation failed\n");
		exit(0);
	}
	else
	
		printf("FIFO file created\n");
	
	sem_init(&s1,0,1);
	sem_init(&s2,0,0);

	pthread_create(&t1,NULL,my_read,NULL);
	pthread_create(&t2,NULL,my_write,NULL);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	exit(0);
}
