//A Steven Karl
// Main Server Header
// ---------------------

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

// Structs
typedef struct TxListNode{
  char *data;
  struct TxListNode *next;
}TxListNode;

typedef struct TxList{
  TxListNode *head;
}TxList;

// Enumerated Values for switch statements
typedef enum{
  CTOMAIN,
  CATOMAIN,
  MAINTOCA,
  CBTOMAIN,
  MAINTOCB,
  MAINTOSA,
  MAINTOSB,
  MAINTOSC,
  ENDTRANS
}MAINSTATE;

// Function Definition

/*
  Desc: Checks to ensure that the file wasn't called with any args
  Inputs:
  argc -> number of args passed to this file when called
*/

void
verify_input_count(int argc);

/*
  Desc: Checks if the call to getaddrinfo resulted in error
  This is taken from beej guide client server setup and 
  packaged into a function.
  Inputs Args:
  1. getaddrinfo_result -> return code from getaddrinfo
  Returns Args: None
*/
void
getaddrinfo_error(int return_value);

/*
  Desc: Sets up the tcp sock preferences (Empty, family, type)
  This was taken from beej guide (SYSTEM CALLS OR BUST - pg19)
  Inputs Args:
  1. sock_prefs - Struct to store tcp preferences in
  Returns Args: None
*/
void
tcp_socket_setup(struct addrinfo *socket_prefs);

/*
  Desc: Sets up the udp sock preferences (Empty, family, type)
  This was taken from beej guide (SYSTEM CALLS OR BUST)
  Inputs Args:
  1. sock_prefs - Struct to store udp preferences in
  Returns Args: None
*/
void
udp_socket_setup(struct addrinfo *socket_prefs);

/*
  Desc: Get socket based on possible connections
  This was taken from beej guide (Client-Server Background)
  Inputs Args:
  1. cxn -> struct to store the cxn of the first successful
  2. poss_cxn -> linked list of possible cxns that can be made
  Returns Args: None
*/
void
get_available_socket(struct addrinfo **cxn,
		     struct addrinfo *cxns);

/*
  Desc: Try to create a socket() for each possible connection from getaddrinfo
  This was taken from beej guide (Client-Server Background pg 31-32)
  Inputs Args:
  1. sock_fd -> specific file descriptor(Client/Main/Backend)
  2. cxns -> Linked list of possible connections to attempt to bind
  Returns Args: None
*/
void
tcp_bind_to_available_socket(int *sock_fd,
			     struct addrinfo *cxns);

/*
  Desc: Try to create a socket() for each possible connection from getaddrinfo
  This was taken from beej guide (Client-Server Background pg 36)
  Inputs Args:
  1. sock_fd -> specific file descriptor(Client/Main/Backend)
  2. cxns -> Linked list of possible connections to attempt to bind
  Returns Args: None
*/
void
udp_bind_to_available_socket(int *sock_fd,
			     struct addrinfo *cxns);


/*
  Desc: Try to create a socket() for each possible connection from getaddrinfo
  This was taken from beej guide (Client-Server Background pg 31-32)
  Inputs Args:
  1. sock_fd -> specific file descriptor(Client/Main/Backend)
  2. backlog -> backlog is the number of connections allowed on the incoming queue 
  Returns Args: None
*/
void
tcp_listen_for_cxn(int *sock_fd,
		   int backlog);

/*
  Desc: Responsible for reaping zombie processes with the other helper functions
  This was taken line for line from beej guide (Client-Server Background pg 31-32)
  Inputs Args:
  1. s -> ??
  Returns Args: None
*/
void
sigchld_handler(int s);

/*
  Desc: Responsible for reaping zombie processes with the other helper functions
  This was taken from beej guide and packaged into a function(Client-Server Background pg 31-32)
  Inputs Args:
  1. sa -> ??
  Returns Args: None
*/
void
reap_dead_processes(struct sigaction *sa);

/*
  Desc: get sockaddr, IPv4/IPv6 helper with inet_ntop
  This was taken line for line from beej guide and 
  packaged into a function(Client-Server Background pg 31-32)
  Inputs Args:
  1. sa -> struct holding address information to read
  Returns Args: None
*/
void *
get_in_addr(struct sockaddr *sa);

/*
  Desc: Returns the port based on the addrinfo ipv4 or ipv6
  This was taken line for line from beej guide and 
  packaged into a function (Client-Server Background pg 33).
  Added support for v4 and v6 though.
  Inputs Args:
  1. addr -> addr that made the connection that we want the port for
  Returns Args:
  1. port -> The port that connected to the
*/
int
get_port(struct sockaddr *addr);

/*
  Desc: Recieves from a specific file descriptor
  and saves the data into a buffer.This was taken line for line 
  from beej guide and packaged into a function (Client-Server Background pg 35).
  Inputs Args:
  1. buf -> This is where the received message is being stored
  2. sock_fd -> specfic fd that you are trying to receive from
  Returns Args:
  1. numbytes -> The total number of bytes received and stored in the buffer.
*/
int
receive_client_msg(char **buf,
		   int sock_fd);

/*
  Desc: Gets the request type the the client has made based on spaces
  Inputs Args:
  1. buf -> This is the buffer that contains the clients message
  2. client_request_type -> This stores the request type for future use
  Returns Args: None
*/
void
get_request_type(char *buf,
		 int *client_request_type);

/*
  Desc: Scans the message and separates into useable variables
  Inputs Args:
  1. client_reqeust_type -> Balance/Transfer/TXLIST
  2. username -> username of a balance request
  3. sender -> sender for a transfer request
  4. receiver -> receiver for a transfer request
  5. transfer_amount -> amount for a transfer request
  6. buf -> The buffer the hold the message received
  Returns Args: None
*/
void
parse_client_msg(int *client_request_type,
		 char *username,
		 char *sender,
		 char *receiver,
		 int *transfer_amount,
		 char *buf);

/*
  Desc: Takes in response data and creates a message to send
  Inputs Args: All necessary inputs to send to backend. Should make a struct
  this is ugly af.
  Returns Args:
  1. Bytes sent -> The number of bytes sent
*/
int
bt_request_send_to_backend(int sock_fd,
			   char *buf,
			   struct addrinfo *cxn,
			   char *username,
			   int balance,
			   char *sender,
			   int sender_balance,
			   char *receiver,
			   int receiver_balance,
			   int transfer_amount,
			   int client_request_type,
			   int max_transaction_index,
			   int append_transaction);

/*
  Desc: This packages up a txlist request and then
  sends it to a backend server also passed in
  Inputs Args: 
  1. buf -> Buffer to load the request into for backend
  2. cxn -> Backend server cxn
  3. type -> Type of request
  4. sock_fd -> socket descriptor main in this case
  Returns Args:
  1. Bytes sent -> The number of bytes sent error checked
*/
int
list_request_send_to_backend(char *buf,
			     struct addrinfo *cxn,
			     int type,
			     int sock_fd);

/*
  Desc: This function takes in the list of transactions
  and a new transaction received from a backend server. 
  It then adds it based on criteria defined below
  Inputs Args: 
  1. txlist -> Linked list of transactions
  2. new_tx -> Transaction data string returned from backend
  Returns Args: None
*/
void
add_transaction_to_list(TxList *txlist,
			char *new_tx);
/*
  Desc: This function loops until all txs have been
  received from the the backend server passed in. Sends
  Done when it has got them all.
  Inputs Args: 
  1. txlist -> Linked list of transactions
  2. sock_fd -> socket descriptor main udp fd
  3. cxn -> Backend server
  Returns Args: None
*/
void
collect_backend_transactions(TxList *txlist,
			     int sock_fd,
			     struct addrinfo *cxn);

/*
  Desc: This is a simple receive function that is used
  in the loop in collect backend transactions to collect
  all transaction associated with a server. Code packaged
  and repurposed from beej guide.
  Inputs Args: 
  1. buf -> Buffer with the message from backend
  2. sock_fd -> socket descriptor of main
  3. cxn -> Backend server cxn
  Returns Args: None
*/
int
receive_backend_data(char **buf,
		     int sock_fd,
		     struct addrinfo *back_svr_cxn);

/*
  Desc: This function is used for a balance or transfer
  request. It takes in the buffer received from the backend
  server and then stores the data in useable variables.
  Inputs Args: (All Variables needed for a balance or transfer)
  Returns Args: None
*/
void
parse_and_store_from_backend(char *buf,
			     char *username,
			     int *user_found,
			     int *balance,
			     char *sender,
			     int *sender_balance,
			     int *sender_found,
			     char *receiver,
			     int *receiver_balance,
			     int *receiver_found,
			     int *max_transaction_index,
			     int client_request_type);

/*
  Desc: This function is used for a bt(Balance/transfer)
  request. It takes in the variables associated with these
  requests and then packages them into buf and sends it via
  tcp to the client that initiated the request.
  Inputs Args: (All of the variables for a balance or transfer
  request)
  Returns Args: 
  1. num_bytes -> The number of bytes that were returned from
  send().
*/
int
bt_request_send_to_client(char *buf,
			  char *username,
			  int balance,
			  int user_found,
			  char *sender,
			  int sender_found,
			  int sender_balance,
			  char *receiver,
			  int receiver_found,
			  int receiver_balance,
			  int client_request_type,
			  int sock_fd,
			  int append_transaction);

/*
  Desc: returns random number to select backend server
  Inputs: None
  Returns: 0,1,2 (Represents server A,B,C)
*/
int
get_random_server();

/*
  Desc: Resets all lookup variables for new client request
  inputs: All request related variables (Make into a struct if time permits)
*/
void
clear_session_variables(int *client_request_type,
			int *transfer_amount,
			int *max_transaction_index,
			int *append_transaction,
			char *username,
			int *balance,
			int *user_found,
			char *sender,
			int *sender_balance,
			int *sender_found,
			char *receiver,
			int *receiver_balance,
			int *receiver_found);

/*
  Desc: Checks if a transaction is valid return 0 or 1
  Inputs Args: (Data that makes up a transaction)
  Returns Args: (Valid or not valid)
*/
int
valid_transaction(char *sender,
		  int sender_balance,
		  int sender_found,
		  char *receiver,
		  int receiver_balance,
		  int receiver_found,
		  int transfer_amount);

/*
  Desc: Opens the output file that has been passed in
  specifically in w+ mode. Assigns to input file descriptor
  Inputs Args:
  1. fout -> File pointer to be opened
  Returns Args: None
*/
void
open_output_file(FILE **fout);

/*
  Desc: Closes the file that has been in
  Inputs Args:
  1. fout -> File pointer to be closed
  Returns Args: None
*/
void
close_output_file(FILE **fout);

/*
  Desc: Takes in the linked list of transactions then
  opens a file and writes each item to it.
  Inputs Args:
  1. txlist -> The linked list containing all transactions
  Returns Args: None
*/
void
output_txlist(TxList *txlist);

int
send_txlist_completion_to_client(char *buf,
				 int sock_fd);

/*
  Desc: Based on if a backend server finds a user it
  updates the ?global? found status for that user.
  Inputs Args:
  1. found -> user/sender/receiver ?global? found status
  2. tmp -> current found status returned from backend
  Returns Args: None
*/
void
update_found_status(int *found, int tmp);

