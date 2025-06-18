
#ifdef HAVE_WINDOWS_H
	typedef long long time_t;
#else
	typedef long int time_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

time_t time(time_t *t);

#ifdef __cplusplus
}
#endif
