/***************************************************/
/*! \class Thread
    \brief STK thread class.

    This class provides a uniform interface for
    cross-platform thread use.  On Linux and IRIX
    systems, the pthread library is used.  Under Windows,
    the Windows thread library is used.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#include "Thread.h"

Thread :: Thread()
{
  thread_ = 0;
}

Thread :: ~Thread()
{
}

bool Thread :: start( THREAD_FUNCTION routine, void * ptr )
{
  if ( thread_ ) {
    errorString_ << "Thread:: a thread is already running!";
    handleError( StkError::WARNING );
    return false;
  }

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  if ( pthread_create(&thread_, NULL, *routine, ptr) == 0 )
    return true;

#elif defined(__OS_WINDOWS__)
  unsigned thread_id;
  thread_ = _beginthreadex(NULL, 0, routine, ptr, 0, &thread_id);
  if ( thread_ ) return true;

#endif
  return false;
}

bool Thread :: cancel()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  if ( pthread_cancel(thread_) == 0 ) {
    return true;
  }

#elif defined(__OS_WINDOWS__)

  TerminateThread((HANDLE)thread_, 0);
  return true;

#endif
  return false;
}

bool Thread :: wait()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  if ( pthread_join(thread_, NULL) == 0 ) {
    thread_ = 0;
    return true;
  }

#elif defined(__OS_WINDOWS__)

  long retval = WaitForSingleObject( (HANDLE)thread_, INFINITE );
  if ( retval == WAIT_OBJECT_0 ) {
    CloseHandle( (HANDLE)thread_ );
    thread_ = 0;
    return true;
  }

#endif
  return false;
}

void Thread :: testCancel(void)
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  pthread_testcancel();

#endif
}
