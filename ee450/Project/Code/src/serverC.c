// Steven Karl
// Backend Server C
// ----------------

// Includes
#include "../header/serverC.h"
#include "stdio.h"

// Macros
#define SRVRCPORT "23711" // Port # ServerC runs on

// Local Functions
void
check_number_of_args(int argc){
  if(argc > 1){
    fprintf(stderr, "The Backend Server C program doesn't take input!\n");
    exit(1);
  }
}

void
set_sock_preferences(struct addrinfo *sock_preferences){
  memset(sock_preferences, 0, sizeof *sock_preferences);
  sock_preferences->ai_family = AF_UNSPEC; // IPv4 or IPv6
  sock_preferences->ai_socktype = SOCK_DGRAM; // UDP
  sock_preferences->ai_flags = AI_PASSIVE; // Use my IP
}

void
check_if_getaddrinfo_failed(int getaddrinfo_result){
  if(getaddrinfo_result == -1){
    fprintf(stderr, "getaddrinfo: Backend Server C: %s\n", gai_strerror(getaddrinfo_result));
    exit(1);
  }
}

void
create_sock_and_bind(int *sock_fd, struct addrinfo *poss_cnntns){
  // Local Variables
  int bind_status;
  struct addrinfo *cnntn;
  
  // Loop through linked list of connections
  for(cnntn = poss_cnntns;
      cnntn != NULL;
      cnntn = cnntn->ai_next){
    // Attempt to call socket
    *sock_fd = socket(cnntn->ai_family,
		      cnntn->ai_socktype,
		      cnntn->ai_protocol);

    // Check return code
    if(*sock_fd == -1){
      perror("Server C: Socket creation failed");
      continue;
    }
    
    // Attempt to bind
    bind_status = bind(*sock_fd,
		       cnntn->ai_addr,
		       cnntn->ai_addrlen);

    // Check bind status
    if(bind_status == -1){
      close(*sock_fd);
      perror("Server C: Connection through socket failed");
      continue;
    }
    break;
  }
  // Check if we made it to a NULL value/list empty/
  if(cnntn == NULL){
    fprintf(stderr, "Server C: Failed to connect.");
    exit(2);
  }
}

// Main function
int main(int argc, const char *argv[]){

  // Check the input arguments
  check_number_of_args(argc);

  // Local variables
  int svr_c_sock_fd;
  int getaddrinfo_result;

  struct addrinfo svr_c_sock_prefs;
  struct addrinfo *svr_c_poss_cnntns;
  struct sockaddr_storage main_addr;
  socklen_t main_addr_len;
  
  // Setup cnntn
  set_sock_preferences(&svr_c_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   SRVRCPORT,
				   &svr_c_sock_prefs,
				   &svr_c_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result);

  // Create and bind socket for cnntn
  create_sock_and_bind(&svr_c_sock_fd, svr_c_poss_cnntns);
  freeaddrinfo(svr_c_poss_cnntns);

  // Start Messages
  printf("The Server C is up and running using UDP on port %s\n", SRVRCPORT);

  while(1){
    sleep(10);
    printf("...waiting...\n");
  }
}
