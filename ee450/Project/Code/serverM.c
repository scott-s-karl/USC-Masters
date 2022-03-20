// Steven Karl
// Main Server
// ---------------

// Include Statements
#include "serverM.h"

// Define Macros
#define MAINTCPPORT "24711" // Port # Main Server TCP runs on
#define MAINUDPPORT "24711" // Port # Main Server UDP runs on
#define SVRAPORT "21711" // Port # ServerA runs on
#define SVRBPORT "22711" // Port # ServerB runs on
#define SVRCPORT "23711" // Port # ServerC runs on


// Local Functions

void
check_number_of_args(int argc){
  if (argc > 1){
    fprintf(stderr, "The Main Server program doesn't take input!\n");
    exit(1);
  }
}

void
check_if_getaddrinfo_failed(int getaddrinfo_result, char *s){
  if(getaddrinfo_result){
    fprintf(stderr,
	    "getaddrinfo value is  %s, which correlates to %s\n",
	    s,
	    gai_strerror(getaddrinfo_result));
    exit(1);
  }
}
/*
  Set the hints variable
*/
void
set_udp_sock_preferences(struct addrinfo *sock_prefs){
  memset(&sock_prefs, 0, sizeof(sock_prefs)); // Empty
  sock_prefs.ai_family = AF_UNSPEC; // IPv4 or IPv6
  sock_prefs.ai_socktype = SOCK_DGRAM; // UDP type
}

void
get_available_socket(struct addrinfo **cnntn, struct addrinfo *poss_cnntns){
  // Loop through connection in the linked list
  
}

// Main Function
int main(int argc const char *argv[]){

  // Local variables
  int main_server_tcp_fd; // File descriptor tcp
  int main_server_udp_fd; // File descriptor udp
  int getaddrinfo_result; // Function return code on error

  struct addrinfo tcp_sock_prefs; // tcp connection prefs
  struct addrinfo udp_sock_prefs; // udp connection prefs

  // Client A
  int client_a_fd;
  struct sockaddr_storage client_a_addr;
  socklen_t client_a_addr_sz;
  int client_a_port;
  
  // Client B
  int client_b_fd;
  struct sockaddr_storage client_b_addr;
  socklen_t client_b_addr_sz;
  int client_b_port;
  
  // Backend Server A
  struct addrinfo sva_sock_prefs; // General Socket Preferences
  struct addrinfo *sva_poss_cnntns; // Return Linked List
  struct addrinfo *sva_cnntn; // One of the items in list
  int sva_port; // ???
  set_udp_sock_preferences(&sva_sock_prefs); // AI_FAMILY/TYPE
  getaddrinfo_result = getaddrinfo(NULL,
				   SVRAPORT,
				   &sva_sock_prefs,
				   &sva_poss_cnntns); // setup structs
  check_if_getaddrinfo_failed(getaddrinfo_result, "sva"); // Check for non zero
  get_available_socket(&sva_cnntn, &sva_poss_cnntns); // Socket() call for returned connections
  
  // Backend Server B
  struct addrinfo *svb_sock_prefs;
  struct addrinfo *svb_cnntn;
  struct addrinfo *svb_poss_cnntns;
  int svb_port;
  set_udp_sock_prefences(&svb_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   SVRBPORT,
				   &svb_sock_prefs,
				   &svb_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "svb");
  get_available_socket(&svb_cnntn, &svb_poss_cnntns);
  
  // Backend Server C
  struct addrinfo *svc_sock_prefs;
  struct addrinfo *svc_poss_cnntns;
  struct addrinfo *svc_cnntn;
  int svc_port;
  set_udp_sock_preferences(&svc_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   SVRCPORT,
				   &svc_sock_prefs,
				   &svc_poss_cnntns);
  check_if_getaddrinfo_fail(getaddrinfo_result, "svc");
  get_available_socket(&svc_cnntn, &svc_poss_cnntns);
  
}
