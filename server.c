// Programmed By Derek Irvin
// Server.c
// Final Project CSC60
// May 18th, 2014
// Professor Busovaca

#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#define SHMSZ 27
#include <time.h>
#include <signal.h>


// Kill Everything
void killSwitchEngage()
{
	kill(0, SIGTERM);
	exit(0);
}

int main()
{
	// SHARED MEMORY
	int shmid; // shared memory ID
	key_t key = 0x6060; // Do not touch
	key_t semkey = 0x6061;	// semkey
	char *shm;	//shared memory
	char *s;
	char c;

	// SEMAPHORES
	struct sembuf operations[1];	
	unsigned short one[1] = {1}; // just one argument for the integer value 
	int semid; // Semaphore ID

	union semun
	{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	} argument;

	semid = semget(semkey, 1, 0666 | IPC_CREAT);
	
	if (semid < 0)
	{
		perror("semget"); 
		exit(1);
	}

	argument.array = one;
	if(semctl(semid, 0, SETALL, argument) < 0)
	printf("Cannot init semaphore 0 .\n");

	// CREATE SEGMENT
	if((shmid=shmget(key, SHMSZ, IPC_CREAT| 0666))<0)
	{
		perror("shmget");
		exit(1);
	}

	// ATTACH THE SEGMENT TO OUR DATA SPACE
	if ((shm=shmat(shmid, NULL, 0)) == (char*) -1)
	{
		perror("shmat");
		exit(1);
	}

	// Fill Shared Memory
	s = shm;
	printf("SHM ID: %d\n", shmid);

	// Fill with letters A - Z
	for(c = 'a'; c <= 'z'; c++) *s++ = c;
	//	initialize to nothing	
		*s = '\0';

//	printf("This is a test \n");
//	printf("This is the value of shm: %s \n", shm);

// This was from the Lecture. no Longer Needed
	// Wait untill the other process changes the first char of our memory 
//	for (s = shm; *s != '\0'; s++) putchar(*s);
//	putchar('\n');	

//	shmdt(shm); shmctl(shmid, IPC_RMID, 0); // Detache and Remove Segment



	// SOCKET
	int srvrFd, clntFd, clntAdrLen, i;
	struct sockaddr_in srvrAddr, clntAddr;
	char buf[256];
	time_t t;
	
	
	srvrFd = socket(AF_INET, SOCK_STREAM, 0);
	if (srvrFd < 0)
	{
		perror("socket");
		exit(1);
	}

	srvrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvrAddr.sin_port = htons(6660);
	if (bind(srvrFd, (struct sockaddr *) &srvrAddr, sizeof(srvrAddr)) < 0)
	{
		perror("bind");
		exit(1);
	}

	listen(srvrFd, 5);
	clntAdrLen = sizeof(clntAddr);
	while (1)
	{	
		printf("\n Waiting For Client: \n \n");
		
		// Receive Connection From Client
		clntFd = accept(srvrFd, (struct sockaddr*)&clntAddr, &clntAdrLen);
		
		// Child Process. so a new process is created.
		if (fork() == 0)
		{	
			while(1)
			{
				// Do a P-op On Semaphore 0. To signify shared memory is in use
				// Put the other clients on hold
				operations[0].sem_num = 0;
				operations[0].sem_op = -1;
				operations[0].sem_flg = 0;
				semop(semid, operations, 1);

				// Receive the request from the client server
				i = recv(clntFd, buf, sizeof buf, 0);
			
				
//				printf("new child process created \n");
				srand(time(NULL));
				int randNumber = rand() % 26; // 0 - 25
//				printf ("rand number = %d ", randNumber);
//				printf ("Testing Testing \n");
	
				// With this new number we want to reference the shared memory to see if it is avaliable
				char tkn = *(shm + randNumber);	// pointer to the position	
//				printf("Char Member %c", tkn);
				
				// So if token we have chosen is equal to '_'  we know it has been taken and thus we cannot use it
				if(tkn == '_')
				{
					// So if the value of shared memory is taken, inform the client of this
					send(clntFd, "Value is Taken, Try Again Please", sizeof("Value is Taken, Try again Please") + 1, 0);	
					sleep(1);
//					printf("Values remaining are: %i", counter);
//					printf("\n");
				
				}
				else
				{	
				

					// Replace it with the old symbol
					*(shm + randNumber) = '_';
					
					// Test that the value was changed in shared memory
					for(s = shm; *s != '\0'; s++) putchar(*s);
					putchar('\n');
	
							
					// notify the client what they got
					char serverResponse[2];
					serverResponse[0] = tkn;
					serverResponse[1] = '\0';				
					send(clntFd, serverResponse, sizeof serverResponse, 0);
					
					int k;
					int tokenCount = 0;
					for(k=0; k<26; k++)
					{
						if(isalpha(shm[k]))
							tokenCount++;
					}

					if( tokenCount == 0)
					{
						send(clntFd, "\n You are the Winner!", strlen("\n You are the Winner!") + 1, 0);
						exit(0);
					}			
				}	
						
					// Do a V-op on Semaphore 0
					operations[0].sem_num = 0;
					operations[0].sem_op = 1;
					operations[0].sem_flg = 0;
					semop(semid, operations, 1);



			}	
		}
		else	 close(clntFd);
	}

	atexit(killSwitchEngage);


	return 0;
}
	
