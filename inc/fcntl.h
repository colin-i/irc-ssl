
//asm-generic/fcntl.h
#define O_WRONLY 00000001
#define O_TRUNC 00001000

#ifdef __cplusplus
extern "C" {
#endif

//typedef unsigned short mode_t;3rd argument
int open(const char*,int,...);

#ifdef __cplusplus
}
#endif
