// Steven Karl
// Client B
// --------------

// Includes
#include "clientB.h"

// Macros
#define CLIENTPORT "4141" // Client Port
#define MAINPORT "26711" // Main Server TCP Port

// Local Functions
void
check_number_of_args(int argc){
  if (argc < 2){
    fprintf(stderr, "The client B program takes input!\n");
    exit(1);
  }
}

void
check_if_getaddrinfo_failed(int getaddrinfo_result){
  if(getaddrinfo_result){
    fprintf(stderr,
	    "getaddrinfo value is %s\n", gai_strerror(getaddrinfo_result));
    exit(1);
  }
}

void
set_sock_preferences(struct addrinfo *sock_preferences){
  memset(sock_preferences, 0, sizeof(*sock_preferences)); // Empty
  sock_preferences->ai_family = AF_UNSPEC; // IPv4 or IPv6
  sock_preferences->ai_socktype = SOCK_STREAM; // UDP type
}

void
create_sock_and_connect(int *sock_fd, struct addrinfo *poss_cnntns){
  // Local variables
  int connect_status;
  struct addrinfo *cnntn;
  
  // Loop through possible connections
  for(cnntn = poss_cnntns;
      cnntn != NULL;
      cnntn = cnntn->ai_next){
    // Attempt to create a socket
    *sock_fd = socket(cnntn->ai_family,
		      cnntn->ai_socktype,
		      cnntn->ai_protocol);
    
    // Check socket return code
    if(*sock_fd == -1){
      perror("Client B: Socket creation failed");
      continue;
    }
    
    // Attempt to connect
    connect_status = connect(*sock_fd,
			     cnntn->ai_addr,
			     cnntn->ai_addrlen);
    
    // Check the return code
    if(connect_status == -1){
      close(*sock_fd);
      perror("Client B: Connection through socket failed");
      continue;
    }
    break;
  }

  // Check if at the end of possible connection without valid
  if(cnntn == NULL){
    fprintf(stderr, "Client B: Failed to connect\n");
    exit(2);
  }
}

int main(int argc, const char* argv[]){
  // Local Variables
  int getaddrinfo_result;
  int client_sock_fd;
  struct addrinfo sock_preferences;
  struct addrinfo *poss_cnntns;
  
  // Check args count
  check_number_of_args(argc);
  
  // Setup client preferences
  set_sock_preferences(&sock_preferences);
  
  // Get and check addrinfo
  getaddrinfo_result = getaddrinfo(NULL,
				   MAINPORT,
				   &sock_preferences,
				   &poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result);
  
  // Create socket and connect
  create_sock_and_connect(&client_sock_fd, poss_cnntns);
  freeaddrinfo(poss_cnntns);
  
  // Boot Up Message
  printf("The client B is up and running.\n");
  
}
