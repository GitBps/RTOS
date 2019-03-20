AUTOMATIC 4 PLAYER GAME OF SNAKES AND LADDER
---------------------------------------

| Attempt |   #1   |   #2   |   #3   |  #4    |   #5   |
| :-----: | :----: | :----: | :----: | :----: | :----: |
|  Time   | 7.2048 | 7.2144 | 7.2294 | 7.2388 | 7.2432 |


Requirements
----------------

1.It should be possibleto simulate the start and stopping of a co-routine as shown in the above program.
----------------------------------------------------------------------------------------------------
2.We should be able to Send data to co-routine with something like handle.send() API (You need to implement this)
----------------------------------------------------------------------------------------------------
3.We should be able to pause and resume the execution of co-routines from a central routine.
----------------------------------------------------------------------------------------------------

4.We should be able to modify and existing routine written in C to a Co-routine pattern
----------------------------------------------------------------------------------------------------

5.We should be able to run and manage multiple co-routines in a single program.
----------------------------------------------------------------------------------------------------

6.Using this concept implement a snake and ladder game with four players. Where each player gets a chance to throw the dice get a random number between 0 and 7 and advance their position to next column the game continues till one of them reaches the max  col 100. Have the starting position of snakes and ladders. This again will be simulation of the game with automatic players, each player will be co routine.  Once starts the players will automatically play and game will get over within a specific time. Repeat this game again and again and find out the time taken for each round and tabulate it. Though  snake  and  ladder  is  a very  simple  game  this can  be  the  first  step  towards  modelling  games with timing constraints. 
----------------------------------------------------------------------------------------------------


----------------------------------------------------------------------------------------------------
Run 1: 
----------------------------------------------------------------------------------------------------
**** player 1 is the winner - exit **** 
Time taken to finish automatic game 72048.59296 Units
----------------------------------------------------------------------------------------------------

Run 2: 
----------------------------------------------------------------------------------------------------
**** player 2 is the winner - exit **** 
Time taken to finish automatic game 72144.37327 Units
----------------------------------------------------------------------------------------------------


Run 3:
----------------------------------------------------------------------------------------------------
**** player 2 is the winner - exit **** 
Time taken to finish automatic game 72294.61487
----------------------------------------------------------------------------------------------------

Run 4:
----------------------------------------------------------------------------------------------------
**** player 3 is the winner - exit **** 
Time taken to finish automatic game 72388.04307
----------------------------------------------------------------------------------------------------

Run 5:
----------------------------------------------------------------------------------------------------
**** player 1 is the winner - exit **** 
Time taken to finish automatic game 72432.58777
----------------------------------------------------------------------------------------------------


