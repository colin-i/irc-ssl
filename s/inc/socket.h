
#define AF_INET 2
#define SOCK_STREAM 1

#include "sockaddr.h"

typedef unsigned int socklen_t;

#ifdef __cplusplus
extern "C" {
#endif

int connect(int __fd, const struct sockaddr* __addr, socklen_t __addr_length);
int shutdown (int socket, int how);
int socket(int __af, int __type, int __protocol);

#ifdef __cplusplus
}
#endif
