
#define AF_INET 2
#define SOCK_STREAM 1

typedef unsigned short sa_family_t;
struct sockaddr {
  sa_family_t sa_family;
  char sa_data[14];
};
typedef int socklen_t;//int32_t
typedef int ssize_t;

#ifdef __cplusplus
extern "C" {
#endif

int connect(int __fd, const struct sockaddr* __addr, socklen_t __addr_length);
ssize_t recv(int __fd, void* __buf, size_t __n, int __flags);
ssize_t send(int __fd, const void* __buf, size_t __n, int __flags);
int socket(int __af, int __type, int __protocol);
int shutdown (int socket, int how);

#ifdef __cplusplus
}
#endif
