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

void
check_number_of_args(int argc);

void
check_if_getaddrinfo_failed(int getaddrinfo_result);

void
set_sock_preferences(struct addrinfo *sock_prefs);

void
create_sock_and_connect(int *sock_fd,
			struct addrinfo *poss_cnntns);

void
send_balance_enquiry(int sock_fd,
		     int msg_len,
		     char *user);

void
recv_balance_enquiry(char *buf,
		     int buf_len,
		     int sock_fd);

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
