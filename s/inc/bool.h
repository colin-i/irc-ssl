
#ifdef __cplusplus

#define FALSE false
#define TRUE true
typedef int BOOL;//char is conflict with windows.h

#else
#ifdef _Bool
typedef typeof(_Bool) BOOL;
#else
typedef int BOOL;
#endif

#ifdef HAVE_STDBOOL_H
#include<stdbool.h>
#define FALSE false
#define TRUE true

#else
enum{FALSE=0!=0,TRUE=1==1};
#endif

#endif
