
#include "inc/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

void * memchr ( const void *, int, size_t );
int memcmp ( const void * ptr1, const void * ptr2, size_t num );
void*memcpy(void*,const void*,size_t);
char * strchr ( const char *, int );
int strcmp(const char*,const char*);
char* strcpy(char* destination, const char* source);
size_t strlen(const char*);

#ifdef __cplusplus
}
#endif
