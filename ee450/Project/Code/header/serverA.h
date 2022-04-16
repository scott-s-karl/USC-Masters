// Steven Karl
// Backend Server A Header
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
#include "stdio.h"

void
check_number_of_args(int argc);

void
set_sock_preferences(struct addrinfo *sock_preferences);

void
check_if_getaddrinfo_failed(int getaddrinfo_result);

void
create_sock_and_bind(int *sock_fd, struct addrinfo *poss_cnntns);

void
receive_and_store_from_main(char *buf,
			    socklen_t *addr_len,
			    struct sockaddr_storage *addr,
			    int sock_fd
			    );
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
prep_and_send_udp_data(int sock_fd,
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
able_to_read_lines(char *buf,
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
rewind_server_file(FILE **fin);
