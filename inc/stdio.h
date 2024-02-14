
#include"inc/stddef.h"

#define EOF (-1)

typedef struct __sFILE FILE;

#ifdef __cplusplus
extern "C" {
#endif

int fclose(FILE *stream);
int fileno(FILE *stream);
FILE *fopen(const char */*restrict*/ pathname, const char */*restrict*/ mode);
size_t fread(void */*ptr[restrict .size * .nmemb]*/, size_t size, size_t nmemb, FILE */*restrict*/ stream);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
size_t fwrite(const void */*ptr[restrict .size * .nmemb]*/, size_t size, size_t nmemb, FILE */*restrict*/ stream);
int getchar(void);
ssize_t getdelim(char **/*restrict*/ lineptr, size_t */*restrict*/ n, int delim, FILE */*restrict*/ stream);
int printf( const char * format, ... );
int puts(const char*);
int sscanf(const char *str, const char *format, ...);
int snprintf(char* __buf, size_t __size, const char* __fmt, ...);
int sprintf(char*,const char*,...);

#ifdef __cplusplus
}
#endif
