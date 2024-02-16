
#include "inc/stddef.h"

typedef long int off_t;

#define F_OK 0

//#include <bits/seek_constants.h>
#define SEEK_SET 0
#define SEEK_END 2

#ifdef __cplusplus
extern "C" {
#endif

int access(const char*,int);
int chdir(const char *path);
int close(int);
off_t lseek(int,off_t,int);
int ftruncate(int fd, off_t length);
ssize_t read(int,void*,size_t);
int rmdir(const char *pathname);
unsigned int sleep(unsigned int seconds);
int unlink(const char*);
ssize_t write(int,const void*,size_t);

#ifdef __cplusplus
}
#endif
