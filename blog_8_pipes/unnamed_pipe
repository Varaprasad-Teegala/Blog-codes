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

#define LEN  1024

pid_t chld_pid;
int fd[2];

void handler()
{
	kill(chld_pid,SIGTERM);
	kill(getpid(),SIGTERM);
}

int main()
{
	signal(SIGINT,handler);
	pipe(fd);
	int p;
	int x = fork();
	if(x == 0)  // child process
	{
		chld_pid = getpid();
		while(1)
                {
                        char s[LEN];
                        //Write 
                        scanf("%[^\n]s",s);
			getchar();
                        write(fd[1],s,strlen(s));
                }

	}
	else if(x>0)  // parent process
	{	
		while(1)
                {
                        char c[BUFSIZ+1];
                        read(fd[0],c,BUFSIZ);
                        printf("client PID-%d : %s\n",getpid(),c);   //read
			memset(c,'\0',BUFSIZ);
                }

	}
	else
	{
		perror("Error in creating fork\n");
	}
}
