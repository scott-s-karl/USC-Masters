// Steven KarlA
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
set_udp_sock_preferences(struct addrinfo *sock_prefs){
  memset(&sock_prefs, 0, sizeof(sock_prefs)); // Empty
  sock_prefs.ai_family = AF_UNSPEC; // IPv4 or IPv6
  sock_prefs.ai_socktype = SOCK_DGRAM; // UDP type
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
			 struct addrrinfo *poss_cnntns){
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

/*

*/
void
create_udp_sock_and_bind(int *sock_fd,
			 struct addrinfo *poss_cnntns){
  // Setup local variables
  int bind_status;
  struct addrinfo *cnntn;

  // Loop through the possible connections and bind to the first available
  for(cnntn = *poss_cnntns;
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
    fprinf(stderr, "mainudp: fail to connect\n");
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
reap_zombie_processes(*sa){
  int sigaction_status;
  sa->sa_handler = sigchld_handler;
  sigemptyset(&sa->sa_mask);
  sa->sa_flags = SA_RESTART;
  sigaction_status = sigaction(SIGCHLD, sa, NULL)
  if(sigaction_status  == -1){
    perror("main: sigaction failed");
    exit(1);
  }
}

// Main Function
int main(int argc const char *argv[]){

  // Check the number of arguments - should be none other then name
  check_number_of_args(argc);

  // Local variables
  int main_server_tcp_a_fd; // File descriptor tcp
  int main_server_tcp_b_fd; // File descriptor tcp
  int main_server_udp_fd; // File descriptor udp
  int getaddrinfo_result; // Function return code on error

  
  struct addrinfo tcp_a_sock_prefs; // tcp connection prefs
  struct addrinfo tcp_a_poss_cnntns;
  struct addrinfo tcp_b_sock_prefs; // tcp connection prefs
  struct addrinfo tcp_b_poss_cnntns;
  struct addrinfo udp_sock_prefs; // udp connection prefs
  struct addrinfo udp_poss_cnntns;
  
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
  get_available_socket(&sva_cnntn, &sva_poss_cnntns); // Socket() call for returned connections
  
  // Backend Server B
  struct addrinfo *svb_sock_prefs;
  struct addrinfo *svb_cnntn;
  struct addrinfo *svb_poss_cnntns;
  int svb_port;
  set_udp_sock_preferences(&svb_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   SVRBPORT,
				   &svb_sock_prefs,
				   &svb_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "svb");
  get_available_socket(&svb_cnntn, &svb_poss_cnntns);
  
  // Backend Server C
  struct addrinfo *svc_sock_prefs;
  struct addrinfo *svc_poss_cnntns;
  struct addrinfo *svc_cnntn;
  int svc_port;
  set_udp_sock_preferences(&svc_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   SVRCPORT,
				   &svc_sock_prefs,
				   &svc_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "svc");
  get_available_socket(&svc_cnntn, &svc_poss_cnntns);

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
  printf("The main server is up and running");
  printf("...waiting for connections...\n");

  // Set initial case for connection state
  int state = CTOMAIN;
  
  // Connection Loop
  while(1){ // main accept() loop 
    break;
  }
  
}
