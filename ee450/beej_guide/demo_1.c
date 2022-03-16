// Steven Karl
// Beej Guide Demo - Page 15


// Includes
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

// Main function
int main(int argc, const char * agrv[]){
  //
  struct sockaddr_in sa;
  struct sockaddr_in6 sa6;

  int t = inet_pton(AF_INET,  "10.12.110.57", &(sa.sin_addr));
  int d = inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr));
  printf("Value is %d\n", t);
  printf("Value is %d\n", d);

  // IPv4:
  char ip4[INET_ADDRSTRLEN];
  struct sockaddr_in sa_1;

  inet_ntop(AF_INET, &(sa_1.sin_addr), ip4, INET_ADDRSTRLEN);
  printf("The IPv4 address is: %s\n", ip4);

  //IPv6
  char ip6[INET6_ADDRSTRLEN];
  struct sockaddr_in sa6_1;

  inet_ntop(AF_INET6, &(sa6_1.sin_addr), ip6, INET6_ADDRSTRLEN);
  printf("The IPv6 address is: %s\n", ip6);
  return 0;
}
