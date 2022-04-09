// Steven KarlA
// Main Server
// ---------------

// Include Statements
#include "../header/serverM.h"
#include <stdio.h>

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
  printf("Getting the port of the client addr\n");
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
  printf("Starting to receive from client\n");
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
    printf("Bad Reqeust\n");
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
  }
  else{
    printf("Bad Arguments\n");
  }
}

// Main Function
int main(int argc, const char *argv[]){

  // Check the number of arguments - should be none other then name
  check_number_of_args(argc);

  // Local variables
  char *buf = (char *)calloc(BUFLEN, sizeof *buf);
  //int num_bytes_sent;
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
  //int sva_port; // Used later backend server side interface
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
  // int svb_port;
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
  //int svc_port;
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
  
  // Connection Loop
  while(1){ // main accept() loop
    
    // Empty out the buffer before each iteration
    memset(buf, 0, BUFLEN * sizeof(*buf));
    // Define the vars to store in when parsing requests
    int client_request_type;
    char *username = (char *)calloc(NAMELEN, sizeof(*username));
    char *sender = (char *)calloc(NAMELEN, sizeof(*sender));
    char *receiver = (char *)calloc(NAMELEN, sizeof(*receiver));
    int transfer_amount;
      
    // Switch on clientA, clientB, SVRA, SVRB, SVRC
    switch(state){
      
    case CATOMAIN: // clientA to main
      printf("Case - Client A to Main\n");

      // Accept the connection and store in fd
      client_a_addr_sz = sizeof client_a_addr;
      client_a_fd = accept(main_server_tcp_a_fd,
			   (struct sockaddr *)&client_a_addr,
			   &client_a_addr_sz);
      printf("Just after accept call\n");
      // Check fd
      if(client_a_fd == -1){
	perror("Client to Main Server: Accept Error!\n");
	continue;
      }

      // Port/ Where connection came from
      client_a_port = get_port((struct sockaddr *)&client_a_addr);
      printf("Client port: %d\n", client_a_port); 

      // From bgnet forking
      if(client_a_fd != -1){ // this is the child process
	printf("Inside the fork if statement\n");
	// Receive the msg from clientA
	num_bytes_recv = receive_client_msg(&buf,
					    BUFLEN,
					    client_a_fd);
	printf("Received Bytes: %d\n", num_bytes_recv);
	// Parse the msg and fill the variables
	parse_client_msg(&client_request_type,
			 username,
			 sender,
			 receiver,
			 &transfer_amount,
			 buf,
			 client_a_port);
	
	// Check the type of request and set the state
	if(client_request_type > 0){ // Balance query
	  printf("Balance Query\n");
	  state = MAINTOSA;
	}
	else if(client_request_type == 2){ // Transfer Query
	  printf("Tranfer Query\n");
	}
	else{ // Invalid query
	  printf("Invalid Query\n");
	}	
      }
      state = (num_bytes_recv != -1 ? ENDTRANS : state);
      break;
    case MAINTOCA: // Main to clientA
      printf("Case - Main to Client A\n");
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

      // Check the type of request to send

      // Send info from main to server A

      // Wait to receive response from server A

      // Parse and store the response from serverA

      // Switch the state to Main to server B
      
      break;
    case MAINTOSB: // Main to Server B
      printf("Case - Main to Server B\n");
      break;
    case MAINTOSC: // Main to Server C
      printf("Case - Main to Server C\n");
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
  
  // Return from main
  return 0;
}
