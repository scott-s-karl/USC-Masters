// Steven Karl
// Backend Server C Header
// ------------------------

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
socket_setup(struct addrinfo *sock_prefs);

void
getaddrinfo_error(int ret_val);

void
bind_to_available_socket(int *sock_fd,
			 struct addrinfo *cxns);

void
receive_and_store_from_main(char *buf,
			    socklen_t *addr_len,
			    struct sockaddr_storage *addr,
			    int sock_fd);
void
get_request_type(char *buf,
		 int *client_request_type);

void
parse_udp_msg(int *client_request_type,
	      char *username,
	      int *balance,
	      char *sender,
	      int *sender_balance,
	      char *receiver,
	      int *receiver_balance,
	      int *transfer_amount,
	      int *max_transaction_index,
	      int *append_transaction,
	      char *buf);

void
bt_request_send_to_main(int sock_fd,
			char *buf,
			int client_request_type,
			char *username,
			int balance,
			int user_found,
			char *sender,
			int sender_balance,
			int sender_found,
			char *receiver,
			int receiver_balance,
			int receiver_found,
			int max_transaction_index,
			struct sockaddr_storage *addr,
			socklen_t addr_len);

void
open_transaction_file(FILE **fin);

char *
read_line(char *buf,
	  FILE **fin,
	  int buf_len);

void
get_user_balance(char *username,
		 int *user_found,
		 int *balance,
		 int *max_transaction_index,
		 FILE **fin);


void
lookup_server_data(FILE **fin,
		   int *balance,
		   int *user_found,
		   int client_request_type,
		   char *username,
		   char *sender,
		   int *sender_balance,
		   int *sender_found,
		   char *receiver,
		   int *receiver_balance,
		   int *receiver_found,
		   int *max_transaction_index);


void
clear_session_variables(char *buf,
			int *client_request_type,
			int *max_transaction_index,
			int *transfer_amount,
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

void
reset_server_file(FILE **fin);

void
append_transaction_to_server_file(FILE **fin,
				  int max_transaction_index,
				  char *sender,
				  char *receiver,
				  int transfer_amount);

void
gather_and_send_transactions(FILE **fin,
			     int sock_fd,
			     struct sockaddr_storage *addr,
			     socklen_t addr_len);

int
send_simple_udp(int sock_fd,
		char *buf,
		struct sockaddr *addr,
		socklen_t addr_len);
