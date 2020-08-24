
//asm-generic/fcntl.h
#define O_RDONLY 00000000
#define O_WRONLY 00000001
#define O_CREAT 00000100
#define O_TRUNC 00001000

//linux/stat.h
#define S_IRUSR 00400
#define S_IWUSR 00200

#ifdef __cplusplus
extern "C" {
#endif

//typedef unsigned short mode_t;3rd argument
int open(const char*,int,...);

#ifdef __cplusplus
}
#endif
