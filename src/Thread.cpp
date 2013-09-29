/***************************************************/
/*! \class Thread
    \brief STK thread class.

    This class provides a uniform interface for
    cross-platform thread use.  On Linux and IRIX
    systems, the pthread library is used.  Under Windows,
    the Windows thread library is used.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Thread.h"
#include<iostream.h>

Thread :: Thread()
{
  thread = 0;
}

Thread :: ~Thread()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_cancel(thread);
  pthread_join(thread, NULL);

#elif defined(__OS_WINDOWS__)

  if ( thread )
    TerminateThread((HANDLE)thread, 0);

#endif
}

bool Thread :: start( THREAD_FUNCTION routine, void * ptr )
{
  bool result = false;
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  if ( pthread_create(&thread, NULL, *routine, ptr) == 0 )
    result = true;

#elif defined(__OS_WINDOWS__)
  unsigned thread_id;
  thread = _beginthreadex(NULL, 0, routine, ptr, 0, &thread_id);
  if ( thread ) result = true;

#endif
  return result;
}

bool Thread :: wait( long milliseconds )
{
  bool result = false;
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_cancel(thread);
  pthread_join(thread, NULL);

#elif defined(__OS_WINDOWS__)

  DWORD timeout, retval;
  if ( milliseconds < 0 ) timeout = INFINITE;
  else timeout = milliseconds;
  retval = WaitForSingleObject( (HANDLE)thread, timeout );
  if ( retval == WAIT_OBJECT_0 ) {
    CloseHandle( (HANDLE)thread );
    thread = 0;
    result = true;
  }

#endif
  return result;
}

void Thread :: test(void)
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_testcancel();

#endif
}


Mutex :: Mutex()
{

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_mutex_init(&mutex, NULL);

#elif defined(__OS_WINDOWS__)

  InitializeCriticalSection(&mutex);

#endif 
}

Mutex :: ~Mutex()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_mutex_destroy(&mutex);

#elif defined(__OS_WINDOWS__)

  DeleteCriticalSection(&mutex);

#endif 
}

void Mutex :: lock()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_mutex_lock(&mutex);

#elif defined(__OS_WINDOWS__)

  EnterCriticalSection(&mutex);

#endif 
}

void Mutex :: unlock()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_mutex_unlock(&mutex);

#elif defined(__OS_WINDOWS__)

  LeaveCriticalSection(&mutex);

#endif 
}
