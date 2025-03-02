
typedef unsigned long sigset_t;
#define SIGUSR1 10
#define SIG_BLOCK 0

#ifdef __cplusplus
extern "C" {
#endif

int sigemptyset(sigset_t* __set);
int sigaddset(sigset_t* __set, int __signal);
int pthread_sigmask(int __how, const sigset_t* __new_set, sigset_t* __old_set);
int sigwait(const sigset_t* __set, int* __signal);

#ifdef __cplusplus
}
#endif
