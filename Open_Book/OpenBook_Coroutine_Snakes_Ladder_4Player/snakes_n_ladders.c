#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


/************************
 * Global defines
 ***********************/
#define MAX_USERS 4
#define debug(x, ...) 

/************************
 * Global ENUMs
 ***********************/
enum userID 
{
    USER_1,
    USER_2,
    USER_3,
    USER_4,
    USER_MAX
};

/*******************************************************
 * Global DataStructures
 *******************************************************/
sem_t    *semap;
pid_t    playerCoroutineId[MAX_USERS]; 
unsigned int currentUserPosition[MAX_USERS] = { 0 };
static int userExecuting;
static int exitStatus = 0;
struct timespec tstart={0,0}, tend={0,0};

/**************************************************
 * Hard coded the snake positions for appreciating
 * the randomness in the winning Vs Loosing Player
 *************************************************/
unsigned int snakePosition[5][2] = 
    { 
        {85, 52},
        {98, 2},
	{20, 1},
	{96, 76},
	{90, 1}
    };

/**************************************************
 * Global definitions of Functions
 *************************************************/
pid_t Fork(void)
{
    pid_t   pid;

    if ( (pid = fork()) == -1)
    perror("fork error");
    return(pid);
}

/**************************************************************************************
 * Initialize the Wait and Signal for the Overall Program. Make sure that the 
 * semap object is kept in the Shared MEM so that it is accessible across processes
 *************************************************************************************/

void initWaitingStates()
{
    /* place sem object in shared memory */
    semap = mmap(NULL,sizeof(semap)*MAX_USERS , PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (semap == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /* kept the second param as 1 as this is to be shared across processes */
    for (int x=0 ; x < MAX_USERS; x++)
        sem_init(semap+x, 1, 0);
}

/**************************************************************************************
 * YIELD Processor for other things , and for the caller go into the wait state 
 * unless the main process or the sequential player actually signals the corroutine
 *************************************************************************************/

void yieldProcessor(int playerCoroutineID)
{
    // start the WaitQueue Simulation.
    // Sem_wait is a kind of the wait_queue implementation inside and should allow the other 
    // process/player to come in as needed.
    debug("Semaphore %d locking.\n", playerCoroutineID);
    sem_wait(semap+playerCoroutineID);
}


/**************************************************************************************
 * Kickoff the starting of one of the couroutines designated by playerCoroutineID
 * This could be started either from the Main processor or even from the other player 
 * coroutine based on the choice of implementation
 *************************************************************************************/

void startPlayerCoroutine(int playerCoroutineID)
{
    // unlock
    debug("Sem %d Unlocking.\n", playerCoroutineID);
    sem_post(semap+playerCoroutineID);
}


/**************************************************************************************
 * Cleanup all the Blocks and waiting states as we may need to exit after this
 * This needs be called from the exit/cleanup routines
 *************************************************************************************/

void cleanupWaitsBeforeExit()
{
    for (int x=0 ; x < MAX_USERS; x++)
        sem_post(semap+x);
    
}

/**************************************************************
 * startMonitoring Performance (starts timers)
 * ***********************************************************/

void startMonitoring()
{
    clock_gettime(CLOCK_MONOTONIC, &tstart);
}

/**************************************************************
 * End Monitoring delta (take timer and do delta)
 * ***********************************************************/

void verifyPerformance()
{
    clock_gettime(CLOCK_MONOTONIC, &tend);
    fprintf(stdout, "Time taken to finish automatic game %.5f\n",
	   ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
}

/**************************************************************************************
 * Updates each of the player's column position after every execution cycle.
 * To be called on every iteration when user plays its dice 
 * also there are snake's hardcoded at some positons to keep it closer to reality
 *************************************************************************************/

int updatePlayerPosition(int pos, int playerId)
{
    int x ;
    debug("Into Updating Position for player (%d)\n", pos); 
   
    if (!exitStatus)
    { 
        if (currentUserPosition[playerId] + pos == 100)
        {
            printf ("**** player %d is the winner - exit **** \n", playerId);
            exitStatus = 1;
            verifyPerformance();
        }
        else if( currentUserPosition[playerId] + pos > 100)
        {
            printf ("player %d remains at position %d\n", playerId, currentUserPosition[playerId]);
        }
        else
        {
            currentUserPosition[playerId] += pos;
	    for(x=0; x< 5; x++)
	    {
	        if (currentUserPosition[playerId] == snakePosition[x][0])
                {
		    printf("SNAKE HIT PLAYER (%d), old Pos (%d), new Pos (%d)\n", playerId, currentUserPosition[playerId], snakePosition[x][1]);
		    currentUserPosition[playerId] = snakePosition[x][1];
		    break;
	        } 
	    }
            printf ("player %d Incremented position by [%d] to [%d] now\n", 
                        playerId, pos, currentUserPosition[playerId]);
        }
    }
      
    return exitStatus;
}

/**************************************************************************************
 * Dice update for each player when user has thrown dice in his turn.
 * Right now it is kept as random number but since the numnber can only be between
 * 1..6 so the entropy is really bad due to lower sample size.
 *************************************************************************************/

int diceUpdate(int userId, int nextUserId)
{
    int c;

    // YIELD the Processor untill signalled by player coroutines
    yieldProcessor(userId);
    
    debug("JOB to resume userExecuting =%d\n", userExecuting);
    // dump caller PID.
    debug("Coroutine JOB caller= (%d)\n", getpid());
    /* Start processing here along with the states */
    usleep(20000);

    srand(time(0)); 
    c = (rand() % 6) + 1;

    debug("User %d got  %d\n", userId, c);
    c = updatePlayerPosition(c, userId);

    if (c == 1) exit(0);

    userExecuting = nextUserId;
    startPlayerCoroutine(userExecuting);
    return c;
}

/**************************************************************************************
 * Player1 coroutine - triggered from the child process from main processes
 * To be run cyclically along with other players as the turn is fixed
 *************************************************************************************/

int player1Coroutine()
{
    return diceUpdate(USER_1, USER_2);
}

/**************************************************************************************
 * Player2 coroutine - triggered from the child process from main processes
 * To be run cyclically along with other players as the turn is fixed
 *************************************************************************************/

int player2Coroutine()
{
    return diceUpdate(USER_2, USER_3);
}

/**************************************************************************************
 * Player3 coroutine - triggered from the child process from main processes
 * To be run cyclically along with other players as the turn is fixed
 *************************************************************************************/

int player3Coroutine()
{
    return diceUpdate(USER_3, USER_4);
}

/**************************************************************************************
 * Player4 coroutine - triggered from the child process from main processes
 * To be run cyclically along with other players as the turn is fixed
 *************************************************************************************/

int player4Coroutine()
{
    return diceUpdate(USER_4, USER_1);
}

/**************************************************************************************
 * ********************************* MAIN *********************************************
 *************************************************************************************/
int main()
{
    int numUsers = 0;
    int status, c;
    pid_t pid;

    debug("In parent starting the service sub process (PID %d)\n", getpid());

    /* Take overall lock and let all children block */
    initWaitingStates();

    /* Create the MAX_USERS processes for the number of users */
    /* Create a service sub process for concurrent access */
    
    if ((playerCoroutineId[numUsers++] = Fork()) == 0)
    {
        debug("into service sub process1 => (%d)\n", getpid());
        while(!exitStatus)
        {
            player1Coroutine();
        }
    }

    if ((playerCoroutineId[numUsers++] = Fork()) == 0)
    {
        debug("into service sub process2 => (%d)\n", getpid());
        while(!exitStatus)
        {
            player2Coroutine();
        }
    }
    
    if ((playerCoroutineId[numUsers++] = Fork()) == 0)
    {
        debug("into service sub process3 => (%d)\n", getpid());
        while(!exitStatus)
        {
            player3Coroutine();
        }
    }

    if ((playerCoroutineId[numUsers] = Fork()) == 0)
    {
        debug("into service sub process4 => (%d)\n", getpid());
        while(!exitStatus)
        {
            player4Coroutine();
        }
    }

    printf("ENTER CHARACTER TO START GAME\n");
    c = getchar();
    startMonitoring();

    /* We will go in cyclic order to get the players throw dice */
    srand(time(0)); 
    c = (rand() % MAX_USERS) + 1;
    userExecuting = c ; 

    startPlayerCoroutine(userExecuting);

    /* Wait for children to exit. */
    while (numUsers > 0) 
    {
        pid = wait(&status);
        --numUsers;  
        fprintf(stderr, "Child with PID %ld exited with status 0x%x. numusers %d\n", (long)pid, status, numUsers);
    }
    
    //cleanupWaitsBeforeExit();
    
    if (sem_destroy(semap) < 0)
    {
        perror("sem_destroy failed");
        exit(EXIT_FAILURE);
    }
    if (munmap(semap, sizeof(semap)*MAX_USERS) < 0) {
      perror("munmap failed");
      exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
} 
