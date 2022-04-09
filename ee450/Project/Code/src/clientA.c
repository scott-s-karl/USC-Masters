// Steven Karl
// Client A
// --------------

// Includes
#include "../header/clientA.h"

// Macros
#define CLIENTPORT "4141" // Client Port
#define MAINPORT "25711" // Main Server TCP Port
#define CLIENTBUFLEN 2048
#define MSGLEN 2048

// Local Functions
void
check_number_of_args(int argc){
  if (argc < 2){
    fprintf(stderr, "The client A program takes input!\n");
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
      perror("Client A: Socket creation failed");
      continue;
    }
    
    // Attempt to connect
    connect_status = connect(*sock_fd,
			     cnntn->ai_addr,
			     cnntn->ai_addrlen);
    
    // Check the return code
    if(connect_status == -1){
      close(*sock_fd);
      perror("Client A: Connection through socket failed");
      continue;
    }
    break;
  }

  // Check if at the end of possible connection without valid
  if(cnntn == NULL){
    fprintf(stderr, "Client A: Failed to connect\n");
    exit(2);
  }
}

void
send_tcp_msg(int sock_fd,
	     int msg_len,
	     char *msg){
  // Send and Check for errors
  if(send(sock_fd, msg, msg_len, 0) == -1){
    perror("Failed to send initial client message to Main Server!\n");
    close(sock_fd);
    exit(1);
  }
}

void
recv_tcp_msg(char *buf,
	     int buf_len,
	     int sock_fd){
  // Make sure your buf is empty
  memset(buf, 0, buf_len * sizeof(*buf));
  int num_bytes = recv(sock_fd, buf, buf_len, 0);
  if(num_bytes == -1){
    perror("Error: receiving client data\n");
    exit(1);
  }
  // Make a cstring by adding nullbyte
  buf[num_bytes] = '\0';
}

int
is_balance_error(char *buf){
  return 0; 
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
  printf("The client A is up and running.\n");

  // Define buffer for sending and receiving
  char *msg = (char *)calloc(MSGLEN, sizeof(*msg));
  char *client_buf = (char *)calloc(CLIENTBUFLEN, sizeof *client_buf);

  // Switch based on arguments given
  switch(argc){
  case 2: // Check Wallet ./clientA Martin
    printf("Action - CHECK WALLET\n");

    // Send Data
    sprintf(msg, "%s", argv[1]);
    send_tcp_msg(client_sock_fd,
		MSGLEN,
		msg);
    printf("%s sent a balance enquiry request to the main server\n", msg);

    // Receive Data  
    recv_tcp_msg(client_buf,
		 CLIENTBUFLEN,
		 client_sock_fd);

    // Print result - (Error/Success)
    if(is_balance_error(client_buf)){
      printf("Bad Username: %s. User not found in database.\n", argv[1]);
    }
    printf("The current balance of %s is: xxx alicoins.\n", argv[1]);
    break;
    
  case 4: // TXCOINS ./clientA Martin Luke 100 
    printf("Action - TXCOINS\n");
    printf("%s has requested to transfer %s coins to %s\n", argv[1],argv[3],argv[2]);  
    break;
    
  default: // All other argc values error out
    fprintf(stderr, "Invlaid number of arguments\n");
    break;
    
  }// End Switch
  
  // Close descriptor
  close(client_sock_fd);

  // Free memory
  free(msg);
  free(client_buf);

  // Return from main
  return 0;
}
