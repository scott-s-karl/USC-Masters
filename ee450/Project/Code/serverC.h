// Steven Karl
// Backend Server C Header
// ------------------------

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

void
check_number_of_args(int argc);

void
set_sock_preferences(struct addrinfo *sock_preferences);

void
check_if_getaddrinfo_failed(int getaddrinfo_result);

void
create_sock_and_bind(int *sock_fd, struct addrinfo *poss_cnntns);

