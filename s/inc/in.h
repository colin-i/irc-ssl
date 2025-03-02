
#include "sockaddr.h"

struct in_addr {
    unsigned int s_addr;  // load with inet_aton()
};

typedef unsigned short int in_port_t;

struct sockaddr_in
{
  sa_family_t sin_family;
  in_port_t sin_port;
  struct in_addr sin_addr;


  unsigned char sin_zero[sizeof (struct sockaddr)
    - (sizeof (unsigned short int))
    - sizeof (in_port_t)
    - sizeof (struct in_addr)];
};

//struct sockaddr_in {
//   short            sin_family;   // e.g. AF_INET
//    unsigned short   sin_port;     // e.g. htons(3490)
//    struct in_addr   sin_addr;     // see struct in_addr, below
//    char             sin_zero[8];  // zero this if you want to
//};

#define htons(a) (unsigned short)(((a)>>8) | ((a)<<8))
