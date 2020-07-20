
#include "inc/stddef.h"

typedef int ssize_t;
typedef long off_t;

//#include <bits/seek_constants.h>
#define SEEK_SET 0
#define SEEK_END 2

#ifdef __cplusplus
extern "C" {
#endif

int close(int);
off_t lseek(int,off_t,int);
ssize_t read(int,void*,size_t);
unsigned int sleep(unsigned int seconds);
ssize_t write(int,const void*,size_t);

#ifdef __cplusplus
}
#endif
