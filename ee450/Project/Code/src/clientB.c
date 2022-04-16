// Steven Karl
// Client B
// --------------

// Includes
#include "../header/clientB.h"

// Macros
#define CLIENTPORT "4141" // Client Port
#define MAINPORT "26711" // Main Server TCP Port
#define BUFLEN 2048
#define MSGLEN 2048

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
	      int *balance,
	      int *user_found,
	      char *sender,
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
	   sender,
	   sender_found,
	   sender_balance,
	   receiver,
	   receiver_found,
	   receiver_balance);
  }
  else{
    perror("Error: Invalid request type\n");
    exit(1);
  }
}


void
check_transfer_return_values(char *sender,
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
    printf("The current balance of %s is : %d alicoins.\n",
	   sender,
	   sender_balance);
  }

  // Check if sender or receiver isn't found
  if(!sender_found && !receiver_found){
    printf("Unable to proceed with the transaction as %s and %s are not part of the network.\n",
	   sender,
	   receiver);
  }
  else if(!sender_found){
    printf("Unable to proceed with the transaction as %s is not part of the network.\n", sender);
  }
  else if(!receiver_found){
    printf("Unable to proceed with the transaction as %s is not part of the network.\n", receiver);
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

    // Switch based on arguments given
  switch(argc){
  case 2: // Check Wallet ./clientA Martin
    printf("Action - CHECK WALLET\n");
    printf("User: %s\n", argv[1]);
    break;
  case 4: // TXCOINS ./clientA Martin Luke 100 
    printf("Action - TXCOINS\n");
    printf("Sender: %s\n", argv[1]);
    printf("Receiver: %s\n", argv[2]);
    printf("Amount: %s\n", argv[3]);
    break;
  default: // All other argc values error out
    fprintf(stderr, "Invlaid number of arguments\n");
    break;
  }
}
