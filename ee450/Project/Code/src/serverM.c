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
#define NAMELEN 2048

struct Requests{
  char *username;
  char *sender;
  char *receiver;
  int transfer_amount;
  int request_type;
}request;

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
  // Set request type based on space count
  if(spaces == 0){
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
prep_and_receive_udp_data(char **buf,
			  int sock_fd,
			  struct addrinfo *back_svr_cnntn){
  // Clear the buffer
  memset(*buf, 0, BUFLEN * sizeof(*buf));
  
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
			  int sock_fd){
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
	     "%s %d %d %s %d %d",
	     sender,
	     sender_balance,
	     sender_found,
	     receiver,
	     receiver_balance,
	     receiver_found);
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
  //int client_b_fd;
  //struct sockaddr_storage client_b_addr;
  //socklen_t client_b_addr_sz;
  //int client_b_port;
  
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
  //int svb_port;
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
  //  int svc_port;
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
  int state = CATOMAIN;

  // Define the vars to store in when parsing requests
  int client_request_type = 999;
  int balance = 1000;
  int transfer_amount = 0;
  int max_transaction_index = 0;
  int append_transaction = 0;
  
  char *username = (char *)calloc(NAMELEN, sizeof(*username));
  int user_found = 0;
  
  char *sender = (char *)calloc(NAMELEN, sizeof(*sender));
  int sender_balance = 1000;
  int sender_found = 0;

  char *receiver = (char *)calloc(NAMELEN, sizeof(*receiver));
  int receiver_balance = 1000;
  int receiver_found = 0;
  
  // Connection Loop
  while(1){ // main accept() loop
    
    // Empty out the buffer before each iteration
    memset(buf, 0, BUFLEN * sizeof(*buf));
      
    // Switch on clientA, clientB, SVRA, SVRB, SVRC
    switch(state){    
      
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
	  state = (num_bytes_recv != -1 ? MAINTOSA : state);
	}
	else if(client_request_type == 2){
	  printf("The main server received from %s to transfer %d coins to %s using TCP over port %d\n",
		 sender,
		 transfer_amount,
		 receiver,
		 client_a_port);
	  state = (num_bytes_recv != -1 ? MAINTOSA : state);
	}
	else{ // Invalid query
	  printf("Invalid Query\n");
	}	
      }
      break;
    case MAINTOCA: // Main to clientA
      printf("Case - Main to Client A\n");

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
						 client_a_fd);

      if(client_request_type == 1){
	printf("The main server sent the current balance to client A.\n");
      }
      else if(client_request_type == 2){
	printf("The main server sent the result of the transaction to client A.\n");
      }
      else{
	printf("Other Case.\n");
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
      state = (num_bytes_sent != -1 ? CATOMAIN : state);
      break;
    case CBTOMAIN: // clientB to main
      printf("Case - Client B to Main\n");
      break;
    case MAINTOCB: // Main to clientB
      printf("Case - Main to Client A\n");
      break;
    case MAINTOSA: // Main to Server A
      printf("Case - Main to Server A\n");

      // Get the port server A
      sva_port = get_port((struct sockaddr *)sva_cnntn->ai_addr);
      
      // Check the type of request to send
      printf("The main server sent a request to server A\n");
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

      // Wait to receive response from server A
      num_bytes_recv = prep_and_receive_udp_data(&buf,
						 main_server_udp_fd,
						 sva_cnntn);
      if(client_request_type == 1){
	printf("The main server received transactions from Server A using UDP over port %d.\n", sva_port);
      }
      else if(client_request_type == 2){
	printf("The main server received the feedback from server A using UDP over port %d.\n", sva_port);
      }
      else{
	printf("Other Case.\n");
      }
      
      printf("Buffer returned from server A: %s\n", buf);
      
      // Parse and store the response from serverA
      parse_and_store_udp(buf,
			  username,
			  &user_found,
			  &balance,
			  sender,
			  &sender_balance,
			  &sender_found,
			  receiver,
			  &receiver_balance,
			  &receiver_found,
			  &max_transaction_index,
			  client_request_type);

      printf("Sender: %s\n", sender);
      printf("Sender Balance: %d\n", sender_balance);
      printf("Sender Found: %d\n", sender_found);
      printf("Receiver: %s\n", receiver);
      printf("Receiver Balance: %d\n", receiver_balance);
      printf("Receiver Found: %d\n", receiver_found);
      printf("Max Transaction Index %d\n", max_transaction_index);
      // Check if the append flag is set
      if(append_transaction){
	state = (num_bytes_recv != -1 ? MAINTOCA : state);
      }
      else{
	state = (num_bytes_recv != -1 ? MAINTOSC : state);
      }
      break;
    case MAINTOSB: // Main to Server B
      printf("Case - Main to Server B\n");
      break;
    case MAINTOSC: // Main to Server C
      printf("Case - Main to Server C\n");

      // Check if the transaciton is valid
      if(valid_transaction(sender,
			   sender_balance,
			   sender_found,
			   receiver,
			   receiver_balance,
			   receiver_found,
			   transfer_amount)){
	// Get the random server to add to
	//int random_server = 0;

	printf("Valid Transaction switch to server A to append");
	// Switch to the random server	
	state = MAINTOSA;

	// Set the append transaction to true to write to random server
	append_transaction = 1;
      }
      else{
	// Switch back to client
	state = MAINTOCA;
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
  
  // Free memory
  free(buf);
  free(username);
  free(sender);
  free(receiver);

  // Return from main
  return 0;
}
