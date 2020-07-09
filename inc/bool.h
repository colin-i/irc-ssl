
#ifdef __cplusplus

#define FALSE false

#else

#ifdef HAVE_STDBOOL_H
#include<stdbool.h>
#define FALSE false

#else
//#ifdef _Bool
//typedef typeof(_Bool) bool;
//#else
//typedef char bool;
//#endif
enum{FALSE=0!=0/*,TRUE=1==1*/};
#endif

#endif
