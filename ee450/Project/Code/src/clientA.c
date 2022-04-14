// Steven Karl
// Client A
// --------------

// Includes
#include "../header/clientA.h"

// Macros
#define CLIENTPORT "4141" // Client Port
#define MAINPORT "25711" // Main Server TCP Port
#define BUFLEN 2048
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

void
parse_tcp_msg(char *username,
	      int *user_found,
	      int *balance,,
	      int *user_found,
	      int *balance,
	      char *sender,,
	      int *sender_balance,
	      int *sender_found,
	      char *receiver,
	      int *receiver_balance,
	      int *receiver_found,
	      char *client_buf,
	      int request_type){
  // Check request type
  if(request_type == 2){
    sscanf(client_buf,
	   "%s %d %d",
	   username,
	   balance,
	   user_found);
  }
  else if(request_type == 4){
    sscanf(client_buf,
	   "%s %d %d %s %d %d",
	   username,
	   balance,
	   user_found);
  }
  else{
    perror("Error: Invalid request type\n");
    exit(1);
  }
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
  char *client_buf = (char *)calloc(BUFLEN, sizeof *client_buf);
  char *username = (char *)calloc(BUFLEN, sizeof *username);
  char *sender = (char *)calloc(BUFLEN, sizeof *sender);
  char *receiver = (char *)calloc(BUFLEN, sizeof *receiver);
  int balance;
  int user_found;
  int transfer_amount;
  
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
		 BUFLEN,
		 client_sock_fd);

    // Parse Received Data
    printf("Parsing Received Message\n");
    printf("Buf returned to client: %s\n", client_buf);
    parse_tcp_msg(username,
		  &user_found,
		  &balance,
		  sender,
		  receiver,
		  &transfer_amount,
		  client_buf,
		  argc);

    printf("Username: %s\n", username);
    printf("Balance: %d\n", balance);
    printf("User Found %d\n", user_found);
    
    // Print result - (Error/Success)
    if(!user_found){
      printf("Bad Username: %s. User not found in database.\n", argv[1]);
    }
    else{
      printf("The current balance of %s is: %d alicoins.\n",
	     username,
	     balance);
    }
    
    break;
    
  case 4: // TXCOINS ./clientA Martin Luke 100 
    // Case Print Statements
    printf("Action - TXCOINS\n");
    printf("%s has requested to transfer %s coins to %s\n",
	   argv[1],
	   argv[3],
	   argv[2]);

    // Prepare message
    snprintf(msg,
	     MSGLEN,
	     "%s %s %s",
	     argv[1],
	     argv[2],
	     argv[3]);

    // Send Message
    send_tcp_msg(client_sock_fd,
		 MSGLEN,
		 msg);
     
    // Receive Response
    recv_tcp_msg(client_buf,
		 BUFLEN,
		 client_sock_fd);

    // Parse Response
    printf("Buf returned to client: %s\n", client_buf);
    parse_tcp_msg(username,
		  &user_found,
		  &balance,
		  sender,
		  sender_balance,
		  sender_found,
		  receiver,
		  receiver_balance,
		  receiver_found,
		  client_buf,
		  argc);

    // Check return values
    if(!sender_found || !receiver_found){
      
    }
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
  free(username);
  free(sender);
  free(receiver);
  // Return from main
  return 0;
}
