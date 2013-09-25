// Thread functions for use with syntmono.
//
// Gary P. Scavone, 1999.

#include "../STK/Object.h"

#define STRING_LEN 60

#if (defined(__STK_REALTIME_) && defined(__OS_IRIX_) )

void newStringByPipe(void *);
void newStringBySocket(void *);

#elif (defined(__STK_REALTIME_) && defined(__OS_Linux_) )

void *newStringByPipe(void *);
void *newStringBySocket(void *);

#elif (defined(__STK_REALTIME_) && defined(__OS_Win_) )

void newStringByPipe(void *);
void newStringBySocket(void *);

#endif

void startPipeThread();
void startSocketThread();
