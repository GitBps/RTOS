#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <ctype.h>


struct my_msgbuf 
{
				long msender;
				char mtext[200];
};

sem_t semap;
FILE *fp = NULL;

int setupMessageQueues(int *);
int sendMessage (struct my_msgbuf *, int, long );
int startReceive(struct my_msgbuf *, int );
void initSemaphore();
void takeSemaphore();
void releaseSemaphore();
int processString(char *, char*);

int main(void)
{
				struct my_msgbuf buf;
				int msqid;
				long recvLen = 0;
				int cnt = 0;
				char filename[128] = {'0'};
				char dataToRead[200] = {'0'};
				
reinit:
				initSemaphore();

				if(setupMessageQueues(&msqid) == -1)
				   exit(1);

		    int cqid ;
				//Main program blocked untill thread closure
				while(1)
				{
reread:
								takeSemaphore();
								//poll on the msqid for all clients
								if((recvLen = startReceive (&buf, msqid)) < 0)
								{
												perror("Server Rx failed ...continuing");
												releaseSemaphore();
												goto reinit;
								}

								printf("Server: Sender[%ld] Input string [%s]\n",
																buf.msender, buf.mtext);	
								if (buf.mtext[recvLen-1] == '\n') buf.mtext[recvLen-1] = '\0';

								cqid = msgget(buf.msender, 0600);
								if (!processString(buf.mtext, filename))
								{
												printf("Server: FileName [%s]\n", filename);
												fp = fopen(filename, "r");
												if( fp == NULL ) {
													 printf("Server: FileName [%s] NOT AVAILABLE\n", filename);
													 strcpy(buf.mtext, "FILE NOT AVAILABLE");
													 sendMessage (&buf, cqid, recvLen); 
													 releaseSemaphore();
													 goto reread;
												}
												while( !feof(fp) ) 
												{  
																recvLen = fread (buf.mtext, 1, 150, fp);
																/* Read and process data from file… */
																if(recvLen > 0)
																{
																		printf("Server: length read from file[%ld]\n", recvLen);
																		//message received, process and send back on the transmit queue
																		if(sendMessage(&buf, cqid, recvLen + sizeof(buf.msender)))
																		{
																						perror("Server: Tx failed ...continuing");
																						releaseSemaphore();
																						goto reinit;
																		}
																		startReceive (&buf, msqid);
																		if (strcmp (buf.mtext, "OK")) continue;				
																}
												}

												strcpy(buf.mtext,"DONE");
												sendMessage(&buf, msqid, 4 + sizeof(buf.msender));

												if( fclose(fp) == 0 ) {
																return EXIT_SUCCESS;
												}	
								}
								releaseSemaphore();

				};
				
				return 0;
}



int processString(char *str, char* filename)
{
   const char s[2] = " ";
   char *token;
	 int i=0;
   
   /* get the first token */
   token = strtok(str, s);
   
   /* walk through other tokens */
   while( token != NULL ) {
      printf("%s %d\n", token, i );
			if (i == 1) 
			{
				//Filename is expected here.
			  strcpy(filename,token);
				return 0;
			}
			i++;
    
      token = strtok(NULL, s);
   }
   
   return -1;
}

/*************************************************************
 *
 * Setup the IPC MSG QUEUE
 *
 * ***********************************************************/
int setupMessageQueues(int *msqid)
{
				key_t key = ftok("./common/key.c", 'b');

				if (key == 0)
				{
								perror("Server: ftok");
								return -1;
				}

				*msqid = msgget(key, 0644 | IPC_CREAT);

				if (*msqid == -1)
				{
								perror("Server: msgget");
								return -1;
				}

				printf("Server: Key =[%u] msqid=[%d]\n", key, *msqid);
				return 0;
}

/**************************************************************
 *
 * SendMessage buffer to the specfied Msqid.
 *
 * ***********************************************************/
int sendMessage (struct my_msgbuf *buf,int msqid, long len)
{
				printf("Server: SendMessage [%ld]:Buffer to send [%s], len=[%ld], msqid=[%d] \n", 
												buf->msender, buf->mtext, len, msqid);


				if (msgsnd(msqid, buf, len, 0) == -1) 
				{
				  	perror("Server: msgsnd");
						return -1;
				}
				printf("Server : Sent [%ld] bytes\n", len);
				return len;
}

/**************************************************************
 *
 * startReceive to starting to receive from the IPC MSG QUEUE
 *
 * ***********************************************************/
int startReceive(struct my_msgbuf *buf,int msqid)
{
				long recvLen;
				//Reset the last read buffer, to avoid corruption
				memset((void*)buf, 0, sizeof(struct my_msgbuf));

				printf("Server: Waiting for incoming message\n");
				recvLen = msgrcv(msqid, buf, sizeof(struct my_msgbuf), 0, 0);
				if ( recvLen <= 0) 
				{
								perror("Server: msgrcv");
								return -1; 
				}
				buf->mtext[recvLen] = '\0';
				printf("Server: StartReceive Sender [%ld], RxLen [%ld], Buffer received =[%s], msqid=[%d] \n", buf->msender, recvLen, buf->mtext, msqid);
	
				return recvLen;
}

void initSemaphore()
{
				// InitSemaphore
				sem_init(&semap, 0, 1);
				printf("Semaphore requested.\n");
}

void takeSemaphore()
{
				// lock
				sem_wait(&semap);
				printf("Semaphore set.\n");
}

void releaseSemaphore()
{
				// unlock
				sem_post(&semap);
				printf("Semaphore unlocked.\n");
}
