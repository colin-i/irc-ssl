
#include "inc/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

void*memcpy(void*,const void*,size_t);
char * strchr ( const char *, int );
int strcmp(const char*,const char*);
size_t strlen(const char*);
int strncmp(const char*,const char*,size_t);
char * strstr(const char *haystack, const char *needle);

#ifdef __cplusplus
}
#endif
