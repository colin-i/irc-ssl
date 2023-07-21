
#include"inc/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

int getchar(void);
int printf( const char * format, ... );
int puts(const char*);
int sscanf(const char *str, const char *format, ...);
int snprintf(char* __buf, size_t __size, const char* __fmt, ...);
int sprintf(char*,const char*,...);

#ifdef __cplusplus
}
#endif
