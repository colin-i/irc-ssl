
#ifndef sockaddr_set_once
#define sockaddr_set_once

typedef unsigned short sa_family_t;

struct sockaddr {
  sa_family_t sa_family;
  char sa_data[14];
};

#endif
