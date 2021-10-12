
#include "inc/stddef.h"

#define EXIT_SUCCESS 0

#ifdef __cplusplus
extern "C" {
#endif
int atoi(const char*);
void free(void*);
char*getenv(const char*);
void*malloc(size_t);
void* realloc(void* __ptr, size_t __byte_count);
#ifdef __cplusplus
}
#endif
