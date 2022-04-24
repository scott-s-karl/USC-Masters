// Steven Karl
// Backend Server B
// ----------------

// Includes
#include "../header/serverB.h"
#include "stdio.h"

// Macros
#define SRVRBPORT "22711" // Port # ServerB runs on
#define BUFLEN 2048

// Local Functions
void
verify_input_count(int argc){
  if(argc > 1){
    fprintf(stderr, "The Backend Server B program doesn't take input!\n");
    exit(1);
  }
}

void
socket_setup(struct addrinfo *socket_prefs){
  memset(socket_prefs, 0, sizeof *socket_prefs);
  socket_prefs->ai_family = AF_UNSPEC; // IPv4 or IPv6
  socket_prefs->ai_socktype = SOCK_DGRAM; // UDP
  socket_prefs->ai_flags = AI_PASSIVE; // Use my IP
}

void
getaddrinfo_error(int ret_val){
  if(ret_val == -1){
    fprintf(stderr, "Error: Backend Server B: %s\n", gai_strerror(ret_val));
    exit(1);
  }
}

void
bind_to_available_socket(int *sock_fd,
	    struct addrinfo *cxns){
  // Local Variables
  int bind_ret_val;
  struct addrinfo *cxn;
  
  // Loop through linked list of connections
  for(cxn = cxns;
      cxn != NULL;
      cxn = cxn->ai_next){
    // Attempt to call socket
    *sock_fd = socket(cxn->ai_family,
		      cxn->ai_socktype,
		      cxn->ai_protocol);

    // Check return code
    if(*sock_fd == -1){
      perror("Error: Socket creation failed\n");
      continue;
    }
    
    // Attempt to bind
    bind_ret_val = bind(*sock_fd,
			cxn->ai_addr,
			cxn->ai_addrlen);

    // Check bind return value
    if(bind_ret_val == -1){
      close(*sock_fd);
      perror("Error: Bind failed\n");
      continue;
    }
    break;
  }
  // Check if we made it to a NULL value/list empty/
  if(cxn == NULL){
    fprintf(stderr, "Error: Couldn't find available socket\n");
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

  printf("The ServerB received a request from the Main Server.\n");
  
  // Check the return code
  if(num_bytes == -1){
    perror("Error: Failed to receive from Main Server\n");
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
bt_request_send_to_main(int sock_fd,
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
    printf("Error: Invalid request type\n");
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
    perror("Error: Failed to send to Main Server.\n");
    exit(1);
  }
}

void
open_transaction_file(FILE **fin){

  *fin = fopen("server_files/serverB/block2.txt", "r+");
  if(*fin == NULL){
    perror("Error: Could not open Server B file.\n");
    exit(1);
  }
}

void
close_transaction_file(FILE **fin){
  int ret_val = fclose(*fin);
  if(ret_val != 0){
    perror("Error: Could not close Server B file.\n");
    exit(1);
  }
}

void
trim_return_char(char *buf){
  // Loop through buf
  for(int i = 0; buf[i] != '\0'; i++){
    // Find newline char
    if(buf[i] == '\n'){
      // Replace it with null byte
      buf[i] ='\0';
    }
  }
}


char *
read_line(char *buf,
	  FILE **fin,
	  int buf_len){
  // Get a line
  char *line = fgets(buf, buf_len, *fin);

  // Check the return value
  if(line == NULL){
    perror("Error: Unable to read line\n");
  }
  if(ferror(*fin)){
    perror("ferror was thrown\n");
  }

  // Remove the return character
  trim_return_char(buf);

  // Return read line status
  return line;
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
  while(read_line(buf, fin, BUFLEN)){
    // Split line
    sscanf(buf,"%d %s %s %d", &idx, sender, receiver, &transfer_amount);

    // Check for max transaction index
    if(*max_transaction_index < idx){
      *max_transaction_index = idx;
    }
    // Check if username matches sender or receiver
    if(strncmp(username, sender, BUFLEN) == 0){
      // User is the sender
      *balance = *balance - transfer_amount;
      *user_found = 1;
    }
    else if(strncmp(username, receiver, BUFLEN) == 0){
      // User is the receiver
      *balance = *balance + transfer_amount;
      *user_found = 1;
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
  if(client_request_type == 1){
    get_user_balance(username,
		     user_found,
		     balance,
		     max_transaction_index,
		     fin);
    // Reset after lookup
    reset_server_file(fin);
  }
  else if(client_request_type == 2){
    // Doing sender lookup
    get_user_balance(sender,
		     sender_found,
		     sender_balance,
		     max_transaction_index,
		     fin);
    
    // Reset file
    reset_server_file(fin);
    
    // Doing receiver lookup
    get_user_balance(receiver,
		     receiver_found,
		     receiver_balance,
		     max_transaction_index,
		     fin);
    // Reset file
    reset_server_file(fin);
    
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
reset_server_file(FILE **fin){
  if(0 != fseek(*fin, 0L, SEEK_SET)){
    perror("ERROR: Problem reseting Server A file.\n");
    exit(1);
  }
}

void
append_transaction_to_server_file(FILE **fin,
				  int max_transaction_index,
				  char *sender,
				  char *receiver,
				  int transfer_amount){

  // Get the cursor to the end of the file
  if (0 != fseek(*fin, 0L, SEEK_END)){
    perror("ERROR: Problem appending to the server A file\n");
    exit(1);
  }

  // Write the new transaction
  fprintf(*fin,
	  "\n%d %s %s %d",
	  (max_transaction_index + 1),
	  sender,
	  receiver,
	  transfer_amount);

  // reset the cursor to the start of the file
  reset_server_file(fin);
}

void
gather_and_send_transactions(FILE **fin,
			     int sock_fd,
			     struct sockaddr_storage *addr,
			     socklen_t addr_len){
  // To hold each line
  char *buf = (char *)calloc(BUFLEN, sizeof(*buf));
  
  // Loop through the lines
  while(read_line(buf, fin, BUFLEN)){
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

  // reset the file
  reset_server_file(fin);

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
  verify_input_count(argc);

  // Local variables
  int svr_b_socket_fd;
  int gai_ret_val;
  struct addrinfo svr_b_socket_prefs;
  struct addrinfo *svr_b_cxns;
  struct sockaddr_storage main_addr;
  socklen_t main_addr_len;
  
  // Setup cxn
  socket_setup(&svr_b_socket_prefs);
  gai_ret_val = getaddrinfo(NULL,
			    SRVRBPORT,
			    &svr_b_socket_prefs,
			    &svr_b_cxns);
  getaddrinfo_error(gai_ret_val);

  // Create and bind socket for cxn
  bind_to_available_socket(&svr_b_socket_fd,
			   svr_b_cxns);
  freeaddrinfo(svr_b_cxns);

  // Start Message
  printf("The Server B is up and running using UDP on port %s\n", SRVRBPORT);

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
  
  // Main Loop that the backend server sits in
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
				svr_b_socket_fd);

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
      gather_and_send_transactions(&fin,
				   svr_b_socket_fd,
				   &main_addr,
				   main_addr_len);
    }
    else{
      // Check if we are appending or looking up
      if(append_transaction){
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
      // Send udp msg back to main
      bt_request_send_to_main(svr_b_socket_fd,
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
    
    printf("The ServerB finished sending the response to the Main Server.\n");
    free(username);
    free(sender);
    free(receiver);
    free(buf);
    close_transaction_file(&fin);
  }// End While
}
