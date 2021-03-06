// Steven Karl
// Beej Guide - Demo
// Server listen to host IP on port 3490

//Includes
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Functions
int main(int argc, const char * argv[]){

  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo; // Will point to the results linked list

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC; // dont care v4 or v6
  hints.ai_socktype = SOCK_STREAM; // TCP stream socket
  hints.ai_flags = AI_PASSIVE; // fill in the IP for me

  // Get the info
  if((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0){
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }

  // Free up the linked-list
  freeaddrinfo(servinfo) 
  
  return 0;

}
