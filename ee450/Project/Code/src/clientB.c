// Steven Karl
// Client B
// --------------

// Includes
#include "../header/clientB.h"

// Macros
#define CLIENTPORT "4141" // Client Port
#define MAINPORT "26711" // Main Server TCP Port
#define BUFLEN 2048

// Local Functions
void
verify_input_count(int argc){
  if (argc < 2){
    fprintf(stderr, "The client B program takes input!\n");
    exit(1);
  }
}

void
getaddrinfo_error(int ret_val){
  if(ret_val){
    fprintf(stderr,
	    "getaddrinfo value is %s\n", gai_strerror(ret_val));
    exit(1);
  }
}

void
socket_setup(struct addrinfo *socket_prefs){
  memset(socket_prefs, 0, sizeof(*socket_prefs)); // Empty
  socket_prefs->ai_family = AF_UNSPEC; // IPv4 or IPv6
  socket_prefs->ai_socktype = SOCK_STREAM; // UDP type
}

void
connect_to_available_socket(int *sock_fd,
			    struct addrinfo *cxns){
  // Local variables
  int connect_return_value;
  struct addrinfo *cxn;
  
  // Loop through possible connections
  for(cxn = cxns;
      cxn != NULL;
      cxn = cxn->ai_next){
    // Attempt to create a socket
    *sock_fd = socket(cxn->ai_family,
		      cxn->ai_socktype,
		      cxn->ai_protocol);
    
    // Check socket return code
    if(*sock_fd == -1){
      perror("Error: Socket creation failed");
      continue;
    }
    
    // Attempt to connect
    connect_return_value = connect(*sock_fd,
				   cxn->ai_addr,
				   cxn->ai_addrlen);
    
    // Check the return code
    if(connect_return_value == -1){
      close(*sock_fd);
      perror("Error: Failed to connect to available socket");
      continue;
    }
    break;
  }

  // Check if at the end of possible connection without valid
  if(cxn == NULL){
    fprintf(stderr, "Error: Failed to find an available socket\n");
    exit(2);
  }
}

void
send_tcp_msg(int sock_fd,
	     int msg_len,
	     char *msg){
  // Send and Check for errors
  if(send(sock_fd, msg, msg_len, 0) == -1){
    perror("Error: Failed to send message to Main Server\n");
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
    perror("Error: Faile to receive from Main Server\n");
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
	      int request_type,
	      int *valid_transaction){
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
	   "%s %d %d %s %d %d %d",
	   sender,
	   sender_found,
	   sender_balance,
	   receiver,
	   receiver_found,
	   receiver_balance,
	   valid_transaction);
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
			     int transfer_amount,
			     int valid_transaction){

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

  // Check if sender doesn't have funds
  if(!valid_transaction){
    printf("%s was unable to transfer %d alicoins to %s because of insufficient balance.\n",
	   sender,
	   transfer_amount,
	   receiver);
    printf("The current balance of %s is: %d alicoins.\n",
	   sender,
	   sender_balance);
  }
}

int
get_request_type(int argc, const char* argv[]){

  // Check if the arg is TXLIST
  if(strncmp(argv[1], "TXLIST", 256) == 0){
      return 3;
  }
  // Check the count
  if(argc == 2){
    return 1;
  }
  else if(argc == 4){
    return 2;
  }
  else{
    return 999;
  }
}

int main(int argc, const char* argv[]){
  // Local Variables
  int gai_ret_val;
  int client_socket_fd;
  struct addrinfo socket_prefs;
  struct addrinfo *cxns;
  
  // Check args count
  verify_input_count(argc);
  
  // Setup client preferences
  socket_setup(&socket_prefs);
  
  // Get and check addrinfo
  gai_ret_val = getaddrinfo(NULL,
			    MAINPORT,
			    &socket_prefs,
			    &cxns);
  getaddrinfo_error(gai_ret_val);
  
  // Create socket and connect
  connect_to_available_socket(&client_socket_fd, cxns);
  freeaddrinfo(cxns);
  
  // Boot Up Message
  printf("The client B is up and running.\n");

   // Define buffer for sending and receiving
  char *msg = (char *)calloc(BUFLEN, sizeof(*msg));
  char *client_buf = (char *)calloc(BUFLEN, sizeof *client_buf);
  char *username = (char *)calloc(BUFLEN, sizeof *username);
  int user_found;
  int balance;
  char *sender = (char *)calloc(BUFLEN, sizeof *sender);
  int sender_found;
  int sender_balance;
  char *receiver = (char *)calloc(BUFLEN, sizeof *receiver);
  int receiver_found;
  int receiver_balance;
  int transfer_amount;
  int valid_transaction;
  
  // Get the type of action the user is attempting to preform
  int request_type = get_request_type(argc, argv);
  
  // Switch based on arguments given
  switch(request_type){
  case 1: // Check Wallet ./clientA Martin

    // Send Data
    sprintf(msg, "%s", argv[1]);
    send_tcp_msg(client_socket_fd,
		BUFLEN,
		msg);
    printf("%s sent a balance enquiry request to the main server\n", msg);

    // Receive Data  
    recv_tcp_msg(client_buf,
		 BUFLEN,
		 client_socket_fd);

    // Parse Received Data
    parse_tcp_msg(username,
		  &balance,
		  &user_found,
		  sender,
		  &sender_balance,
		  &sender_found,
		  receiver,
		  &receiver_balance,
		  &receiver_found,
		  client_buf,
		  argc,
		  &valid_transaction);
    
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
    
  case 2: // TXCOINS ./clientA Martin Luke 100 
    // Case Print Statements
    printf("%s has requested to transfer %s coins to %s\n",
	   argv[1],
	   argv[3],
	   argv[2]);

    // Prepare message
    transfer_amount = atoi(argv[3]);
    snprintf(msg,
	     BUFLEN,
	     "%s %s %s",
	     argv[1],
	     argv[2],
	     argv[3]);

    // Send Message
    send_tcp_msg(client_socket_fd,
		 BUFLEN,
		 msg);
     
    // Receive Response
    recv_tcp_msg(client_buf,
		 BUFLEN,
		 client_socket_fd);

    // Parse Response
    parse_tcp_msg(username,
		  &balance,
		  &user_found,
		  sender,
		  &sender_balance,
		  &sender_found,
		  receiver,
		  &receiver_balance,
		  &receiver_found,
		  client_buf,
		  argc,
		  &valid_transaction);
    
    // Check return values
    check_transfer_return_values(sender,
				 sender_balance,
				 sender_found,
				 receiver,
				 receiver_balance,
				 receiver_found,
				 transfer_amount,
				 valid_transaction);
    break;

  case 3: // TXLIST
    // Send Data
    sprintf(msg, "%s", argv[1]);
    send_tcp_msg(client_socket_fd,
		 BUFLEN,
		 msg);
    printf("TXLIST request was sent to the main server.\n");
    // Receive Data  
    recv_tcp_msg(client_buf,
		 BUFLEN,
		 client_socket_fd);
 
    // Print result - (Error/Success)
    printf("Done Getting transaction list look in main server\n");
    break;
    
  default: // All other argc values error out
    fprintf(stderr, "Invlaid number of arguments\n");
    break;
    
  }// End Switch
  
  // Close descriptor
  close(client_socket_fd);

  // Free memory
  free(msg);
  free(client_buf);
  free(username);
  free(sender);
  free(receiver);

  // Return from main
  return 0;
}

 
