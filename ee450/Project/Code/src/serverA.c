// Steven Karl
// Backend Server A
// -----------------

// Includes
#include "../header/serverA.h"
#include "stdio.h"

// Macros
#define SRVRAPORT "21711" // Port # ServerA runs on
#define BUFLEN 2048
#define NAMELEN 2048

// Local Functions
void
check_number_of_args(int argc){
  if(argc > 1){
    fprintf(stderr, "The Backend Server A program doesn't take input!\n");
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
    fprintf(stderr, "getaddrinfo: Backend Server A: %s\n", gai_strerror(getaddrinfo_result));
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
      perror("Server A: Socket creation failed");
      continue;
    }
    
    // Attempt to bind
    bind_status = bind(*sock_fd,
		       cnntn->ai_addr,
		       cnntn->ai_addrlen);

    // Check bind status
    if(bind_status == -1){
      close(*sock_fd);
      perror("Server A: Connection through socket failed");
      continue;
    }
    break;
  }
  // Check if we made it to a NULL value/list empty/
  if(cnntn == NULL){
    fprintf(stderr, "Server A: Failed to connect.");
    exit(2);
  }
}

void
receive_and_store_from_main(char *buf,
			    socklen_t *addr_len,
			    struct sockaddr_storage *addr,
			    int sock_fd
			    ){
  // Attempt to receive bytes from main
  int num_bytes = recvfrom(sock_fd,
			   buf,
			   BUFLEN,
			   MSG_WAITALL,
			   (struct sockaddr *)addr,
			   addr_len);

  printf("The ServerA received a request from the Main Server.\n");
  
  // Check the return code
  if(num_bytes == -1){
    perror("Error: Failed to receive bytes from main\n");
    exit(1);
  }

  // Add a null byte to the received message to make cstring
  buf[num_bytes] = '\0';
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
parse_udp_msg(int *client_request_type,
	      char *username,
	      char *sender,
	      char *receiver,
	      int *transfer_amount,
	      char *buf){
  
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
    perror("Error: Bad Arguments\n");
    exit(1);
  }
}

void
prep_and_send_udp_data(int sock_fd,
		       char *buf,
		       int client_request_type,
		       char *username,
		       char *sender,
		       char *receiver,
		       int transfer_amount,
		       struct sockaddr_storage *addr,
		       socklen_t addr_len){
  // Pack based on request
  if(client_request_type == 1){
    printf("Balance Request\n");
    snprintf(buf, 1024, "%s", username);
  }
  else if(client_request_type == 2){
    printf("Transfer Request\n");
  }
  else{
    printf("Error: Invalid request type, gather and send udp\n");
    exit(1);
  }

  // Send msg over udp
  printf("Sending udp message to Main Server\n");
  int num_bytes = sendto(sock_fd,
			 buf,
			 BUFLEN,
			 0,
			 (struct sockaddr *)addr,
			 addr_len);
  // Check return
  if(num_bytes == -1){
    perror("Error: Failed to send bytes to Main Server\n");
    exit(1);
  }
}

void
able_to_read_lines(char *buf,
		   FILE *file,
		   int buf_len){
  // Get a line
  char *ret_val = fgets(buf, buf_len, file);

  // Check the return value
  if(ret_val == NULL && 0 != ferror(file)){
    perror("Error: Unable to read line\n");
  }
  return ret_val;
}


void
get_user_balance(char *username,
		 int *balance){
  // Open the file for server A
  FILE *server_file;
  server_file = fopen("../servers/block1.txt", "r");
  if(server_file == NULL){
    perror("Error: Could not open Server A file\n");
    exit(1);
  }
  // Variables to save data in
  char *sender = (char *)calloc(BUFLEN, sizeof(*sender));
  char *receiver = (char *)calloc(BUFLEN, sizeof(*sender));
  char *buf = (char *)calloc(BUFLEN, sizeof(*sender));
  int transfer_amount;
  
  
  // Loop through the lines in the file
  while(able_to_read_line(buf, server_file, BUFLEN)){
    // Split line
    sscanf(buf,"%s %s %d", sender, receiver, transfer_amount);

    // Check if username matches sender or receiver
    if(strncmp(username, sender, BUFLEN) == 1){
      printf("User is the sender\n");
      balance = balance - transfer_amount;
    }
    else if(strncmp(username, receiver, BUFLEN) == 1){
      printf("User is the receiver\n");
      balance = balance + transfer_amount;
    }
    else{
      printf("User not involved in this transaction\n");
    }
    // Clear buf for next line
    memset(buf, 0, sizeof(BUFLEN));
  }
  // Free Memory
  free(sender);
  free(receiver);
  free(buf);
}

void
lookup_server_data(int client_request_type,
		   char *username,
		   int *balance,
		   char *sender,
		   char *receiver,
		   int *transfer_amount){
  // Switch on request type
  if(client_request_type == 1){
    get_user_balance(username,
		     balance);
  }
  else if(client_request_type == 2){
    printf("Transaction Request\n")
  }
  eles{
    perror("Invalid request type\n");
    exit(1);
  }
}

// Main function
int main(int argc, const char *argv[]){

  // Check the input arguments
  check_number_of_args(argc);

  // Local variables
  int svr_a_sock_fd;
  int getaddrinfo_result;

  struct addrinfo svr_a_sock_prefs;
  struct addrinfo *svr_a_poss_cnntns;
  struct sockaddr_storage main_addr;
  socklen_t main_addr_len;
  
  // Setup cnntn
  set_sock_preferences(&svr_a_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   SRVRAPORT,
				   &svr_a_sock_prefs,
				   &svr_a_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result);

  // Create and bind socket for cnntn
  create_sock_and_bind(&svr_a_sock_fd, svr_a_poss_cnntns);
  freeaddrinfo(svr_a_poss_cnntns);

  // Start Messages
  printf("The Server A is up and running using UDP on port %s\n", SRVRAPORT);

  // Define while loop storage variables
  char *buf = (char *)calloc(BUFLEN, sizeof(*buf));
  int client_request_type;
  char *username = (char *)calloc(NAMELEN, sizeof(*username));
  char *sender = (char *)calloc(NAMELEN, sizeof(*sender));
  char *receiver = (char *)calloc(NAMELEN, sizeof(*receiver));
  int transfer_amount;
  
  while(1){
    // Reset buffer
    memset(buf, 0, BUFLEN*sizeof(*buf));
    
    // Try to receive from main
    printf("Waiting for udp message from main\n");
    receive_and_store_from_main(buf,
				&main_addr_len,
				&main_addr,
				svr_a_sock_fd);
    printf("Received udp message from main\n");  
    printf("Buffer: %s\n", buf);
    
    // Parse msg from main
    printf("Parsing received message from main\n");
    parse_udp_msg(&client_request_type,
		  username,
		  sender,
		  receiver,
		  &transfer_amount,
		  buf);
    printf("Username: %s\n", username);

    // Do a lookup in my file and collect data
    lookup_server_data(client_request_type,
		       username,
		       sender,
		       receiver,
		       &transfer_amount);
    
    // Send udp msg back to main
    prep_and_send_udp_data(svr_a_sock_fd,
			   buf,
			   client_request_type,
			   username,
			   sender,
			   receiver,
			   transfer_amount,
			   &main_addr,
			   main_addr_len);
    
    printf("The ServerA finished sending the response to the Main Server.\n");
  }
}
