#include "../common/common.h"

int main(void)
{
    char buf[2048] = { '0' };
    int sockfd; 
    char time[60];
    int i = 0;

    sockfd  = connectSocket();
    if(sockfd < 0)
    {
        perror("CLIENT: Connection Failed to server socket");
        exit(0);
    }

#ifdef AUTO_TEST_MODE
    while (1)
    {
        strcpy(buf, "Get sensor");
	usleep(200);
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

            debug("\nClient Sent: %s (Len=%ld bytes)\n", buf, sent);
                   
            while(1)
            {
                memset (buf, 0, sizeof(buf));
                len = recv(sockfd, buf, MAX_BUFFER, 0);
                if(len < 0)
                {
                    perror("Client Rx failed  ...continuing");
                    continue;
                }
                
                if (!strcmp (buf, "DONE") || (!strcmp (buf, "FILE NOT AVAILABLE")))
                break;
        
		dumpLocalTime(time);
                fprintf(stdout,"Sensor,%d,Time,%s,%s\n", (i++)%MAX_SENSORS, time, buf);

                strcpy(buf,"OK");
                debug("Client : sending (%s), len(%ld)\n", buf, len);
                sendData (sockfd, buf, strlen(buf));
            }
       
            //End Monitoring and begin verification
            verifyPerformance();
        }
#ifdef AUTO_TEST_MODE
    }
#endif
    return 0;
}

