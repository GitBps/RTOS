#include "../common/common.h"


int main(void)
{
				struct my_msgbuf buf;
				int msqid, rxqid;
				key_t key;

				if(setupMessageQueues(&msqid, &rxqid) == -1)
				   exit(1);
			
				buf.msender = getpid();

				printf("Client[%ld]: Key =[%u] msqid=[%d]\n", buf.msender, key, msqid);
				printf("Client: Enter lines of text, ^D to quit:\n");


#ifdef AUTO_TEST_MODE
				while (1)
			  {
	      	strcpy(buf.mtext, "abcdefghijklmnopqrstuvwxyz");

#else
					while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL)
#endif
					{
								long len = strlen(buf.mtext);

								/* ditch newline at end, if it exists */
								if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';

#ifndef AUTO_TEST_MODE
								if (len == 1) continue; //It is only a \n no need to transmit.

				        len = sizeof(buf.msender) + len-1;
#else
				        len = sizeof(buf.msender) + len;
#endif
								//Start Perfomance Monitoring per PID
								startMonitoring(&buf);

								//Send message to the msquid
								if(sendMessage (&buf, msqid, len) < 0)
								{
								  	perror("Client: Tx failed ... continuing");
										continue;
								}

								while(1) 								
								{
												startReceive (&buf, rxqid);
												if (strcmp (buf.mtext, "EOF")) break;				
												strcpy(buf.mtext,"OK");
												sendMessage(&buf, msqid, 2 + sizeof(buf.msender));
								}							
							
								//End Monitoring and begin verification
								verifyPerformance(&buf);
						}
#ifdef AUTO_TEST_MODE
				}
#endif
				if (msgctl(msqid, IPC_RMID, NULL) == -1)
				{
								perror("Client: msgctl");
								exit(1);
				}
				return 0;

}

