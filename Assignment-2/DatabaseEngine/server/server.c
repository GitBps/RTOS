#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>

struct my_msgbuf 
{
	long msender;
	char mtext[200];
};

sem_t semap;
FILE *fp = NULL;
struct timespec tstart={0,0}, tend={0,0};

int setupMessageQueues(int *);
int sendMessage (struct my_msgbuf *, int, long );
int startReceive(struct my_msgbuf *, int );
void initSemaphore();
void takeSemaphore();
void releaseSemaphore();
int  processString(char *, char*);
void sendBack(int sockfd);
void startMonitoring();
void verifyPerformance();
pid_t Fork(void);
int startRead(int sockfd, char* buf);

#define MAX_CLIENTS 1024
#define MAX_BUFFER 2048
/*************************/

int main(int argc, char **argv)
{
    int                     listenfd, connfd;
    pid_t                   childpid;
    socklen_t               clilen;
    struct sockaddr_in      cliaddr, servaddr;

    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("Socket creation Failed");
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(8877);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind Failure");
        exit(0);
    }

    if(listen(listenfd, MAX_CLIENTS) < 0)
    {
        perror("Listen Failure");
        exit(0);
    }

    //initSemaphore();
    for ( ; ; )
    {
        //takeSemaphore();
        clilen = sizeof(cliaddr);
  
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        if(connfd < 0)
        {
            perror("Accept Failed");
            continue;
        }

        printf("client connected with ip address: %s\n", inet_ntoa(cliaddr.sin_addr));

        /* Create a child process for concurrent access */
        if ((childpid = Fork()) == 0)
        {
            while(1)
            {
                close(listenfd);     /* close listening socket */
                sendBack(connfd);    /* process the request */
            }
            exit(0);
        }
        close(connfd);               /* parent closes connected socket */
        //releaseSemaphore();
    }
}

pid_t Fork(void)
{
    pid_t   pid;

    if ( (pid = fork()) == -1)
        perror("fork error");
    
    return(pid);
}

int sendData (int sockfd, char* buf, int len)
{
    int ret =  send(sockfd, buf, len, 0);
    if (ret <= 0)
        perror("Write Failed");
    else 
        printf("\nwritten %d\n", ret);
    
    return ret;
}


char filename[128] = {'0'};

void sendBack(int sockfd)
{
    ssize_t     n = 0;
    char        buf[MAX_BUFFER];
	long recvLen = 0;

again:
    startMonitoring();
    n = recv(sockfd, buf, MAX_BUFFER, 0);
    
	if (n > 0) 
    {
        buf[n] = '\0';
        printf("Server Connected and Sending FileSender pid =%d \n", getpid());
        printf("Server Received: %s (Len=%ld bytes) sockfd (%d) \n", buf, n, sockfd);

		if (!processString(buf, filename))
		{
            printf("Server: FileName [%s]\n", filename);
			fp = fopen(filename, "r");
			if( fp == NULL )
			{
			    printf("Server: FileName [%s] NOT AVAILABLE\n", filename);
				strcpy(buf, "FILE NOT AVAILABLE");
               	sendData(sockfd, buf, strlen(buf));

				//releaseSemaphore();
                verifyPerformance();
				goto again;
			}
			
			/* File was found */
			while( !feof(fp) ) 
			{  
			    recvLen = fread (buf, 1, 150, fp);
				/* Read and process data from file… */
				if(recvLen > 0)
				{
				    printf("Server: length read from file[%ld]\n", recvLen);
					//message received, process and send back on the transmit queue
					
               	    if (sendData(sockfd, buf, recvLen) < 0) 
					{
					    perror("Server: Tx failed ...continuing");
						//releaseSemaphore();
						goto again;
					}
					
					startRead (sockfd, buf);
					if (strcmp (buf, "OK")) continue;				
				}
			}

			strcpy(buf,"DONE");
            sendData(sockfd, buf, strlen(buf));

			fclose(fp);
			
		}

        verifyPerformance();
    }

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        perror("sendBack: read error");
}




#if 0


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
#endif



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
 * startRead to starting to receive from the IPC MSG QUEUE
 *
 * ***********************************************************/
int startRead(int sockfd, char* buf)
{
	long n;
	//Reset the last read buffer, to avoid corruption
	memset((void*)buf, 0, sizeof(buf));

	printf("Server: Waiting for incoming message\n");

    n = recv(sockfd, buf, MAX_BUFFER, 0);
    if (n < 0) 
    {
	    perror ("Server:Read handshake failed");
		return -1;
	}
    
	buf[n] = '\0';
    printf("Server read from pid =%d \n", getpid());
    printf("Server Received: %s (Len=%ld bytes) sockfd (%d) \n", buf, n, sockfd);
		
	return n;
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


/**************************************************************
 *
 * startMonitoring Performance (starts timers)
 *
 * ***********************************************************/
void startMonitoring()
{
    clock_gettime(CLOCK_MONOTONIC, &tstart);
}

/**************************************************************
 *
 * End Monitoring delta (take timer and do delta)
 *
 * ***********************************************************/
void verifyPerformance()
{
    clock_gettime(CLOCK_MONOTONIC, &tend);
    
    //printf("\n**********************************************************\n");
    fprintf(stderr, "%d,%.5f\n",
           getpid(),
	   ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
}
