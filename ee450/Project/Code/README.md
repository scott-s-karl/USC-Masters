Name: Steven Scott Karl
Student ID: 1375-0871-14

What I have Completed:
     I completed all of the required phases. Which includes Checking Wallet, Transfering Coins, List transactions. I did not complete the extra optional phase listing statistics.


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

Message Formats:
The format of all the messages exchanged is the same as is listed on the project guide.
Main Server:
1. “The main server is up and running.”
2. “The main server received input=<USERNAME> from the client using TCP over port <port number>.”
3. “The main server received from <SENDER_USERNAME> to transfer <TRANSFER_AMOUNT> coins to
<RECEIVER_USERNAME> using TCP over port <port number>.”
4. “The main server sent a request to server <i>.”
5. “The main server received transactions from Server <i> using UDP over port <PORT_NUMBER>.”
6. “The main server sent a request to server <i>.”
7. “The main server received the feedback from server <i> using UDP over port <PORT_NUMBER>.”
8. “The main server sent the current balance to client <j>.”
9. “The main server sent the result of the transaction to client <j>.”


Backend Server:
1. “The ServerX is up and running using UDP on port <port number>.”
2. “The ServerX received a request from the Main Server.”
3. “The ServerX finished sending the response to the Main Server.”

Client:
1. “The client X is up and running.”
2. "<USERNAME> sent a balance enquiry request to the main server.”
3. “<SENDER_USERNAME> has requested to transfer <TRANSFER_AMOUNT> coins to <RECEIVER_USERNAME>.”
4. “The current balance of <USERNAME> is : <BALANCE_AMOUNT> alicoins.”
5. “<SENDER_USERNAME> successfully transferred <TRANSFER_AMOUT> alicoins to <RECEIVER_USERNAME>.
6. The current balance of <SENDER_USERNAME> is : <BALANCE_AMOUNT> alicoins.”
7. “<SENDER_USERNAME> was unable to transfer <TRANSFER_AMOUT> alicoins to <RECEIVER_USERNAME> because of insufficient balance.
8. The current balance of <SENDER_USERNAME> is : <BALANCE_AMOUNT> alicoins.”
9. “Unable to proceed with the transaction as <SENDER_USERNAME/RECEIVER_USERNAME> is not part of the network.”
10. “Unable to proceed with the transaction as <SENDER_USERNAME> and <RECEIVER_USERNAME> are not part of the network.”
11. “<USERNAME> sent a sorted list request to the main server.”

Project Idiosyncrasy:
I have tested my code and it will break in a few situations. Bad files (Non existent or improperly formatted). Bad input arguments that don't fit the mold given. If there is an error in an file the printout will be as follows:
Error: Description of the error

Reused Code:
Yes I reused code from the following sources:
    1. Beej Guide

All of the setup for the servers and the clients was taken are repackaged from the client server interaction section of the beej guide on page 30 - 38. Specifically, setting up, binding, connecting, accepting, and listening. Each function description in the header files designates whether or not it was taken from beej.  
