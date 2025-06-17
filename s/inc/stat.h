
typedef unsigned short mode_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MKDIR_1ARG
int mkdir(const char *pathname);
#else
int mkdir(const char *pathname, mode_t mode);
#endif

#ifdef __cplusplus
}
#endif
