
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#define STDIN 0
#define NAMELEN 256
#define BUFLEN 2048

void
get_request_type(char *buf,
		 int *client_request_type){
  int spaces = 0;
  for(int i = 0; buf[i] != '\0'; i++){
    if(buf[i] == ' '){
      spaces++;
    }
  }
  if(spaces == 0){
    printf("No Spaces\n");
    *client_request_type = 1;
  }
  else if(spaces == 2){
    printf("2 Spaces\n");
    *client_request_type = 2;
  }
  else{
    printf("Bad Reqeust\n");
    *client_request_type = 0;
  }
  printf("Client Request Type: %d\n", *client_request_type);
}

void
parse_client_msg(int *client_request_type,
		 char *username,
		 char *sender,
		 char *receiver,
		 int *transfer_amount,
		 char *buf){
  get_request_type(buf,
		   client_request_type); 
  if(*client_request_type == 1){
    sscanf(buf, "%s", username);
  }
  else if(*client_request_type == 2){
    sscanf(buf, "%s %s %d", sender, receiver, transfer_amount);
  }
  else{
    printf("Bad Arguments\n");
  }
}

void
open_transaction_file(FILE **fin){
  
  *fin = fopen("./../servers/block1.txt", "r");
  if(*fin == NULL){
    perror("Error: Could not open Server A file\n");
    exit(1);
  }
}

char *
able_to_read_lines(char *buf,
		   FILE **fin,
		   int buf_len){
  // Get a line
  char *ret_val = fgets(buf, buf_len, *fin);

  // Check the return value
  if(ret_val == NULL && 0 != ferror(*fin)){
    perror("Error: Unable to read line\n");
  }
  return ret_val;
}

void
get_user_balance(char *username,
		 int *balance,
		 FILE **fin){
  // Open the file for server A
  open_transaction_file(fin);
  
  // Variables to save temp data for each line
  char *sender = (char *)calloc(BUFLEN, sizeof(*sender));
  char *receiver = (char *)calloc(BUFLEN, sizeof(*sender));
  char *buf = (char *)calloc(BUFLEN, sizeof(*sender));
  int transfer_amount;
  int idx;
    
  // Loop through the lines in the file
  while(able_to_read_lines(buf, fin, BUFLEN)){
    printf("Line: %s\n", buf);
  }
}

int main(int argc, const char *argv[]){
 
  printf("Argument count: %d\n", argc);
  for(int i = 0; i < argc; i++){
    printf("Argument %d is: %s\n", i, argv[i]);
  }

  struct timeval tv;
  fd_set readfds;
  tv.tv_sec = 2;
  tv.tv_usec = 500000;
  FD_ZERO(&readfds);
  FD_SET(STDIN, &readfds);

  // don't care about writefds and exceptfds:
  select(STDIN+1, &readfds, NULL, NULL, &tv);
  if (FD_ISSET(STDIN, &readfds))
    printf("A key was pressed!\n");
  else
    printf("Timed out.\n");
  
  return 0;
}
