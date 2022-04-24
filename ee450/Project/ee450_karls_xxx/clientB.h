// Steven Karl
// Client B Header
// ----------------------

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

/*
  Desc: Checks to ensure that the file wasn't called with any args
  Inputs:
  argc -> number of args passed to this file when called
*/
void
verify_input_count(int argc);

/*
  Desc: Checks if the call to getaddrinfo resulted in error
  Inputs Args:
  1. getaddrinfo_result -> return code from getaddrinfo
  2. s -> Specific server that attempted to call getaddrinfo
  Returns Args: None
*/
void
getaddrinfo_error(int ret_val);

/*
  Desc: Sets up the socket preferences (Empty, family, type)
  This was taken from beej guide (SYSTEM CALLS OR BUST - pg19)
  Inputs Args:
  1. sock_prefs - Struct to store tcp preferences in
  Returns Args: None
*/
void
socket_setup(struct addrinfo *socket_prefs);

/*
  Desc: Attempts to connect to first available socket in cxns
  This was taken from beej guide (Client-Server Setup pg 31-35)
  Inputs Args:
  1. sock_fd -> The specific socket fd that is trying to find
  an available socket
  2. cxns -> the linked list of possible sockets to connect to
  Returns Args: None
*/
void
connect_to_available_socket(int *sock_fd,
			    struct addrinfo *cxns);

/*
  Desc: This sends a balance enquiry to the main server this
  uses the send command because this is tcp.
  This was taken from beej guide (Client Server Setup pg 31-35)
  Inputs Args:
  1. sock_fd -> socket descriptor
  2. user -> The user that is initiating the request
  Returns Args: None
*/
void
send_balance_enquiry(int sock_fd,
		     char *user);

/*
  Desc: This receives the balance reponse from the main server
  and stores it in the buf to be parsed later.
  This was taken from beej guide (Client Server Setup pg 31-35)
  Inputs Args:
  1. buf -> Buffer to fill from the main server
  2. sock_fd -> The socket descriptor
  Returns Args: None
*/
void
recv_balance_enquiry(char *buf,
		     int sock_fd);;

/*
  Desc: This is a send used to send the TXLIST to the main
  server because I don't need to send the other data
  This was taken from beej guide (Client Server Setup pg 31-35)
  Inputs Args:
  1. sock_fd -> The socket descriptor
  2. buf -> Buffer to send to the main server with the request
  Returns Args: None
*/
void
send_tcp(int sock_fd,
	 char *buf);
/*
  Desc: This is a receive used to receive and store the txlist
  results from the main server because. Honestly realizing now
  this is redundant and I didn't need it remove later if time.
  This was taken from beej guide (Client Server Setup pg 31-35)
  Inputs Args:
  1. sock_fd -> The socket descriptor
  2. buf -> Buffer to send to the main server with the request
  Returns Args: None
*/
void
recv_tcp(char *buf,
	 int sock_fd);
/*
  Desc: This is a parsing function for the buffer returned
  from main for the balance and transfer requests.
  This was taken from beej guide (Client Server Setup pg 31-35)
  Inputs Args: (All of the user/sender/receiver related
  variables that are needed for a balance and transfer request)
  Returns Args: None
*/
void
parse_tcp(char *username,
	  int *balance,
	  int *user_found,
	  char *sender,
	  int *sender_balance,
	  int *sender_found,
	  char *receiver,
	  int *receiver_balance,
	  int *receiver_found,
	  char *client_buf,
	  int request_type,
	  int *valid_transaction);

/*
  Desc: When a transfer request is returned from main I
  check the resulting buffer to see if it was valid and
  display it for the client.
  Inputs: (All of the variables involved in a balance or transfer request)
  Returns Args: None
*/
void
check_transfer_return_values(char *sender,
			     int sender_balance,
			     int sender_found,
			     char *receiver,
			     int receiver_balance,
			     int receiver_found,
			     int transfer_amount,
			     int valid_transaction);

/*
  Desc: This is a function to check the request type based
  on the input arguments that were passed in.
  Inputs Args:
  1. argc -> The count of the arguments passed
  2. argv -> The ~dictionary that contains each of the values  
  Returns Args: None
*/
int
get_request_type(int argc,
		 const char* argv[]);
