
#ifdef __cplusplus

#define FALSE false
#define TRUE true
typedef char BOOL;

#else
#ifdef _Bool
typedef typeof(_Bool) BOOL;
#else
typedef char BOOL;
#endif

#ifdef HAVE_STDBOOL_H
#include<stdbool.h>
#define FALSE false
#define TRUE true

#else
enum{FALSE=0!=0,TRUE=1==1};
#endif

#endif
