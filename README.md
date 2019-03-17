# RTOS

PERFORMANCE PLOTS 
-------------------
PICS PNG1 and 2 are in root from the socket example for 1,2,3,4,5 clients working together.

ECHOSERVER : 
------------
Some opensource exercises for IPC Experimentation.
-------------------------------------------------

CONTENTS
---------
a) ClientA: client code for IPC demonstration and Performance evaluation

b) Common.h: includes the common functions and headers which can be used between the clients and 
servers since the functionality is repeating - this is to keep the code clean

c) ServerA: is the main server IPC Echo engine which is working the way described. 

and their individual exectibles too.
ClientA to be run on multiple windows/terminal tabs.

I plan to club the common into a c library that folks could use for all wrappers for IPC experiments.


HOW TO BUILD : 
--------------------
makecommands.txt : mentions the 2 ways to build ->

Manual mode =>
-------------------
killall -KILL serverA; gcc -o serverA server/server.c -lrt -lpthread -std=c99; gcc -o clientA client/clientA.c  -lrt -lpthread

Automatic calculation mode =>
---------------------------------
killall -KILL serverA; gcc -o serverA server/server.c -lrt -lpthread -std=c99; gcc -o clientA client/clientA.c -DAUTO_TEST_MODE -lrt -lpthread
                                                        
a) one which is user centric - user can enter the char or strings on the client and get back the upper case

b) FLAG - AUTO_TEST_MODE MACRO to be able to test the performance in a loop via automation.

-----------oooo-----------

DATABASE ENGINE: 
-----------------

CONTENTS
---------
a) ClientA: client code for IPC demonstration and Performance evaluation

b) Common.h: includes the common functions and headers which can be used between the clients and 
servers since the functionality is repeating - this is to keep the code clean

c) ServerA: is the main server IPC Database engine which is working the way described. 
and their individual exectibles too.

ClientA to be run on multiple windows/terminal tabs. Or you could also rename them to clientB , C and so on..

HOW TO BUILD : 
--------------------
makecommands.txt : mentions the 2 ways to build ->

Manual mode =>
-------------------
killall -KILL serverA; gcc -o serverA server/server.c -lrt -lpthread -std=c99; gcc -o clientA client/clientA.c  -lrt -lpthread
                                                       
User centric - user can Enter Get Command like this -> "GET testfile.txt" and in return the client will get the full file back from the serverA. similarly you could also send this command from multiple Command Shells.

a) on one prompt run : sudo ./serverA
b) on other prompt run : sudo ./clientA 

"GET testfile.txt"
---------------------
Client[6404]:Buffer to send [get testfile.txt], len=[24], msqid=[163840] 
Client : Sent [24] bytes
Client: Waiting for incoming message
Client: Sender [6404], RxLen [158], Buffer received =[START OF FILE
1: 123123123123134352981293081309812398192
2: 12387192387
3: 123981293871238712379379
4: 1231293879837
5: 21309123091-239-
6: kfdjejlkfd], msqid=[458765] 

**************** PID -> [6404] ***********************
*****************[0.00051] seconds**********************
---------------------------------------------------------
**************** PID -> [6404] ***********************
**********************************************************

-----------oooo-----------
