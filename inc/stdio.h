
#include"inc/stddef.h"

#define EOF (-1)

typedef struct __sFILE FILE;

#ifdef __cplusplus
extern "C" {
#endif

int fclose(FILE *stream);
FILE *fopen(const char */*restrict*/ pathname, const char */*restrict*/ mode);
int getchar(void);
int printf( const char * format, ... );
int puts(const char*);
int sscanf(const char *str, const char *format, ...);
int snprintf(char* __buf, size_t __size, const char* __fmt, ...);
int sprintf(char*,const char*,...);

#ifdef __cplusplus
}
#endif
