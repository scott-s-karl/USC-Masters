Name: Steven Scott Karl
Student ID: 1375-0871-14

What I have Completed:
     I completed all of the required phases. Which includes Checking Wallet, Transfering Coins, List transactions. I did not complete the extra optional phase.


Project Files Legend:
Source Files:
       1. serverM.c - Source code for the main server. Handles interactions with both clients. Talks to the three backend servers to get data from their text files or append to their text files.
       2. serverA.c - Interacts with the main server in that it responds to requests that are made whether balance, transfer or list requests.Has access to block1.txt
       3. serverB.c - Interacts with the main server in that it responds to requests that are made whether balance, transfer or list requests. Has access to block2.txt
       4. serverC.c - Interacts with the main server in that it responds to requests that are made whether balance, transfer or list requests. Has access to block3.txt
       5. clientA.c - Talks to the main server over tcp initiating requests and waiting for a response from the main server.
       6. clientB.c - Talks to the main server over tcp initiating requests and waiting for a response from the main server.

Header Files: 
       1. serverM.h - Header file for the main server file. Contains all function definitions and descriptions. Also contains the definitions of structs used in the main src file. Also contains the enumerate types for the state machine. 
       2. serverA.h - Header file for the backend server A file. Contains all function definitions and there descriptions 
       3. serverB.h - Header file for the backend server B file. Contains all function definitions and there descriptions 
       4. serverC.h - Header file for the backend server C file. Contains all function definitions and there descriptions 
       5. clientA.h - Header file for the client A file. Contains all function definitions and there descriptions 
       6. clientB.h - Header file for the client B file. Contains all function definitions and there descriptions

Server Files:
       1. block1.txt
       2. block2.txt
       3. block3.txt
       4. ~alichain.txt (This file only shows up when you run ./clientA TXLIST or ./clientB TXLIST)

Message Formats:
e. The format of all the messages exchanged.

Porject Idiosyncrasy:
In the following error cases:
   1. Invalid inputs
   2. Can't send
   3. Can't receive
   4. Can't open file

Reused Code:
Yes I reused code from the following sources:
    1. Beej Guide
    2.

Functions that have reused code:
