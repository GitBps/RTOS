# RTOS
Some opensource exercises for IPC Experimentation.
-------------------------------------------------

Contents - 

a) ClientA: client code for IPC demonstration and Performance evaluation
b) Common.h: includes the common functions and headers which can be used between the clients and c) servers since the functionality is repeating - this is to keep the code clean
d) ServerA: is the main server IPC Echo engine which is working the way described. 

and their individual exectibles too.
ClientA to be run on multiple windows/terminal tabs.


See how to make : 
--------------------
makecommands.txt : mentions the 2 ways to build ->
a) one which is user centric - user can enter the char or strings on the client and get back the upper case

b) FLAG - AUTO_TEST_MODE MACRO to be able to test the performance in a loop via automation.

----------------------
