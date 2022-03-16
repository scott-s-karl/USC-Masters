// Steven Karl
// Beej Guide - Demo
// Client listen to a server on port 3490

// Includes

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Local Functions
int main(){

  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo; // will point to the results

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // get ready to connect
  status = getaddrinfo("www.example.net", "3490", &hints, &servinfo);

  // servinfo now points to a linked list of 1 or more struct addrinfos 13
  // etc.
}
