#include "../common/common.h"

int main(void)
{
	char buf[2048] = { '0' };
    int sockfd; 

    sockfd  = connectSocket();
    if(sockfd < 0)
    {
        perror("CLIENT: Connection Failed to server socket");
        exit(0);
    }

#ifdef AUTO_TEST_MODE
    while (1)
    {
        strcpy(buf, "Get testfile.txt");
#else
    	while(fgets(buf, sizeof(buf), stdin) != NULL)
#endif
    	{
    	    long len = strlen(buf);
	    long sent = 0;

    		/* ditch newline at end, if it exists */
    	    if (buf[len-1] == '\n') buf[len-1] = '\0';

#ifndef AUTO_TEST_MODE
            if (len == 1) continue; //It is only a \n no need to transmit.
    	    len = len-1;
#endif
    					//Start Perfomance Monitoring per PID
    	    startMonitoring();

            sent = sendData (sockfd, buf, len);
            if(sent < 0)
            {
                perror("Client: Write Failed");
                continue;
            }

            printf("\nClient Sent: %s (Len=%ld bytes)\n", buf, sent);
                   
    		while(1) 								
     		{
				memset (buf, 0, sizeof(buf));
                len = recv(sockfd, buf, MAX_BUFFER, 0);
                if(len < 0)
                {
                    perror("Client Rx failed  ...continuing");
                    continue;
                }
                printf("Read Back from Server %s Len (%ld)\n", buf, len);
			
    			if (!strcmp (buf, "DONE") || (!strcmp (buf, "FILE NOT AVAILABLE")))
				    break;				
    			
				strcpy(buf,"OK");
                printf("Client : sending (%s), len(%ld)\n", buf, len);
                sendData (sockfd, buf, strlen(buf));
            }							
    				
            //End Monitoring and begin verification
    	    //verifyPerformance();
    	}
#ifdef AUTO_TEST_MODE
    }
#endif

    return 0;

}

