// Steven Karl
// Main Server
// ---------------

// Include Statements
#include "serverM.h"

// Define Macros
#define SVRAPORT "21711" // Port # ServerA runs on
#define SVRBPORT "22711" // Port # ServerB runs on
#define SVRCPORT "23711" // Port # ServerC runs on
#define MAINUDPPORT "24711" // Port # Main Server UDP runs on
#define MAINTCPPORTA "25711" // Port # Main Server TCP runs on
#define MAINTCPPORTB "26711" // Port # Main Server TCP runs on

#define BACKLOG 10 // max len(pending connections queue) for listening
#define BUFLEN 2048

// ----------------
// Local Functions
// ----------------

void
verify_input_count(int argc){
  // Check the argc variable
  if (argc > 1){
    fprintf(stderr, "Input Error\n");
    exit(1);
  }
}

void
getaddrinfo_error(int return_value){
  if(return_value){
    fprintf(stderr,
	    "getaddrinfo: %s\n",
	    gai_strerror(return_value));
    exit(1);
  }
}

void
tcp_socket_setup(struct addrinfo *socket_prefs){
  memset(socket_prefs, 0, sizeof(*socket_prefs)); // make sure the struct is empty
  socket_prefs->ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
  socket_prefs->ai_socktype = SOCK_STREAM; // TCP stream sockets
  socket_prefs->ai_flags = AI_PASSIVE; //fill in my IP for me
}

void
udp_socket_setup(struct addrinfo *socket_prefs){
  memset(socket_prefs, 0, sizeof(*socket_prefs)); // make sure the struct is empty
  socket_prefs->ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
  socket_prefs->ai_socktype = SOCK_DGRAM; // UDP Datagram sockets
}

void
get_available_socket(struct addrinfo **cxn,
		     struct addrinfo *cxns){
  // Loop through all the results and connect to the first one we can
  int socket_status;
  for(*cxn = cxns;
      *cxn != NULL;
      *cxn = (*cxn)->ai_next){
    // Attempt to create the socket
    socket_status = socket((*cxn)->ai_family,
			   (*cxn)->ai_socktype,
			   0);
    // Check the return code and continue else break out
    if(socket_status == -1){
      perror("Failed to create socket for this connection");
      continue;
    }
    // If we made it here a cxn was made so break out
    break;
  }
}

void
tcp_bind_to_available_socket(int *sock_fd,
		   struct addrinfo *cxns){
  /* Local Variables */
  int yes = 1; // non-zero signals a sock option will change
  int bind_status; // Return code for bind call
  int setsockopt_status; // Return code for setsockopt call
  struct addrinfo *cxn;

  /* Loop through possible connections and bind to first one we can */
  for (cxn = cxns;
       cxn != NULL;
       cxn = cxn->ai_next){
    /* Attempt to create a socket from the cxn */
    *sock_fd = socket(cxn->ai_family,
		      cxn->ai_socktype,
		      cxn->ai_protocol);
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
		       cxn->ai_addr,
		       cxn->ai_addrlen);
    // Check the bind status
    if(bind_status == -1){
      close(*sock_fd);
      perror("Error: bind failed");
      continue;
    }
    // Socket and bind complete break for first found
    break;
  }

  // Check if we made it through the cxns without a valid socket
  if(cxn == NULL){
    fprintf(stderr, "Error: Failed to get an available socket\n");
    exit(2);
  }
}

void
udp_bind_to_available_socket(int *sock_fd,
			     struct addrinfo *cxns){
  // Setup local variables
  int bind_status;
  struct addrinfo *cxn;

  // Loop through the possible connections and bind to the first available
  for(cxn = cxns;
      cxn != NULL;
      cxn = cxn->ai_next){
    // Attempt to create the socket
    *sock_fd = socket(cxn->ai_family,
		      cxn->ai_socktype,
		      cxn->ai_protocol);
    // Check return code
    if(*sock_fd == -1){
      perror("Error: socket creation failed");
      continue;
    }
    
    // Attempt to bind
    bind_status = bind(*sock_fd,
		       cxn->ai_addr,
		       cxn->ai_addrlen);
    // Check return code
    if(bind_status == -1){
      close(*sock_fd);
      perror("Error: bind failed");
      continue;
    }
    break;
  }
  // Check if the cxn is null after going through all of the linked list
  if(cxn == NULL){
    fprintf(stderr, "mainudp: fail to connect\n");
    exit(2);
  }
}

void
tcp_listen_for_cxn(int *sock_fd,
		   int backlog){
  // Local variables
  int listen_status;

  // Call to the listen - to start listening 
  listen_status = listen(*sock_fd, backlog);

  // Check the return code 
  if(listen_status == -1){
    perror("Error: Failed call to listen\n");
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
reap_dead_processes(struct sigaction *sa){

  // Local variable defintion
  int sigaction_status;
  sa->sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa->sa_mask);
  sa->sa_flags = SA_RESTART;
  sigaction_status = sigaction(SIGCHLD, sa, NULL);

  // Check return code
  if(sigaction_status  == -1){
    perror("Error: Call to sigaction failed\n");
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
  
  // Check if the addr input is v4
  if(addr->sa_family == AF_INET){
    // Create memory to store result of inet_ntop
    char *their_addr = (char *)calloc(INET_ADDRSTRLEN, sizeof(*their_addr));

    // Convert the ip address to human readable form
    inet_ntop(addr->sa_family,
	      get_in_addr((struct sockaddr *)&addr),
	      their_addr,
	      INET_ADDRSTRLEN);
    printf("Server: Got connection from %s\n", their_addr);

    // Free up the memory created
    free(their_addr);
    return ((struct sockaddr_in *)&addr)->sin_port;
  }
  // Check if the addr input is v6
  else if(addr->sa_family == AF_INET6){
    // Create memory to store result of inet_ntop
    char *their_addr = (char *)calloc(INET6_ADDRSTRLEN, sizeof(*their_addr));

    // Convert the address to human readable form
    inet_ntop(addr->sa_family,
	      get_in_addr((struct sockaddr *)&addr),
	      their_addr,
	      INET6_ADDRSTRLEN);
    printf("Server: Got connection from %s\n", their_addr);

    // Free up the memory
    free(their_addr);
    return ((struct sockaddr_in6 *)&addr)->sin6_port;
  }
  // If not v4 or v6 return and error
  else{ // Invalid
    perror("Error: Not IPv4 or IPv6\n");
    exit(1);
  }
}

int
receive_client_msg(char **buf,
		   int sock_fd){
  // Call recv to get the msg from the client fd
  int num_bytes;
  num_bytes = recv(sock_fd,
		   *buf,
		   BUFLEN,
		   0);

  // Check for bad return value -1
  if(num_bytes == -1){
    perror("Error: Recv call failed\n");
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
  // If its a TXLIST request it just compares and breaks
  if(strncmp(buf, "TXLIST", 256) == 0){
    *client_request_type = 3;
  }
  // 1 input which means that its a balance request
  else if(spaces == 0){
    *client_request_type = 1;
  }
  // 3 inputs so its a transfer request
  else if(spaces == 2){
    *client_request_type = 2;
  }
  // Doesnt match any known request type
  else{
    perror("Error: Unknown request type can't process\n");
    //printf("Bad Request\n");
    *client_request_type = 0;
  }
}

void
parse_client_msg(int *client_request_type,
		 char *username,
		 char *sender,
		 char *receiver,
		 int *transfer_amount,
		 char *buf){

  // Get the request type
  get_request_type(buf,
		   client_request_type);

  // Parse based on type
  if(*client_request_type == 1){ // Balance request
    sscanf(buf, "%s", username);

  }
  else if(*client_request_type == 2){ // Transfer request
    sscanf(buf, "%s %s %d", sender, receiver, transfer_amount);
    strncpy(username, sender, BUFLEN);
  }
  else if(*client_request_type == 3){ // List request
    return;
  }
  else{
    printf("Bad Arguments\n");
  }
}

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
			   int append_transaction){
  // Pack based on request
  if(client_request_type == 1){
    snprintf(buf,
	     BUFLEN,
	     "%s %d",
	     username,
	     balance);
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
			 cxn->ai_addr,
			 cxn->ai_addrlen);
  // Check return
  if(num_bytes == -1){
    perror("Error: Failed to send bytes to Server A\n");
    exit(1);
  }
  return num_bytes;
}

int
list_request_send_to_backend(char *buf,
			     struct addrinfo *cxn,
			     int type,
			     int sock_fd){
  // Load the buffer
  snprintf(buf,
	   BUFLEN,
	   "%d",
	   type);

  // Send the data
  int num_bytes = sendto(sock_fd,
			 buf,
			 BUFLEN,
			 0,
			 cxn->ai_addr,
			 cxn->ai_addrlen);
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

    // Check for a duplicate
    if(new_tx_index == current_tx_index){
      prev = current;
      break;
    }
    
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
			     struct addrinfo *cxn){
  // Define buffer to store transactions
  char *tx = (char *)calloc(BUFLEN, sizeof(*tx));
  
  // Loop until buffer returns done
  while(1){
    receive_backend_data(&tx,
			 sock_fd,
			 cxn);

    // Check for end
    if(strncmp(tx, "Done", BUFLEN) == 0){
      
      break;
    }
    // Add to tx list
    add_transaction_to_list(txlist,
			    tx);
  }
  // Free mem
  free(tx);
}

int
receive_backend_data(char **buf,
		     int sock_fd,
		     struct addrinfo *back_svr_cxn){
  // Clear the buffer
  memset(*buf, 0, BUFLEN * sizeof(**buf));
  
  // Receive from backend server
  int num_bytes = recvfrom(sock_fd,
			   *buf,
			   BUFLEN,
			   MSG_WAITALL,
			   back_svr_cxn->ai_addr,
			   &back_svr_cxn->ai_addrlen);
  
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
			     int client_request_type){


  // Check request type
  if(client_request_type == 1){ // Balance
    // Parse the input buffer
    sscanf(buf,
	   "%s %d %d",
	   username,
	   balance,
	   user_found);
  }
  else if(client_request_type == 2){ // Transfer
    // Parse the input buffer
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
			  int append_transaction){
  // Check the type of request
  if(client_request_type == 1){ // Balance
    // Load the return buffer
    snprintf(buf,
	     BUFLEN,
	     "%s %d %d",
	     username,
	     balance,
	     user_found);
  }
  else if(client_request_type == 2){ // Transfer
    // Load the return buffer
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
  else{ // Invalid
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
  // Seed the time
  srand(time(NULL));
  // Return a random number on a 3 scale (0,1,2)
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
  // Open the output file for the transaction list
  *fout = fopen("alichain.txt","w+");

  // Check if there was an error in opening said output file
  if(*fout == NULL){
    perror("ERROR: Could not open output file for writing.\n");
    exit(1);
  }
}

void
close_output_file(FILE **fout){
  //  Close the output file 
  int close_status = fclose(*fout);

  // Check the return code to make sure it closed properly
  if(close_status != 0){
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
send_txlist_completion_to_client(char *buf,
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
  // Nonzero if found zero if no backend found the user
  *found+=tmp;  
}

// Main Function
int main(int argc, const char *argv[]){

  // Verify inputs - Main takes none should be 0
  verify_input_count(argc);

  // Local variables
  char *buf = (char *)calloc(BUFLEN, sizeof *buf);
  int num_bytes_sent;
  int num_bytes_recv;
  

  int main_server_udp_fd; // File descriptor udp
  int gai_ret_val; // Function return code on error

  struct addrinfo udp_sock_prefs; // udp connection prefs
  struct addrinfo *udp_cxns;
  
  // Client A
  int main_server_tcp_a_fd; // File descriptor tcp client A
  struct addrinfo tcp_a_sock_prefs; // tcp connection prefs
  struct addrinfo *tcp_a_cxns; 
  int client_a_fd;
  struct sockaddr_storage client_a_addr;
  socklen_t client_a_addr_sz;
  int client_a_port;
  
  // Client B
  int main_server_tcp_b_fd; // File descriptor tcp client B
  struct addrinfo tcp_b_sock_prefs; // tcp connection prefs
  struct addrinfo *tcp_b_cxns;
  int client_b_fd;
  struct sockaddr_storage client_b_addr;
  socklen_t client_b_addr_sz;
  int client_b_port;
  
  // Backend Server A
  struct addrinfo sva_sock_prefs; // General Socket Preferences
  struct addrinfo *sva_cxns; // Return Linked List
  struct addrinfo *sva_cxn; // One of the items in list
  int sva_port; // Used later backend server side interface
  udp_socket_setup(&sva_sock_prefs); // AI_FAMILY/TYPE
  gai_ret_val = getaddrinfo(NULL,
			    SVRAPORT,
			    &sva_sock_prefs,
			    &sva_cxns); // setup structs
  getaddrinfo_error(gai_ret_val); // Check for non zero
  get_available_socket(&sva_cxn,
		       sva_cxns); // Socket() call for returned connections

  // Backend Server B
  struct addrinfo svb_sock_prefs;
  struct addrinfo *svb_cxn;
  struct addrinfo *svb_cxns;
  int svb_port;
  udp_socket_setup(&svb_sock_prefs);
  gai_ret_val = getaddrinfo(NULL,
			    SVRBPORT,
			    &svb_sock_prefs,
			    &svb_cxns);
  getaddrinfo_error(gai_ret_val);
  get_available_socket(&svb_cxn,
		       svb_cxns);
  
  // Backend Server C
  struct addrinfo svc_sock_prefs;
  struct addrinfo *svc_cxns;
  struct addrinfo *svc_cxn;
  int svc_port;
  udp_socket_setup(&svc_sock_prefs);
  gai_ret_val = getaddrinfo(NULL,
			    SVRCPORT,
			    &svc_sock_prefs,
			    &svc_cxns);
  getaddrinfo_error(gai_ret_val);
  get_available_socket(&svc_cxn,
		       svc_cxns);

  // Setup Main Server TCP Support Client A
  tcp_socket_setup(&tcp_a_sock_prefs);
  gai_ret_val = getaddrinfo(NULL,
			    MAINTCPPORTA,
			    &tcp_a_sock_prefs,
			    &tcp_a_cxns);
  getaddrinfo_error(gai_ret_val);

  // Setup Main Server TCP Support Client B
  tcp_socket_setup(&tcp_b_sock_prefs);
  gai_ret_val = getaddrinfo(NULL,
			    MAINTCPPORTB,
			    &tcp_b_sock_prefs,
			    &tcp_b_cxns);
  getaddrinfo_error(gai_ret_val);

  // Setup Main Server UDP Support
  udp_socket_setup(&udp_sock_prefs);
  gai_ret_val = getaddrinfo(NULL,
			    MAINUDPPORT,
			    &udp_sock_prefs,
			    &udp_cxns);
  getaddrinfo_error(gai_ret_val);

  // TCP A - Create and Bind socket
  tcp_bind_to_available_socket(&main_server_tcp_a_fd,
			       tcp_a_cxns);
  freeaddrinfo(tcp_a_cxns);

  // TCP B - Create and Bind socket
  tcp_bind_to_available_socket(&main_server_tcp_b_fd,
			       tcp_b_cxns);
  freeaddrinfo(tcp_b_cxns);

  // UDP - Create and Bind socket
  udp_bind_to_available_socket(&main_server_udp_fd,
			       udp_cxns);
  freeaddrinfo(udp_cxns);

  // Listen for connections
  tcp_listen_for_cxn(&main_server_tcp_a_fd, BACKLOG);
  tcp_listen_for_cxn(&main_server_tcp_b_fd, BACKLOG);

  // Reap all the dead processes
  struct sigaction sa;
  reap_dead_processes(&sa);
  
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

  // Select() handle multiple clients
  fd_set rset;
  int maxfdp1 = (main_server_tcp_a_fd > main_server_tcp_b_fd) ? (main_server_tcp_a_fd + 1) : (main_server_tcp_b_fd + 1);
  
  // Connection Loop
  while(1){ // main accept() loop
    
    // Empty out the buffer before each iteration
    memset(buf, 0, BUFLEN * sizeof(*buf));
      
    // Switch on Unknown Client, clientA, clientB, SVRA, SVRB, SVRC
    switch(state){    
    case CTOMAIN:
      // Add both client descriptors to the set
      FD_ZERO(&rset);
      FD_SET(main_server_tcp_a_fd, &rset);
      FD_SET(main_server_tcp_b_fd, &rset);

      // Select 
      if(select(maxfdp1, &rset, NULL, NULL, NULL) == -1){
	perror("Error: Problem selecting a client fd\n");
	exit(1);
      }

      if(FD_ISSET(main_server_tcp_a_fd, &rset)){
	current_client = 1;
      }
      
      else if(FD_ISSET(main_server_tcp_b_fd, &rset)){
	current_client = 2;
      }
      else{
	printf("No Change...Refreshing loop\n");
	sleep(3);
	break;
      }
      
      // Switch to that client
      state = (current_client == 1) ? CATOMAIN : CBTOMAIN;
      break;
    case CATOMAIN: // clientA to main

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
      //client_a_port = get_port((struct sockaddr *)&client_a_addr);
      client_a_port = ((struct sockaddr_in *)&client_a_addr)->sin_port;
      // From bgnet forking
      if(client_a_fd != -1){ // this is the child process
	// Receive the msg from clientA
	num_bytes_recv = receive_client_msg(&buf,
					    client_a_fd);

	// Parse the msg and fill the variables
	parse_client_msg(&client_request_type,
			 username,
			 sender,
			 receiver,
			 &transfer_amount,
			 buf);
	
	// Check the type of request and set the state
	if(client_request_type == 1){
	  printf("The main server received input = %s from the client using TCP over port %d\n", username, client_a_port);
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
	   printf("Main server received a TXLIST request\n");
	   state = (num_bytes_recv != -1 ? MAINTOSA : CTOMAIN);
	}
	else{ // Invalid query
	  printf("Invalid Query\n");
	}	
      }
      break;
    case MAINTOCA: // Main to clientA
      if(client_request_type == 3){
	// Gather and send
	num_bytes_sent = send_txlist_completion_to_client(buf,
							  client_a_fd);
	printf("The main server sent finished sending txlist confirmation to client.\n");
      }
      else{
	// Prepare the buffer and send the data
	num_bytes_sent = bt_request_send_to_client(buf,
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
      //client_b_port = get_port((struct sockaddr *)&client_b_addr);
      client_b_port = ((struct sockaddr_in *)&client_b_addr)->sin_port;
      // From bgnet forking
      if(client_b_fd != -1){ // this is the child process
	// Receive the msg from clientA
	num_bytes_recv = receive_client_msg(&buf,
					    client_b_fd);
	
	// Parse the msg and fill the variables
	parse_client_msg(&client_request_type,
			 username,
			 sender,
			 receiver,
			 &transfer_amount,
			 buf);
	
	// Check the type of request and set the state
	if(client_request_type == 1){
	  printf("The main server received input = %s from the client using TCP over port %d\n", username, client_b_port);
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
	   printf("Main server received a TXLIST request\n");
	   state = (num_bytes_recv != -1 ? MAINTOSA : CTOMAIN);
	}
	else{ // Invalid query
	  printf("Invalid Query\n");
	}	
      }
      break;
    case MAINTOCB: // Main to clientB

      if(client_request_type == 3){
	// Gather and send
	num_bytes_sent = send_txlist_completion_to_client(buf,
							  client_b_fd);
	printf("The main server sent finished sending txlist confirmation to client.\n");
      }
      else{
	// Prepare the buffer and send the data
	num_bytes_sent = bt_request_send_to_client(buf,
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

      // Get the port server A
      // sva_port = get_port((struct sockaddr *)sva_cxn->ai_addr);
      sva_port = ((struct sockaddr_in *)&sva_cxn->ai_addr)->sin_port;
      // Check the type of request to send
      if(client_request_type != 3){
	num_bytes_sent = bt_request_send_to_backend(main_server_udp_fd,
						    buf,
						    sva_cxn,
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
	num_bytes_recv = receive_backend_data(&buf,
					      main_server_udp_fd,
					      sva_cxn);

	// Parse and store the response from serverA
	parse_and_store_from_backend(buf,
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
	num_bytes_sent = list_request_send_to_backend(buf,
						      sva_cxn,
						      client_request_type,
						      main_server_udp_fd);
	printf("The main server sent a request to Server A\n");

	// Sit and wait in a while loop until all data is collected
	collect_backend_transactions(&txlist,
				     main_server_udp_fd,
				     sva_cxn);
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
      break;
  
    case MAINTOSB: // Main to Server B

      // Get the port server A
      //svb_port = get_port((struct sockaddr *)svb_cxn->ai_addr);
      svb_port = ((struct sockaddr_in *)&svb_cxn->ai_addr)->sin_port;
      // Check the type of request to send
      if(client_request_type != 3){
	num_bytes_sent = bt_request_send_to_backend(main_server_udp_fd,
						    buf,
						    svb_cxn,
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
	num_bytes_recv = receive_backend_data(&buf,
					      main_server_udp_fd,
					      svb_cxn);

	// Parse and store the response from serverA
	parse_and_store_from_backend(buf,
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
	num_bytes_sent = list_request_send_to_backend(buf,
						      svb_cxn,
						      client_request_type,
						      main_server_udp_fd);
	printf("The main server sent a request to server B\n");

	// Sit and wait in a while loop until all data is collected
	collect_backend_transactions(&txlist,
				     main_server_udp_fd,
				     svb_cxn);
	state = MAINTOSC;
      }
      
      if(client_request_type == 1){
	printf("The main server received transactions from Server B using UDP over port %d.\n", svb_port);
      }
      else if(client_request_type == 2){
	printf("The main server received the feedback from Server B using UDP over port %d.\n", svb_port);
      }
      else if(client_request_type == 3){
	printf("The main server received the TXLIST feedback from Server B using UDP over port %d.\n", svb_port);
      }
      else{
	printf("Other Case.\n");
      }
      break;
      
    case MAINTOSC: // Main to Server C

      // Get the port server C
      //svc_port = get_port((struct sockaddr *)svc_cxn->ai_addr);
      svc_port = ((struct sockaddr_in *)&svc_cxn->ai_addr)->sin_port;
      // Check the type of request to send
      if(client_request_type != 3){
	num_bytes_sent = bt_request_send_to_backend(main_server_udp_fd,
						    buf,
						    svc_cxn,
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
	num_bytes_recv = receive_backend_data(&buf,
					      main_server_udp_fd,
					      svc_cxn);

	// Parse and store the response from serverA
	parse_and_store_from_backend(buf,
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
	num_bytes_sent = list_request_send_to_backend(buf,
						      svc_cxn,
						      client_request_type,
						      main_server_udp_fd);
	printf("The main server sent a request to Server C\n");

	// Sit and wait in a while loop until all data is collected
	collect_backend_transactions(&txlist,
				     main_server_udp_fd,
				     svc_cxn);
      }
      
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
	
	  // Get the random server to add to
	  int random_server = get_random_server();
	
	  // Switch to the random server
	  if(random_server == 0){
	    printf("The transaction will be added to Server A\n");
	    state = MAINTOSA;
	  }
	  else if(random_server == 1){
	    printf("The transaction will be added to Server B\n");
	    state = MAINTOSB;
	  }
	  else{
	    printf("The transaction will be added to Server C\n");
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
	printf("The Main Server received the TXLIST feedback from Server C using UDP over port %d.\n", svc_port);
	  
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
