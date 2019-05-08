#include "./common/common.h"

unsigned char mtext[MAX_BUFFER] = { '0' };
int main(void)
{
    int sockfd; 
    printf("Client: Enter lines of text, ^D to quit:\n");

    struct my_msgbuf buf; 

    static const pa_sample_spec ss =
    {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
	.channels = 2
    };
    
    pa_simple *s = NULL;
    int error;

    if (!(s = pa_simple_new(NULL, "myvoipapp", PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
		fprintf(stdout, ": pa_simple_new() failed: %d\n", error);
	
    }

    sockfd  = connectSocket();
    if(sockfd < 0)
    {
        perror("CLIENT: Connection Failed to server socket");
        exit(0);
    }
    long len ; 
    long sent = 0; 

    while (1)
    {
#ifdef AUTO_TEST_MODE
        strcpy(mtext, "abcdefghijklmnopqrstuvwxyz");
#elif GET_FROM_FILE
        while(fgets(mtext, sizeof(mtext), stdin) != NULL)
#else
	
#endif
        {
#ifdef GET_FROM_FILE
            long len = strlen(mtext);
            long sent = 0;
            /* ditch newline at end, if it exists */
            
            if (mtext[len-1] == '\n') mtext[len-1] = '\0';
            if (len == 1) continue; //It is only a \n no need to transmit.
            len = len-1;
#endif
            //Start Perfomance Monitoring per PID
            //startMonitoring(&buf);


#if defined(AUTO_TEST_MODE) || defined(GET_FROM_FILE)          
            sent = sendData (sockfd, mtext, len);
            if(sent < 0)
            {
                perror("Client: Write Failed");
                continue;
            }

            printf("\nClient Sent: (Len=%ld bytes)\n", sent);
#else

        /* Record some data ... */
         if (pa_simple_read(s, mtext, sizeof(mtext), &error) < 0) {
            fprintf(stdout,": pa_simple_read() failed: %d\n",error);
            
        }
	sent = sendData (sockfd, mtext, sizeof(mtext) );

        if(sent < 0)
        {
            perror("Client: Write Failed");
            continue;
        }
        printf("\nClient Sent: (Len=%ld bytes)\n", sent);
#endif        



#if defined(AUTO_TEST_MODE) || defined(GET_FROM_FILE)          
            len = recv(sockfd, mtext, MAX_BUFFER, 0);
            if(len < 0)
            {
                perror("Client Rx failed  ...continuing");
                continue;
            }

            printf("Read from Stream Len (%ld)\n", len);
#endif 
            //End Monitoring and begin verification
            //verifyPerformance(&buf);
        }
    }

    close (sockfd);
    return 0;
}

int sendData (int sockfd, char* buf, int len)
{
    int ret =  send(sockfd, buf, len, 0);
    if (ret <= 0)
        perror("Write Failed");
    else 
        printf("\nClient fd %d written %d bytes\n", sockfd, ret);

    return ret;

}


