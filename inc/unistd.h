
#include "inc/stddef.h"
#include"inc/sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

int close(int);
unsigned int sleep(unsigned int seconds);
ssize_t write(int,const void*,size_t);

#ifdef __cplusplus
}
#endif
