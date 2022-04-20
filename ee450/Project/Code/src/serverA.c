// Steven Karl
// Backend Server A
// -----------------

// Includes
#include "../header/serverA.h"

// Macros
#define SRVRAPORT "21711" // Port # ServerA runs on
#define BUFLEN 2048

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
  if(spaces == 1){
    *client_request_type = 1;
  }
  else if(spaces == 6){
    *client_request_type = 2;
  }
  else if(spaces == 0){
    printf("Type 3 Request - get type\n");
    *client_request_type = 3;
  }
  else{
    printf("Bad Reqeust\n");
    *client_request_type = 0;
  }
}

void
parse_udp_msg(int *client_request_type,
	      char *username,
	      int *balance,
	      char *sender,
	      int *sender_balance,
	      char *receiver,
	      int *receiver_balance,
	      int *transfer_amount,
	      int *max_transaction_index,
	      int *append_transaction,
	      char *buf){
  
  // Get the request type
  get_request_type(buf,
		   client_request_type);

  // Parse based on type
  if(*client_request_type == 1){
    sscanf(buf, "%s %d", username, balance);

  }
  else if(*client_request_type == 2){
    sscanf(buf,
	   "%s %d %s %d %d %d %d",
	   sender,
	   sender_balance,
	   receiver,
	   receiver_balance,
	   transfer_amount,
	   max_transaction_index,
	   append_transaction);
  }
  else if(*client_request_type == 3){
    return;
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
		       int balance,
		       int user_found,
		       char *sender,
		       int sender_balance,
		       int sender_found,
		       char *receiver,
		       int receiver_balance,
		       int receiver_found,
		       int max_transaction_index,
		       struct sockaddr_storage *addr,
		       socklen_t addr_len){
  // Pack based on request
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
    printf("Error: Invalid request type, gather and send udp\n");
    exit(1);
  }

  // Send msg over udp
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
open_transaction_file(FILE **fin){

  *fin = fopen("servers/block1.txt", "r+");
  if(*fin == NULL){
    perror("Error: Could not open Server A file\n");
    exit(1);
  }
}

void
close_transaction_file(FILE **fin){
  int ret_val = fclose(*fin);
  if(ret_val != 0){
    perror("Error: Could not close file properly\n");
    exit(1);
  }
}

void
trim_return_char(char *buf){
  // Loop through buf
  for(int i = 0; buf[i] != '\0'; i++){
    if(buf[i] == '\n'){
      buf[i] ='\0';
    }
  }
}

char *
able_to_read_lines(char *buf,
		   FILE **fin,
		   int buf_len){
  // Get a line
  char *ret_val = fgets(buf, buf_len, *fin);

  // Check the return value
  if( ret_val == NULL){
    perror("Error: Line was null\n");
  }
  if ( ferror(*fin) ){
    perror("ferror was thrown\n");
  }

  // Remove the return character
  trim_return_char(buf);
  
  printf("Ret val: %s\n", ret_val);
  return ret_val;
}

void
get_user_balance(char *username,
		 int *user_found,
		 int *balance,
		 int *max_transaction_index,
		 FILE **fin){
  
  // Variables to save temp data for each line
  char *sender = (char *)calloc(BUFLEN, sizeof(*sender));
  char *receiver = (char *)calloc(BUFLEN, sizeof(*sender));
  char *buf = (char *)calloc(BUFLEN, sizeof(*sender));
  int transfer_amount;
  int idx;
    
  // Loop through the lines in the file
  while(able_to_read_lines(buf, fin, BUFLEN)){
    // Split line
    sscanf(buf,"%d %s %s %d", &idx, sender, receiver, &transfer_amount);
    printf("Line Read %s\n", buf);

    // Check for max transaction index
    if(*max_transaction_index < idx){
      *max_transaction_index = idx;
    }
    
    // Check if username matches sender or receiver
    if(strncmp(username, sender, BUFLEN) == 0){
      printf("User is the sender\n");
      printf("User Current Balance: %d\n", *balance);
      printf("Transfer Amount: %d\n", transfer_amount);
      *balance = *balance - transfer_amount;
      *user_found = 1;
      printf("New Balance: %d\n", *balance);
    }
    else if(strncmp(username, receiver, BUFLEN) == 0){
      printf("User is the receiver\n");
      printf("User Current Balance: %d\n", *balance);
      printf("Transfer Amount: %d\n", transfer_amount);
      *balance = *balance + transfer_amount;
      *user_found = 1;
      printf("New Balance: %d\n", *balance);
    }
    else{
      printf("User not involved in this transaction\n");
    }
    // Clear buf for next line
    memset(buf, 0, BUFLEN);
    memset(sender, 0, BUFLEN);
    memset(receiver, 0, BUFLEN);
  }
  
  // Free Memory
  free(sender);
  free(receiver);
  free(buf);
}

void
lookup_server_data(FILE **fin,
		   int *balance,
		   int *user_found,
		   int client_request_type,
		   char *username,
		   char *sender,
		   int *sender_balance,
		   int *sender_found,
		   char *receiver,
		   int *receiver_balance,
		   int *receiver_found,
		   int *max_transaction_index){

  // Switch on request type
  printf("Looking up server A data checking request type.\n");
  if(client_request_type == 1){
    printf("Type 1 request\n");
    get_user_balance(username,
		     user_found,
		     balance,
		     max_transaction_index,
		     fin);
    // Rewind after lookup
    rewind_server_file(fin);
  }
  else if(client_request_type == 2){
    printf("Type 2 request\n");
    printf("Doing sender lookup\n");
    get_user_balance(sender,
		     sender_found,
		     sender_balance,
		     max_transaction_index,
		     fin);
    
    // Rewind file
    rewind_server_file(fin);
    
    printf("Doing receiver lookup\n");
    get_user_balance(receiver,
		     receiver_found,
		     receiver_balance,
		     max_transaction_index,
		     fin);
    // Rewind file
    rewind_server_file(fin);
    
  }
  else{
    perror("Invalid request type\n");
    exit(1);
  }
}

void
clear_session_variables(char *buf,
			int *client_request_type,
			int *max_transaction_index,
			int *transfer_amount,
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

  // Reset all pointers
  memset(buf, 0, BUFLEN*sizeof(*buf));
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

void
rewind_server_file(FILE **fin){
  if(0 != fseek(*fin, 0L, SEEK_SET)){
    perror("ERROR: Problem rewinding Server A file.\n");
    exit(1);
  }
}

void
append_transaction_to_server_file(FILE **fin,
				  int max_transaction_index,
				  char *sender,
				  char *receiver,
				  int transfer_amount){
  printf("Moving Cursor to end of file\n");
  // Get the cursor to the end of the file
  if (0 != fseek(*fin, 0L, SEEK_END)){
    perror("ERROR: Problem appending to the server A file\n");
    exit(1);
  }
  printf("Appending transaction to file\n");
  // Write the new transaction
  fprintf(*fin,
	  "\n%d %s %s %d",
	  (max_transaction_index + 1),
	  sender,
	  receiver,
	  transfer_amount);

  // rewind the cursor to the start of the file
  rewind_server_file(fin);
}

void
gather_and_send_transactions(FILE **fin,
			     int sock_fd,
			     struct sockaddr_storage *addr,
			     socklen_t addr_len){
  // To hold each line
  char *buf = (char *)calloc(BUFLEN, sizeof(*buf));
  
  // Loop through the lines
  while(able_to_read_lines(buf, fin, BUFLEN)){
    printf("Buffer is: %s\n", buf);
    // Send the line to the main server
    send_simple_udp(sock_fd,
		    buf,
		    (struct sockaddr *)addr,
		    addr_len);
    // clear the buf
    memset(buf,0,BUFLEN);
  }
  // Send final message
  strncpy(buf, "Done", BUFLEN);
  send_simple_udp(sock_fd,
		  buf,
		  (struct sockaddr *)addr,
		  addr_len);

  // rewind the file
  rewind_server_file(fin);

  // Free mem
  free(buf);
}

int
send_simple_udp(int sock_fd,
		char *buf,
		struct sockaddr *addr,
		socklen_t addr_len){
  // Send the buffer
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
  // Return bytes sent
  return num_bytes;
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
 
  int client_request_type = 999;
  int balance = 1000;
  int user_found = 0;
  int sender_found = 0;
  int sender_balance = 1000;
  int receiver_found = 0;
  int receiver_balance = 1000;
  int max_transaction_index = 0;
  int transfer_amount = 0;
  int append_transaction = 0;
  
  while(1){
    FILE *fin;
    open_transaction_file(&fin);

    char *buf = (char *)calloc(BUFLEN, sizeof(*buf));
    char *username = (char *)calloc(BUFLEN, sizeof(*username));
    char *sender = (char *)calloc(BUFLEN, sizeof(*sender));
    char *receiver = (char *)calloc(BUFLEN, sizeof(*receiver));
    
    // Reset Local variables
    clear_session_variables(buf,
			    &client_request_type,
			    &max_transaction_index,
			    &transfer_amount,
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
    
    // Try to receive from main
    printf("Waiting for connection from main server\n");
    receive_and_store_from_main(buf,
				&main_addr_len,
				&main_addr,
				svr_a_sock_fd);

    printf("Buffer from Main: %s\n", buf);
    // Parse msg from main
    parse_udp_msg(&client_request_type,
		  username,
		  &balance,
		  sender,
		  &sender_balance,
		  receiver,
		  &receiver_balance,
		  &transfer_amount,
		  &max_transaction_index,
		  &append_transaction,
		  buf);
    if(client_request_type == 3){
      // Send all transactions
      printf("Gathering transactions and sending\n");
      gather_and_send_transactions(&fin,
				   svr_a_sock_fd,
				   &main_addr,
				   main_addr_len);
    }
    else{
      // Check if we are appending or looking up
      if(append_transaction){
	printf("Appending transaction to server A\n");
	append_transaction_to_server_file(&fin,
					  max_transaction_index,
					  sender,
					  receiver,
					  transfer_amount);
	// Update the senders balance
	sender_balance = sender_balance - transfer_amount;
	receiver_balance = receiver_balance + transfer_amount;

	// Update the status's
	sender_found = 1;
	receiver_found = 1;
      }
      else{
	// Do a lookup in my file and collect data
	printf("Looking up data\n");
	lookup_server_data(&fin,
			   &balance,
			   &user_found,
			   client_request_type,
			   username,
			   sender,
			   &sender_balance,
			   &sender_found,
			   receiver,
			   &receiver_balance,
			   &receiver_found,
			   &max_transaction_index);
      }
      // Print out variable status's
      printf("User: %s\n", username);
      printf("User Balance: %d\n", balance);
      printf("User Found: %d\n", user_found);
      printf("Sender: %s\n", sender);
      printf("Sender Balance: %d\n", sender_balance);
      printf("Sender Found: %d\n", sender_found);
      printf("Receiver: %s\n", receiver);
      printf("Receiver Balance: %d\n", receiver_balance);
      printf("Receiver Found: %d\n", receiver_found);
      printf("Max Transaction Index: %d\n", max_transaction_index);
    
      // Send udp msg back to main
      prep_and_send_udp_data(svr_a_sock_fd,
			     buf,
			     client_request_type,
			     username,
			     balance,
			     user_found,
			     sender,
			     sender_balance,
			     sender_found,
			     receiver,
			     receiver_balance,
			     receiver_found,
			     max_transaction_index,
			     &main_addr,
			     main_addr_len);
    }
    
    printf("The ServerA finished sending the response to the Main Server.\n");
    free(username);
    free(sender);
    free(receiver);
    free(buf);
    close_transaction_file(&fin);
  }// End While
}
