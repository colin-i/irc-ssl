
#ifdef HAVE_WINDOWS_H
	typedef unsigned long long size_t;
	typedef long long __ssize_t;
#else
	typedef long unsigned int size_t;
	typedef long int __ssize_t;
#endif
typedef __ssize_t ssize_t;
