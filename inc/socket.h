
#define AF_INET 2
#define SO_SNDBUF 7
#define SOCK_STREAM 1
#define SOL_SOCKET 1

typedef unsigned short sa_family_t;
struct sockaddr {
  sa_family_t sa_family;
  char sa_data[14];
};
typedef int socklen_t;//int32_t

#ifdef __cplusplus
extern "C" {
#endif

int connect(int __fd, const struct sockaddr* __addr, socklen_t __addr_length);
int getsockopt(int __fd, int __level, int __option, void* __value, socklen_t* __value_length);int socket(int __af, int __type, int __protocol);
int shutdown (int socket, int how);

#ifdef __cplusplus
}
#endif
