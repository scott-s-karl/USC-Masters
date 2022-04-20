// Steven KarlAA
// Main Server
// ---------------

// Include Statements
#include "../header/serverM.h"
#include <stdio.h>
#include "time.h"

// Define Macros
#define SVRAPORT "21711" // Port # ServerA runs on
#define SVRBPORT "22711" // Port # ServerB runs on
#define SVRCPORT "23711" // Port # ServerC runs on
#define MAINUDPPORT "24711" // Port # Main Server UDP runs on
#define MAINTCPPORTA "25711" // Port # Main Server TCP runs on
#define MAINTCPPORTB "26711" // Port # Main Server TCP runs on

#define BACKLOG 10 // max len(pending connections queue) for listening
#define BUFLEN 2048

// Local Functions
/*
  Desc -> checks to ensure that the file wasn't called with any args
  argc -> number of args passed to this file when called
*/
void
check_number_of_args(int argc){
  if (argc > 1){
    fprintf(stderr, "The Main Server program doesn't take input!\n");
    exit(1);
  }
}

/*
  Desc -> Checks if the getaddrinfo failed -> return -1
  getaddrinfo_result -> return code from getaddrinfo
  s -> Specific server that attempted to call getaddrinfo
*/
void
check_if_getaddrinfo_failed(int getaddrinfo_result, char *s){
  if(getaddrinfo_result){
    fprintf(stderr,
	    "getaddrinfo value is %s, which correlates to %s\n",
	    s,
	    gai_strerror(getaddrinfo_result));
    exit(1);
  }
}
/*
  Desc - Sets up the udp sock preferences (Empty, family, type)
  sock_prefs - Struct to store udp preferences in
*/
void
set_tcp_sock_preferences(struct addrinfo *sock_preferences){
  memset(sock_preferences, 0, sizeof(*sock_preferences)); // Empty
  sock_preferences->ai_family = AF_UNSPEC; // IPv4 or IPv6
  sock_preferences->ai_socktype = SOCK_STREAM; // TCP type
  sock_preferences->ai_flags = AI_PASSIVE; // Use my ip
}

/*
  Desc - Sets up the udp sock preferences (Empty, family, type)
  sock_prefs - Struct to store udp preferences in
*/
void
set_udp_sock_preferences(struct addrinfo *sock_preferences){
  memset(sock_preferences, 0, sizeof(*sock_preferences)); // Empty
  sock_preferences->ai_family = AF_UNSPEC; // IPv4 or IPv6
  sock_preferences->ai_socktype = SOCK_DGRAM; // UDP type
}
/*
  Desc -> Get socket based on possible connections
  cnntn -> Return connection
  poss_cnntns -> Input connection list to create socket for
*/
void
get_available_socket(struct addrinfo **cnntn, struct addrinfo *poss_cnntns){
  // Loop through all the results and connect to the first one we can
  int socket_status;
  for(*cnntn = poss_cnntns;
      *cnntn != NULL;
      *cnntn = (*cnntn)->ai_next){
    // Attempt to create the socket
    socket_status = socket((*cnntn)->ai_family,
			   (*cnntn)->ai_socktype,
			   0);
    // Check the return code and continue else break out
    if(socket_status == -1){
      perror("Failed to create socket for this connection");
      continue;
    }
    break;
  }
}

/*
 * Desc -> Try to create a socket() for each possi * ble connection from getaddrinfo
 * set the socket level tcp option to resusable
 * Bind to the first one that doesn't fail
 * We listen with this server so we only bind
 * The client will issue the connection
*/

void
create_tcp_sock_and_bind(int *sock_fd,
			 struct addrinfo *poss_cnntns){
  /* Local Variables */
  int yes = 1; // non-zero signals a sock option will change
  int bind_status; // Return code for bind call
  int setsockopt_status; // Return code for setsockopt call
  struct addrinfo *cnntn;

  /* Loop through possible connections and bind to first one we can */
  for (cnntn = poss_cnntns;
       cnntn != NULL;
       cnntn = cnntn->ai_next){
    /* Attempt to create a socket from the cnntn */
    *sock_fd = socket(cnntn->ai_family,
		      cnntn->ai_socktype,
		      cnntn->ai_protocol);
    // Check the return code from the socket call
    if(*sock_fd == -1){
      perror("maintcp: socket creation failed");
      continue;
    }

    // With the socket created set options
    setsockopt_status = setsockopt(*sock_fd,
				   SOL_SOCKET,
				   SO_REUSEADDR,
				   &yes,
				   sizeof(yes));
    // Check return code
    if(setsockopt_status == -1){
      perror("maintcp: setsockopt failed");
      exit(1);
    }

    // Socket is set now attempt to bind
    bind_status = bind(*sock_fd,
		       cnntn->ai_addr,
		       cnntn->ai_addrlen);
    // Check the bind status
    if(bind_status == -1){
      close(*sock_fd);
      perror("maintcp: bind failed");
      continue;
    }
    // Socket and bind complete break for first found
    break;
  }

  // Check if we made it through the poss_cnntns without a valid socket
  if(cnntn == NULL){
    fprintf(stderr, "maintcp: failed to connect\n");
    exit(2);
  }
}


void
create_udp_sock_and_bind(int *sock_fd,
			 struct addrinfo *poss_cnntns){
  // Setup local variables
  int bind_status;
  struct addrinfo *cnntn;

  // Loop through the possible connections and bind to the first available
  for(cnntn = poss_cnntns;
      cnntn != NULL;
      cnntn = cnntn->ai_next){
    // Attempt to create the socket
    *sock_fd = socket(cnntn->ai_family,
		      cnntn->ai_socktype,
		      cnntn->ai_protocol);
    // Check return code
    if(*sock_fd == -1){
      perror("mainudp: socket creation failed");
      continue;
    }
    
    // Attempt to bind
    bind_status = bind(*sock_fd,
		       cnntn->ai_addr,
		       cnntn->ai_addrlen);
    // Check return code
    if(bind_status == -1){
      close(*sock_fd);
      perror("mainudp: bind failed");
      continue;
    }
    break;
  }
  // Check if the cnntn is null after going through all of the linked list
  if(cnntn == NULL){
    fprintf(stderr, "mainudp: fail to connect\n");
    exit(2);
  }
}

void
begin_tcp_listening(int *sock_fd, int max_q_len){
  int listen_status;
  listen_status = listen(*sock_fd, max_q_len);
  if(listen_status == -1){
    perror("main: listen failed");
    exit(1);
  }
}

void
sigchld_handler(int s){
  // waitpid() might overwrite errno so we have to save and restore it
  int saved_errno = errno;
  while(waitpid(-1,NULL,WNOHANG) > 0);
  errno = saved_errno;
}

void
reap_zombie_processes(struct sigaction *sa){
  int sigaction_status;
  sa->sa_handler = sigchld_handler;
  sigemptyset(&sa->sa_mask);
  sa->sa_flags = SA_RESTART;
  sigaction_status = sigaction(SIGCHLD, sa, NULL);
  if(sigaction_status  == -1){
    perror("main: sigaction failed");
    exit(1);
  }
}

void *
get_in_addr(struct sockaddr *sa){
  if(sa->sa_family == AF_INET){
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  else{
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  }
}

int
get_port(struct sockaddr *addr){
  // Check what type of IP address we have IPv6 IPv4 other
  if(addr->sa_family == AF_INET){ // IPv4
    char *ip_addr = (char *)calloc(INET_ADDRSTRLEN, sizeof(*ip_addr));
    inet_ntop(addr->sa_family,
	      get_in_addr((struct sockaddr *)&addr),
	      ip_addr,
	      INET_ADDRSTRLEN);
    printf("Main Server: Got connection from %s\n", ip_addr);
    free(ip_addr);
    return ((struct sockaddr_in *)&addr)->sin_port;
  }
  else if(addr->sa_family == AF_INET6){ // IPv6
    char *ip_addr = (char *)calloc(INET6_ADDRSTRLEN, sizeof(*ip_addr));
    inet_ntop(addr->sa_family,
	      get_in_addr((struct sockaddr *)&addr),
	      ip_addr,
	      INET6_ADDRSTRLEN);
    printf("Main Server: Got connection from %s\n", ip_addr);
    free(ip_addr);
    return ((struct sockaddr_in6 *)&addr)->sin6_port;
  }
  else{ // Invalid
    perror("The socket type is not of type IPv4 or IPv6\n");
    exit(1);
  }
}

int
receive_client_msg(char **buf,
		   int buf_len,
		   int sock_fd){
  // Call recv to get the msg from the client fd
  int num_bytes;
  num_bytes = recv(sock_fd,
		   *buf,
		   buf_len,
		   0);

  // Check for bad return value -1
  if(num_bytes == -1){
    perror("Error: recv call failed for Client data\n");
    close(sock_fd);
    exit(1);
  }

  // Terminate the buf string by adding a null byte
  (*buf)[num_bytes] = '\0';
  return num_bytes;
}

void
get_request_type(char *buf,
		 int *client_request_type){
  // Loop through the msg count spaces
  int spaces = 0;
  for(int i = 0; buf[i] != '\0'; i++){
    if(buf[i] == ' '){
      spaces++;
    }
  }
  if(strncmp(buf, "TXLIST", 256) == 0){
    *client_request_type = 3;
  }
  else if(spaces == 0){
    *client_request_type = 1;
  }
  else if(spaces == 2){
    *client_request_type = 2;
  }
  else{
    printf("Bad Request\n");
    *client_request_type = 0;
  }
}

void
parse_client_msg(int *client_request_type,
		 char *username,
		 char *sender,
		 char *receiver,
		 int *transfer_amount,
		 char *buf,
		 int port){

  // Get the request type
  get_request_type(buf,
		   client_request_type);

  // Parse based on type
  if(*client_request_type == 1){
    sscanf(buf, "%s", username);

  }
  else if(*client_request_type == 2){
    sscanf(buf, "%s %s %d", sender, receiver, transfer_amount);
    strncpy(username, sender, BUFLEN);
  }
  else if(*client_request_type == 3){
    return;
  }
  else{
    printf("Bad Arguments\n");
  }
}

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
			int append_transaction){
  // Pack based on request
  if(client_request_type == 1){
    snprintf(buf, BUFLEN, "%s %d", username, balance);
  }
  else if(client_request_type == 2){
    snprintf(buf,
	     BUFLEN,
	     "%s %d %s %d %d %d %d",
	     sender,
	     sender_balance,
	     receiver,
	     receiver_balance,
	     transfer_amount,
	     max_transaction_index,
	     append_transaction);
  }
  else{
    printf("Error: Invalid request type, gather and send udp\n");
    exit(1);
  }

  // Send msg over udp
  printf("Sending udp message to backend server\n");
  int num_bytes = sendto(sock_fd,
			 buf,
			 BUFLEN,
			 0,
			 sva_cnntn->ai_addr,
			 sva_cnntn->ai_addrlen);
  // Check return
  if(num_bytes == -1){
    perror("Error: Failed to send bytes to Server A\n");
    exit(1);
  }
  return num_bytes;
}

int
gather_and_send_list_request(char *buf,
			     struct addrinfo *cnntn,
			     int type,
			     int sock_fd){
  // Load the buffer
  snprintf(buf, BUFLEN, "%d", type);

  // Send the data
  int num_bytes = sendto(sock_fd,
			 buf,
			 BUFLEN,
			 0,
			 cnntn->ai_addr,
			 cnntn->ai_addrlen);
  // Check return
  if(num_bytes == -1){
    perror("Error: Failed to send bytes to Server A\n");
    exit(1);
  }
  return num_bytes;
}


void
add_transaction_to_list(TxList *txlist,
			char *new_tx){
  // Setup local variables for storing
  int new_tx_index;
  int current_tx_index;
  TxListNode *current = calloc(1, sizeof(TxListNode));
  TxListNode *prev = calloc(1, sizeof(TxListNode));
  TxListNode *new = calloc(1, sizeof(TxListNode));
  current->data = calloc(BUFLEN, sizeof(char));
  prev->data = calloc(BUFLEN, sizeof(char));
  new->data = calloc(BUFLEN, sizeof(char));
  strncpy(new->data, new_tx, BUFLEN);
  new->next = NULL;
  prev = txlist->head;

  // If head is empty just make it the head
  if(txlist->head->data == NULL){
    txlist->head = new;
    return;
  }
  
  // Get the index
  sscanf(new->data, "%d", &new_tx_index);

  // Loop through the current list
  for(current = txlist->head; current != NULL; current = current->next){
    
    // Get the current transaction index
    sscanf(current->data, "%d", &current_tx_index);
   
    // If were at the first node
    if(new_tx_index < current_tx_index){
      // check if its the head
      if(current == txlist->head){
	// Insert
	new->next = current;
	txlist->head = new;
	break;
      } 
      else{
	new->next = prev->next;
	prev->next = new;
	break;
      }
    }
    if(current->next == NULL){
	current->next = new;
	break;
      }
    prev = current;
  }
}

void
collect_backend_transactions(TxList *txlist,
			     int sock_fd,
			     struct addrinfo *cnntn){
  // Define buffer to store transactions
  char *tx = (char *)calloc(BUFLEN, sizeof(*tx));
  
  // Loop until buffer returns done
  while(1){
    prep_and_receive_udp_data(&tx,
			      sock_fd,
			      cnntn);

    printf("Transaction Received: %s\n", tx);
    // Check for end
    if(strncmp(tx, "Done", BUFLEN) == 0){
      printf("Found the ending message breaking out\n");
      break;
    }
    // Add to tx list
    add_transaction_to_list(txlist,
			    tx);
  }
  // Free mem
  free(tx);
  printf("Done collecting transactions\n");
}

int
prep_and_receive_udp_data(char **buf,
			  int sock_fd,
			  struct addrinfo *back_svr_cnntn){
  // Clear the buffer
  memset(*buf, 0, BUFLEN * sizeof(**buf));
  
  // Receive from backend server
  int num_bytes = recvfrom(sock_fd,
			   *buf,
			   BUFLEN,
			   MSG_WAITALL,
			   back_svr_cnntn->ai_addr,
			   &back_svr_cnntn->ai_addrlen);
  
  // Check return from receive
  if(num_bytes == -1){
    perror("Error: Failed to receive data from backend server \n");
    exit(1);
  }
  
  // Add null byte to make it a cstring
  (*buf)[num_bytes] ='\0';
  
  // Return bytes received
  return num_bytes;
}

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
		    int client_request_type){


  // Check request type
  if(client_request_type == 1){
    sscanf(buf,
	   "%s %d %d",
	   username,
	   balance,
	   user_found);
  }
  else if(client_request_type == 2){
    sscanf(buf,
	   "%s %d %d %s %d %d %d",
	   sender,
	   sender_balance,
	   sender_found,
	   receiver,
	   receiver_balance,
	   receiver_found,
	   max_transaction_index);
  }
  else{
    perror("Error: Invalid request type\n");
    exit(1);
  }
}

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
			  int append_transaction){
  // Check the type
  if(client_request_type == 1){
    snprintf(buf,
	     BUFLEN,
	     "%s %d %d",
	     username,
	     balance,
	     user_found);
  }
  else if(client_request_type == 2){
    snprintf(buf,
	     BUFLEN,
	     "%s %d %d %s %d %d %d",
	     sender,
	     sender_balance,
	     sender_found,
	     receiver,
	     receiver_balance,
	     receiver_found,
	     append_transaction);
  }
  else{
    perror("Error: Gather and send to client bad requst type\n");
    exit(1);
  }

  // Send the buf
  int num_bytes = send(sock_fd,
		       buf,
		       BUFLEN,
		       0);
  // Check the return
  if(num_bytes == -1){
    perror("Error: Failed to send data back to client\n");
    close(sock_fd);
    exit(1);
  }
  return num_bytes;
}

int
get_random_server(){
  srand(time(NULL));
  return(rand() % 3);
}

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
			int *receiver_found){
  // memset all pointers
  memset(username, 0, BUFLEN*sizeof(*username));
  memset(sender, 0, BUFLEN*sizeof(*sender));
  memset(receiver, 0, BUFLEN*sizeof(*receiver));
  
  // Reset all balances
  *balance = 1000;
  *sender_balance = 1000;
  *receiver_balance = 1000;
  
  // Reset all status variables
  *user_found = 0;
  *sender_found = 0;
  *receiver_found = 0;
  *append_transaction = 0;

  // Reset miscelaneous leftover
  *client_request_type = 999;
  *max_transaction_index = 0;
  *transfer_amount = 0;
}

int
valid_transaction(char *sender,
		  int sender_balance,
		  int sender_found,
		  char *receiver,
		  int receiver_balance,
		  int receiver_found,
		  int transfer_amount){
  // Check if sender doesn't have funds
  if(sender_balance < transfer_amount){
    printf("%s was unable to transfer %d alicoins to %s because of insufficient balance.\n",
	   sender,
	   transfer_amount,
	   receiver);
    printf("The current balance of %s is: %d alicoins.\n",
	   sender,
	   sender_balance);
    return 0;
  }

  // Check if sender or receiver isn't found
  if(!sender_found && !receiver_found){
    printf("Unable to proceed with the transaction as %s and %s are not part of the network.\n",
	   sender,
	   receiver);
    return 0;
  }
  else if(!sender_found){
    printf("Unable to proceed with the transaction as %s is not part of the network.\n", sender);
    return 0;
  }
  else if(!receiver_found){
    printf("Unable to proceed with the transaction as %s is not part of the network.\n", receiver);
    return 0;
  }
  return 1;
}

void
open_output_file(FILE **fout){
  *fout = fopen("servers/alichain.txt","w+");
  if(*fout == NULL){
    perror("ERROR: Could not open output file for writing.\n");
    exit(1);
  }
}

void
close_output_file(FILE **fout){
  int ret_val = fclose(*fout);
  if(ret_val != 0){
    perror("ERROR: Could not close output file.\n");
    exit(1);
  }
}

void
output_txlist(TxList *txlist){
  // Define local variables
  TxListNode *current;
  FILE *fout;
  
  // Open a file w+
  open_output_file(&fout);
  
  // Loop through the txlist
  for(current = txlist->head;
      current != NULL;
      current = current->next){
    // Print to the file
    fprintf(fout,
	     "%s\n",
	     current->data);
  }
  // Close the file
  close_output_file(&fout);
}

int
gather_and_send_txlist_completion(char *buf,
				  int sock_fd){
  // Gather message
  strncpy(buf, "Done", BUFLEN);
  
  // Send the buf
  int num_bytes = send(sock_fd,
		       buf,
		       BUFLEN,
		       0);
  // Check the return
  if(num_bytes == -1){
    perror("Error: Failed to send data back to client\n");
    close(sock_fd);
    exit(1);
  }
  return num_bytes;
}

void
update_found_status(int *found, int tmp){
  *found+=tmp;  
}

void
accept_client_connection(int *client_a_fd,
			 int main_server_tcp_a_fd,
			 struct sockaddr_storage *client_a_addr,
			 socklen_t *client_a_addr_sz,
			 int *client_b_fd,
			 int main_server_tcp_b_fd,
			 struct sockaddr_storage *client_b_addr,
			 socklen_t *client_b_addr_sz){
  // Try A
  *client_a_fd = accept(main_server_tcp_a_fd,
		       (struct sockaddr *)client_a_addr,
		       client_a_addr_sz);
  // Check fd
  if(*client_a_fd == -1){
    perror("Client to Main Server: Accept Error!\n");
  }
  else{
    return;
  }
 
  // Try B
  *client_b_fd = accept(main_server_tcp_b_fd,
		       (struct sockaddr *)client_b_addr,
		       client_b_addr_sz);
  // Check fd
  if(*client_a_fd == -1){
    perror("Client to Main Server: Accept Error!\n");
  }
  else{
    return;
  }
}

// Main Function
int main(int argc, const char *argv[]){

  // Check the number of arguments - should be none other then name
  check_number_of_args(argc);

  // Local variables
  char *buf = (char *)calloc(BUFLEN, sizeof *buf);
  int num_bytes_sent;
  int num_bytes_recv;
  int main_server_tcp_a_fd; // File descriptor tcp
  int main_server_tcp_b_fd; // File descriptor tcp
  int main_server_udp_fd; // File descriptor udp
  int getaddrinfo_result; // Function return code on error

  struct addrinfo tcp_a_sock_prefs; // tcp connection prefs
  struct addrinfo *tcp_a_poss_cnntns;
  struct addrinfo tcp_b_sock_prefs; // tcp connection prefs
  struct addrinfo *tcp_b_poss_cnntns;
  struct addrinfo udp_sock_prefs; // udp connection prefs
  struct addrinfo *udp_poss_cnntns;
  
  // Client A
  int client_a_fd;
  struct sockaddr_storage client_a_addr;
  socklen_t client_a_addr_sz;
  int client_a_port;
  
  // Client B
  int client_b_fd;
  struct sockaddr_storage client_b_addr;
  socklen_t client_b_addr_sz;
  int client_b_port;
  
  // Backend Server A
  struct addrinfo sva_sock_prefs; // General Socket Preferences
  struct addrinfo *sva_poss_cnntns; // Return Linked List
  struct addrinfo *sva_cnntn; // One of the items in list
  int sva_port; // Used later backend server side interface
  set_udp_sock_preferences(&sva_sock_prefs); // AI_FAMILY/TYPE
  getaddrinfo_result = getaddrinfo(NULL,
				   SVRAPORT,
				   &sva_sock_prefs,
				   &sva_poss_cnntns); // setup structs
  check_if_getaddrinfo_failed(getaddrinfo_result, "sva"); // Check for non zero
  get_available_socket(&sva_cnntn, sva_poss_cnntns); // Socket() call for returned connections

  // Backend Server B
  struct addrinfo svb_sock_prefs;
  struct addrinfo *svb_cnntn;
  struct addrinfo *svb_poss_cnntns;
  int svb_port;
  set_udp_sock_preferences(&svb_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   SVRBPORT,
				   &svb_sock_prefs,
				   &svb_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "svb");
  get_available_socket(&svb_cnntn, svb_poss_cnntns);
  
  // Backend Server C
  struct addrinfo svc_sock_prefs;
  struct addrinfo *svc_poss_cnntns;
  struct addrinfo *svc_cnntn;
  int svc_port;
  set_udp_sock_preferences(&svc_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   SVRCPORT,
				   &svc_sock_prefs,
				   &svc_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "svc");
  get_available_socket(&svc_cnntn, svc_poss_cnntns);

  // Setup Main Server TCP Support Client A
  set_tcp_sock_preferences(&tcp_a_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   MAINTCPPORTA,
				   &tcp_a_sock_prefs,
				   &tcp_a_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "maintcpa");

  // Setup Main Server TCP Support Client B
  set_tcp_sock_preferences(&tcp_b_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   MAINTCPPORTB,
				   &tcp_b_sock_prefs,
				   &tcp_b_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "maintcpb");

  // Setup Main Server UDP Support
  set_udp_sock_preferences(&udp_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   MAINUDPPORT,
				   &udp_sock_prefs,
				   &udp_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "mainudp");

  // TCP A - Create and Bind socket
  create_tcp_sock_and_bind(&main_server_tcp_a_fd,
			   tcp_a_poss_cnntns);
  freeaddrinfo(tcp_a_poss_cnntns);

  // TCP B - Create and Bind socket
  create_tcp_sock_and_bind(&main_server_tcp_b_fd,
			   tcp_b_poss_cnntns);
  freeaddrinfo(tcp_b_poss_cnntns);

  // UDP - Create and Bind socket
  create_udp_sock_and_bind(&main_server_udp_fd,
			   udp_poss_cnntns);
  freeaddrinfo(udp_poss_cnntns);

  // Listen for connections
  begin_tcp_listening(&main_server_tcp_a_fd, BACKLOG);
  begin_tcp_listening(&main_server_tcp_b_fd, BACKLOG);

  // Reap all the dead processes
  struct sigaction sa;
  reap_zombie_processes(&sa);
  
  // Start Messages
  printf("The main server is up and running\n");

  // Set initial case for connection state
  int state = CTOMAIN;

  // Define the vars to store in when parsing requests
  int client_request_type = 999;
  int balance = 1000;
  int transfer_amount = 0;
  int max_transaction_index = 0;
  int append_transaction = 0;
  int current_client = 0;
  
  char *username = (char *)calloc(BUFLEN, sizeof(*username));
  int user_found = 0;
  int user_found_tmp = 0;
  
  char *sender = (char *)calloc(BUFLEN, sizeof(*sender));
  int sender_balance = 1000;
  int sender_found = 0;
  int sender_found_tmp = 0;

  char *receiver = (char *)calloc(BUFLEN, sizeof(*receiver));
  int receiver_balance = 1000;
  int receiver_found = 0;
  int receiver_found_tmp = 0;

  TxList txlist;
  txlist.head = calloc(1, sizeof(TxListNode));
  txlist.head->data = NULL;
  txlist.head->next = NULL;

  // Select
  fd_set rset;
  int maxfdp1 = (client_a_fd > client_b_fd) ? (client_a_fd + 1) : (client_b_fd + 1);
  
  // Connection Loop
  while(1){ // main accept() loop
    
    // Empty out the buffer before each iteration
    memset(buf, 0, BUFLEN * sizeof(*buf));
      
    // Switch on clientA, clientB, SVRA, SVRB, SVRC
    switch(state){    
    case CTOMAIN:
      printf("Case - Client Unknown to Main\n");
      client_a_addr_sz = sizeof client_a_addr;
      client_b_addr_sz = sizeof client_b_addr;

      // Try to accept the client
      /*accept_client_connection(&client_a_fd,
			       main_server_tcp_a_fd,
			       &client_a_addr,
			       &client_a_addr_sz,
			       &client_b_fd,
			       main_server_tcp_b_fd,
			       &client_b_addr,
			       &client_b_addr_sz);
      */
      FD_SET(client_a_fd, &rset);
      FD_SET(client_b_fd, &rset);
      
      current_client = select(maxfdp1, &rset, NULL, NULL, NULL);

      if(FD_ISSET(client_a_fd, &rset)){
	printf("Client A Set\n");
      }
      
      if(FD_ISSET(client_b_fd, &rset)){
	printf("Client B Set\n");
      }

      // Determine which client
      printf("Client A fd: %d\n", client_a_fd);
      printf("Client B fd: %d\n", client_b_fd);
      
      if(client_a_fd != 0){
	if(client_a_fd == -1){
	  perror("Client A to Main Server: Accept Error!\n");
	  continue;
	}
	else{
	  state = CATOMAIN;
	  current_client = 1;
	}
      }
      else if(client_b_fd != 0){
	if(client_b_fd == -1){
	  perror("Client B to Main Server: Accept Error!\n");
	  continue;
	}
	else{
	  state = CBTOMAIN;
	  current_client = 2;
	}
      }
      // Switch to that client
      printf("Current Client is: %d", current_client);
      break;
    case CATOMAIN: // clientA to main
      printf("Case - Client A to Main\n");

      // Accept the connection and store in fd
      client_a_addr_sz = sizeof client_a_addr;
      client_a_fd = accept(main_server_tcp_a_fd,
			   (struct sockaddr *)&client_a_addr,
			   &client_a_addr_sz);
      // Check fd
      if(client_a_fd == -1){
	perror("Client to Main Server: Accept Error!\n");
	continue;
      }

      // Port/ Where connection came from
      client_a_port = get_port((struct sockaddr *)&client_a_addr);

      // From bgnet forking
      if(client_a_fd != -1){ // this is the child process
	// Receive the msg from clientA
	num_bytes_recv = receive_client_msg(&buf,
					    BUFLEN,
					    client_a_fd);

	printf("Buffer from client is: %s\n", buf);
	// Parse the msg and fill the variables
	parse_client_msg(&client_request_type,
			 username,
			 sender,
			 receiver,
			 &transfer_amount,
			 buf,
			 client_a_port);

	printf("Request Type: %d\n", client_request_type);
	printf("Username: %s\n", username);
	printf("Sender: %s\n", sender);
	printf("Receiver: %s\n", receiver);
	printf("Transfer Amount %d\n", transfer_amount);
	
	// Check the type of request and set the state
	if(client_request_type == 1){
	  printf("The main server received input=%s from the client using TCP over port %d\n", username, client_a_port);
	  state = (num_bytes_recv != -1 ? MAINTOSA : CTOMAIN);
	}
	else if(client_request_type == 2){
	  printf("The main server received from %s to transfer %d coins to %s using TCP over port %d\n",
		 sender,
		 transfer_amount,
		 receiver,
		 client_a_port);
	  state = (num_bytes_recv != -1 ? MAINTOSA : CTOMAIN);
	}
	else if(client_request_type == 3){
	   printf("Main server received a TXLIST reqeust\n");
	   state = (num_bytes_recv != -1 ? MAINTOSA : CTOMAIN);
	}
	else{ // Invalid query
	  printf("Invalid Query\n");
	}	
      }
      break;
    case MAINTOCA: // Main to clientA
      printf("Case - Main to Client A\n");
      if(client_request_type == 3){
	// Gather and send
	num_bytes_sent = gather_and_send_txlist_completion(buf,
							   client_a_fd);
	printf("The main server sent finished sending txlist confirmation to client.\n");
      }
      else{
	// Prepare the buffer and send the data
	num_bytes_sent = gather_and_send_to_client(buf,
						   username,
						   balance,
						   user_found,
						   sender,
						   sender_balance,
						   sender_found,
						   receiver,
						   receiver_balance,
						   receiver_found,
						   client_request_type,
						   client_a_fd,
						   append_transaction);
	if(client_request_type == 1){
	  printf("The main server sent the current balance to client A.\n");
	}
	else if(client_request_type == 2){
	  printf("The main server sent the result of the transaction to client A.\n");
	}
      }

      // clear state variables for next use
      clear_session_variables(&client_request_type,
			      &transfer_amount,
			      &max_transaction_index,
			      &append_transaction,
			      username,
			      &balance,
			      &user_found,
			      sender,
			      &sender_balance,
			      &sender_found,
			      receiver,
			      &receiver_balance,
			      &receiver_found);
      
      // Update state to wait for connection from client
      state = (num_bytes_sent != -1 ? CTOMAIN : state);
      break;
    case CBTOMAIN: // clientB to main
      printf("Case - Client B to Main\n");

      // Accept the connection and store in fd
      client_b_addr_sz = sizeof client_a_addr;
      client_b_fd = accept(main_server_tcp_b_fd,
			   (struct sockaddr *)&client_b_addr,
			   &client_b_addr_sz);
      // Check fd
      if(client_b_fd == -1){
	perror("Client to Main Server: Accept Error!\n");
	continue;
      }

      // Port/ Where connection came from
      client_b_port = get_port((struct sockaddr *)&client_b_addr);

      // From bgnet forking
      if(client_b_fd != -1){ // this is the child process
	// Receive the msg from clientA
	num_bytes_recv = receive_client_msg(&buf,
					    BUFLEN,
					    client_b_fd);

	printf("Buffer from client is: %s\n", buf);
	// Parse the msg and fill the variables
	parse_client_msg(&client_request_type,
			 username,
			 sender,
			 receiver,
			 &transfer_amount,
			 buf,
			 client_b_port);

	printf("Request Type: %d\n", client_request_type);
	printf("Username: %s\n", username);
	printf("Sender: %s\n", sender);
	printf("Receiver: %s\n", receiver);
	printf("Transfer Amount %d\n", transfer_amount);
	
	// Check the type of request and set the state
	if(client_request_type == 1){
	  printf("The main server received input=%s from the client using TCP over port %d\n", username, client_b_port);
	  state = (num_bytes_recv != -1 ? MAINTOSA : CTOMAIN);
	}
	else if(client_request_type == 2){
	  printf("The main server received from %s to transfer %d coins to %s using TCP over port %d\n",
		 sender,
		 transfer_amount,
		 receiver,
		 client_b_port);
	  state = (num_bytes_recv != -1 ? MAINTOSA : CTOMAIN);
	}
	else if(client_request_type == 3){
	   printf("Main server received a TXLIST reqeust\n");
	   state = (num_bytes_recv != -1 ? MAINTOSA : CTOMAIN);
	}
	else{ // Invalid query
	  printf("Invalid Query\n");
	}	
      }
      break;
    case MAINTOCB: // Main to clientB
      printf("Case - Main to Client B\n");

      if(client_request_type == 3){
	// Gather and send
	num_bytes_sent = gather_and_send_txlist_completion(buf,
							   client_b_fd);
	printf("The main server sent finished sending txlist confirmation to client.\n");
      }
      else{
	// Prepare the buffer and send the data
	num_bytes_sent = gather_and_send_to_client(buf,
						   username,
						   balance,
						   user_found,
						   sender,
						   sender_balance,
						   sender_found,
						   receiver,
						   receiver_balance,
						   receiver_found,
						   client_request_type,
						   client_b_fd,
						   append_transaction);
	if(client_request_type == 1){
	  printf("The main server sent the current balance to client B.\n");
	}
	else if(client_request_type == 2){
	  printf("The main server sent the result of the transaction to client B.\n");
	}
      }

      // clear state variables for next use
      clear_session_variables(&client_request_type,
			      &transfer_amount,
			      &max_transaction_index,
			      &append_transaction,
			      username,
			      &balance,
			      &user_found,
			      sender,
			      &sender_balance,
			      &sender_found,
			      receiver,
			      &receiver_balance,
			      &receiver_found);
      
      // Update state to wait for connection from client
      state = (num_bytes_sent != -1 ? CTOMAIN : state);
      break;
    case MAINTOSA: // Main to Server A
      printf("Case - Main to Server A\n");

      // Get the port server A
      sva_port = get_port((struct sockaddr *)sva_cnntn->ai_addr);
      
      // Check the type of request to send
      if(client_request_type != 3){
	num_bytes_sent = gather_and_send_udp_msg(main_server_udp_fd,
						 buf,
						 sva_cnntn,
						 username,
						 balance,
						 sender,
						 sender_balance,
						 receiver,
						 receiver_balance,
						 transfer_amount,
						 client_request_type,
						 max_transaction_index,
						 append_transaction);

	printf("The main server sent a request to server A\n");
	
	// Wait to receive response from server A
	num_bytes_recv = prep_and_receive_udp_data(&buf,
						   main_server_udp_fd,
						   sva_cnntn);

	// Parse and store the response from serverA
	parse_and_store_udp(buf,
			    username,
			    &user_found_tmp,
			    &balance,
			    sender,
			    &sender_balance,
			    &sender_found_tmp,
			    receiver,
			    &receiver_balance,
			    &receiver_found_tmp,
			    &max_transaction_index,
			    client_request_type);
      
	printf("Sender: %s\n", sender);
	printf("Sender Balance: %d\n", sender_balance);
	printf("Sender Found: %d\n", sender_found);
	printf("Receiver: %s\n", receiver);
	printf("Receiver Balance: %d\n", receiver_balance);
	printf("Receiver Found: %d\n", receiver_found);
	printf("Max Transaction Index %d\n", max_transaction_index);

	update_found_status(&user_found,
			    user_found_tmp);
	update_found_status(&sender_found,
			    sender_found_tmp);
	update_found_status(&receiver_found,
			    receiver_found_tmp);
	
	// Check if the append flag is set
	if(append_transaction){
	  if(current_client == 1){
	    state = (num_bytes_recv != -1 ? MAINTOCA : state);
	  }
	  else{
	    state = (num_bytes_recv != -1 ? MAINTOCB : state);
	  }
	}
	else{
	  state = (num_bytes_recv != -1 ? MAINTOSB : state);
	}
      }
      else{
	// Gather and sender the udp message to server A
	num_bytes_sent = gather_and_send_list_request(buf,
						      sva_cnntn,
						      client_request_type,
						      main_server_udp_fd);
	printf("The main server sent a request to Server A\n");

	// Sit and wait in a while loop until all data is collected
	collect_backend_transactions(&txlist,
				     main_server_udp_fd,
				     sva_cnntn);
	state = MAINTOSB;
      }
      
      if(client_request_type == 1){
	printf("The main server received transactions from Server A using UDP over port %d.\n", sva_port);
      }
      else if(client_request_type == 2){
	printf("The main server received the feedback from server A using UDP over port %d.\n", sva_port);
      }
      else if(client_request_type == 3){
	printf("The main server received the feedback from server A using UDP over port %d.\n", sva_port);
      }
      else{
	printf("Other Case.\n");
      }
      
      printf("Buffer returned from server A: %s\n", buf);
      break;
  
    case MAINTOSB: // Main to Server B
      printf("Case - Main to Server B\n");

      // Get the port server A
      svb_port = get_port((struct sockaddr *)svb_cnntn->ai_addr);
      
      // Check the type of request to send
      if(client_request_type != 3){
	num_bytes_sent = gather_and_send_udp_msg(main_server_udp_fd,
						 buf,
						 svb_cnntn,
						 username,
						 balance,
						 sender,
						 sender_balance,
						 receiver,
						 receiver_balance,
						 transfer_amount,
						 client_request_type,
						 max_transaction_index,
						 append_transaction);

	printf("The main server sent a request to Server B\n");
	
	// Wait to receive response from Server B
	num_bytes_recv = prep_and_receive_udp_data(&buf,
						   main_server_udp_fd,
						   svb_cnntn);

	// Parse and store the response from serverA
	parse_and_store_udp(buf,
			    username,
			    &user_found_tmp,
			    &balance,
			    sender,
			    &sender_balance,
			    &sender_found_tmp,
			    receiver,
			    &receiver_balance,
			    &receiver_found_tmp,
			    &max_transaction_index,
			    client_request_type);
      
	printf("Sender: %s\n", sender);
	printf("Sender Balance: %d\n", sender_balance);
	printf("Sender Found: %d\n", sender_found);
	printf("Receiver: %s\n", receiver);
	printf("Receiver Balance: %d\n", receiver_balance);
	printf("Receiver Found: %d\n", receiver_found);
	printf("Max Transaction Index %d\n", max_transaction_index);

	update_found_status(&user_found,
			    user_found_tmp);
	update_found_status(&sender_found,
			    sender_found_tmp);
	update_found_status(&receiver_found,
			    receiver_found_tmp);
	
	// Check if the append flag is set
	if(append_transaction){
	  if(current_client == 1){
	    state = (num_bytes_recv != -1 ? MAINTOCA : state);
	  }
	  else{
	    state = (num_bytes_recv != -1 ? MAINTOCB : state);
	  }
	}
	else{
	  state = (num_bytes_recv != -1 ? MAINTOSC : state);
	}
      }
      else{
	// Gather and sender the udp message to Server B
	num_bytes_sent = gather_and_send_list_request(buf,
						      svb_cnntn,
						      client_request_type,
						      main_server_udp_fd);
	printf("The main server sent a request to server B\n");

	// Sit and wait in a while loop until all data is collected
	collect_backend_transactions(&txlist,
				     main_server_udp_fd,
				     svb_cnntn);
	state = MAINTOSC;
      }
      
      if(client_request_type == 1){
	printf("The main server received transactions from Server B using UDP over port %d.\n", svb_port);
      }
      else if(client_request_type == 2){
	printf("The main server received the feedback from Server B using UDP over port %d.\n", svb_port);
      }
      else if(client_request_type == 3){
	printf("The main server received the feedback from Server B using UDP over port %d.\n", svb_port);
      }
      else{
	printf("Other Case.\n");
      }
      
      printf("Buffer returned from server B: %s\n", buf);
      break;
      
    case MAINTOSC: // Main to Server C
      printf("Case - Main to Server C\n");

      // Get the port server C
      svc_port = get_port((struct sockaddr *)svc_cnntn->ai_addr);
      
      // Check the type of request to send
      if(client_request_type != 3){
	num_bytes_sent = gather_and_send_udp_msg(main_server_udp_fd,
						 buf,
						 svc_cnntn,
						 username,
						 balance,
						 sender,
						 sender_balance,
						 receiver,
						 receiver_balance,
						 transfer_amount,
						 client_request_type,
						 max_transaction_index,
						 append_transaction);

	printf("The main server sent a request to Server C\n");
	
	// Wait to receive response from server A
	num_bytes_recv = prep_and_receive_udp_data(&buf,
						   main_server_udp_fd,
						   svc_cnntn);

	// Parse and store the response from serverA
	parse_and_store_udp(buf,
			    username,
			    &user_found_tmp,
			    &balance,
			    sender,
			    &sender_balance,
			    &sender_found_tmp,
			    receiver,
			    &receiver_balance,
			    &receiver_found_tmp,
			    &max_transaction_index,
			    client_request_type);
      
	printf("Sender: %s\n", sender);
	printf("Sender Balance: %d\n", sender_balance);
	printf("Sender Found: %d\n", sender_found);
	printf("Receiver: %s\n", receiver);
	printf("Receiver Balance: %d\n", receiver_balance);
	printf("Receiver Found: %d\n", receiver_found);
	printf("Max Transaction Index %d\n", max_transaction_index);

	update_found_status(&user_found,
			    user_found_tmp);
	update_found_status(&sender_found,
			    sender_found_tmp);
	update_found_status(&receiver_found,
			    receiver_found_tmp);
	
	// Check if the append flag is set
	if(append_transaction){
	  if(current_client == 1){
	    state = (num_bytes_recv != -1 ? MAINTOCA : state);
	  }
	  else{
	    state = (num_bytes_recv != -1 ? MAINTOCB : state);
	  }
	  break;
	}
      }
      else{
	// Gather and sender the udp message to Server C
	num_bytes_sent = gather_and_send_list_request(buf,
						      svc_cnntn,
						      client_request_type,
						      main_server_udp_fd);
	printf("The main server sent a request to Server C\n");

	// Sit and wait in a while loop until all data is collected
	collect_backend_transactions(&txlist,
				     main_server_udp_fd,
				     svc_cnntn);
      }
      // Print out the buffer
      printf("Buffer returned from server C: %s\n", buf);
      
      if(client_request_type == 1){
	printf("The Main Server received transactions from Server C using UDP over port %d.\n", svc_port);
	state = (current_client == 1) ? MAINTOCA : MAINTOCB;
	break;
      }
      
      else if(client_request_type == 2){
	printf("The Main Server received the feedback from Server C using UDP over port %d.\n", svc_port);
	// Check if the transaciton is valid
	if(valid_transaction(sender,
			     sender_balance,
			     sender_found,
			     receiver,
			     receiver_balance,
			     receiver_found,
			     transfer_amount)){
	
	  printf("Valid Transaction\n");
	
	  // Get the random server to add to
	  int random_server = get_random_server();
	  printf("Random server is: %d\n", random_server);
	
	  // Switch to the random server
	  if(random_server == 0){
	    printf("Random Server is A\n");
	    state = MAINTOSA;
	  }
	  else if(random_server == 1){
	    printf("Random Server is B\n");
	    state = MAINTOSB;
	  }
	  else{
	    printf("Random Server is C\n");
	    state = MAINTOSC;
	  }
	  
	  // Set the append transaction to true to write to random server
	  append_transaction = 1;
	}
	else{
	  printf("Invalid Transaction\n");
	  state = (current_client == 1) ? MAINTOCA : MAINTOCB;
	  break;
	}
      }
      
      else if(client_request_type == 3){
	printf("The Main Server received the feedback from Server C using UDP over port %d.\n", svc_port);
	  
	printf("Outputting File of transactions\n");
	  
	// Write the transaction list to file
	output_txlist(&txlist);

	// Switch state to return to main
	state = (current_client == 1) ? MAINTOCA : MAINTOCB;
	break;
      }
      else{
	printf("Other Case.\n");
	state = (current_client == 1) ? MAINTOCA : MAINTOCB;
      }		
      break;
    case ENDTRANS:
    default: // All other cases error
      printf("Case - Default\n");
      break;
    } // Switch
    if(state == ENDTRANS){
      printf("Ending While loop\n");
      break;
    }
  } // While loop

  // Close descriptor
  close(client_a_fd);
  close(client_b_fd);
  
  // Free memory
  free(buf);
  free(username);
  free(sender);
  free(receiver);

  // Return from main
  return 0;
}
