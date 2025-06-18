
#ifdef HAVE_WINDOWS_H
	typedef unsigned long long pthread_t;
#else
	typedef unsigned long int pthread_t;
#endif
typedef void pthread_attr_t;

#ifdef __cplusplus
extern "C" {
#endif

int pthread_create(pthread_t* __pthread_ptr, pthread_attr_t const* __attr, void* (*__start_routine)(void*), void*);
int pthread_kill(pthread_t __pthread, int __signal);

#ifdef __cplusplus
}
#endif
