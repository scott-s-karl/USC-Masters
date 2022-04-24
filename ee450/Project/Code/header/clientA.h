// Steven Karl
// Client A Header

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

void
verify_input_count(int argc);

void
getaddrinfo_error(int ret_val);

void
socket_setup(struct addrinfo *socket_prefs);

void
connect_to_available_socket(int *sock_fd,
			    struct addrinfo *cxns);

void
send_balance_enquiry(int sock_fd,
		     int msg_len,
		     char *user);

void
recv_balance_enquiry(char *buf,
		     int buf_len,
		     int sock_fd);;

void
send_tcp_msg(int sock_fd,
	     int msg_len,
	     char *msg);

void
recv_tcp_msg(char *buf,
	     int buf_len,
	     int sock_fd);

void
parse_tcp_msg(char *username,
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

void
check_transfer_return_values(char *sender,
			     int sender_balance,
			     int sender_found,
			     char *receiver,
			     int receiver_balance,
			     int receiver_found,
			     int transfer_amount,
			     int valid_transaction);

int
get_request_type(int argc,
		 const char* argv[]);


