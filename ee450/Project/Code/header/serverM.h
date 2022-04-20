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


// Structs
typedef struct TxListNode{
  char *data;
  struct TxListNode *next;
}TxListNode;

typedef struct TxList{
  TxListNode *head;
}TxList;

// Enumerated Values
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

void
check_number_of_args(int argc);

void
set_udp_sock_preferences(struct addrinfo *sock_preferences);

void
set_tcp_sock_preferences(struct addrinfo *sock_preferences);

void
get_available_socket(struct addrinfo **cnntn,
		     struct addrinfo *poss_cnntns);

void
check_if_getaddrinfo_failed(int getaddrinfo_result,
			    char *s);

void
create_tcp_sock_and_bind(int *sock_fd,
			 struct addrinfo *poss_cnntns);

void
begin_tcp_listening(int *sock_fd,
		    int max_q_len);

void
sigchld_handler(int s);

void
reap_zombie_processes(struct sigaction *sa);

void *
get_in_addr(struct sockaddr *sa);

int
get_port(struct sockaddr *addr);

int
receive_client_msg(char **buf,
		   int buf_len,
		   int sock_fd);

void
get_request_type(char *buf,
		 int *client_request_type);

void
parse_client_msg(int *client_request_type,
		 char *username,
		 char *sender,
		 char *receiver,
		 int *transfer_amount,
		 char *buf,
		 int port);

int
gather_and_send_udp_msg(int sock_fd,
			char *buf,
			struct addrinfo *sva_cnntn,
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

int
gather_and_send_list_request(char *buf,
			     struct addrinfo *cnntn,
			     int type,
			     int sock_fd);


void
add_transaction_to_list(TxList *txlist,
			char *new_tx);

void
collect_backend_transactions(TxList *txlist,
			     int sock_fd,
			     struct addrinfo *cnntn);

int
prep_and_receive_udp_data(char **buf,
			  int sock_fd,
			  struct addrinfo *back_svr_cnntn);

void
parse_and_store_udp(char *buf,
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

int
gather_and_send_to_client(char *buf,
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

int
get_random_server();

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

int
valid_transaction(char *sender,
		  int sender_balance,
		  int sender_found,
		  char *receiver,
		  int receiver_balance,
		  int receiver_found,
		  int transfer_amount);

void
open_output_file(FILE **fout);

void
close_output_file(FILE **fout);

void
output_txlist(TxList *txlist);

void
scan_buf_and_update(char *buf,
		    int position,
		    int *found_status);
