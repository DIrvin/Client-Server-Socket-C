// Programmed By: Derek irvin
// Final Project Csc60
// Professor Busovaca
// May 18th, 2014

#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>


#define SHMSZ 27

int main(int argc, char *argv[])
{
	////////////// Shared Memory Code /////////////////// 
//	int shmid;
//	key_t key = 0x6060;
//	char *shm, *s;

	/* Get the segment id */
//	if((shmid=shmget(key, SHMSZ, IPC_CREAT | 6660))<0)
//		{perror("shmget"); exit(1);}
	
	/* Attach the segment to our data space. */
//	if((shm=shmat(shmid, NULL, 1))==(char*)-1)
//	{
//		perror("shmat");
//		exit(1);
//	}
	
	/*read what the server put in the memory */
//	printf("Shared memory contains: \n");
//	for(s=shm; *s!='\0'; s++) putchar(*s);
//	putchar('\n');

	// Client Socket
	char buf[80];
	int result, srvrFd, srvrLen;
	struct hostent *host;
	struct sockaddr_in srvrAddr;

	srvrFd = socket(AF_INET, SOCK_STREAM, 0);

	/* Set up TCP server's address */
	srvrLen = sizeof srvrAddr;
	memset(&srvrAddr, 0, srvrLen);
	srvrAddr.sin_port = htons(6660);
	if ((host = gethostbyname("athena.ecs.csus.edu"))==NULL){
	printf("Host Athena Is Not Found \n");
	exit(1);
	}

	srvrAddr.sin_addr = *(struct in_addr*)(host->h_addr);
	srvrAddr.sin_family = AF_INET;
	do{
		result = connect(srvrFd, (struct sockaddr*) &srvrAddr, srvrLen);
		if(result==-1) sleep(1);
	} while (result == -1);
	
	while(1){
	
		send(srvrFd, "Hello", strlen("Hello") +1, 0);
		sleep(1);
		if( recv(srvrFd, buf, sizeof buf, 0)<=0)
		{
			printf("Game Over \n");
			exit(0);
		}
		else
		{
			
			printf("Server: %s \n", buf);
		}
	       	
	}

	// Close Socket Feed with Server
	close(srvrFd);
}

