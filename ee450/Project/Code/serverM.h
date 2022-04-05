// Steven Karl
// Main Server Header
// ---------------------

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// Enumerated Values

// Function Definition

void
check_number_of_args(int argc);

void
set_udp_sock_preferences(struct addrinfo *sock_prefs);

void
set_tcp_sock_preferences(struct addrinfo *sock_prefs);

void
get_available_socket(struct addrinfo **cnntn, struct addrinfo *poss_cnntns);

void
check_if_getaddrinfo_failed(int getaddrinfo_result, char *s);

void
create_tcp_sock_and_bind(int *sock_fd,
			 struct addrinfo *poss_cnntns);

