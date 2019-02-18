# RTOS

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
a) one which is user centric - user can enter the char or strings on the client and get back the upper case

b) FLAG - AUTO_TEST_MODE MACRO to be able to test the performance in a loop via automation.

-----------oooo-----------
