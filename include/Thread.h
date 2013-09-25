/***************************************************/
/*! \class Thread
    \brief STK thread class.

    This class provides a uniform interface for
    cross-platform threads.  On unix systems,
    the pthread library is used.  Under Windows,
    the C runtime threadex functions are used.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__THREAD_H)
#define __THREAD_H

#include "Stk.h"

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__))

  #include <pthread.h>
  #define THREAD_TYPE
  typedef pthread_t THREAD_HANDLE;
  typedef void * THREAD_RETURN;
  typedef void * (*THREAD_FUNCTION)(void *);
  typedef pthread_mutex_t MUTEX;

#elif defined(__OS_WINDOWS__)

  #include <windows.h>
  #include <process.h>
  #define THREAD_TYPE __stdcall
  typedef unsigned long THREAD_HANDLE;
  typedef unsigned THREAD_RETURN;
  typedef unsigned (__stdcall *THREAD_FUNCTION)(void *);
  typedef CRITICAL_SECTION MUTEX;

#endif

class Thread : public Stk
{
 public:
  //! Default constructor.
  Thread();

  //! The class destructor waits indefinitely for the thread to end before returning.
  ~Thread();

  //! Begin execution of the thread \e routine.  Upon success, TRUE is returned.
  /*!
    The thread routine can be passed an argument via \e ptr.  If
    the thread cannot be created, the return value is FALSE.
  */
  bool start( THREAD_FUNCTION routine, void * ptr = NULL );

  //! Wait the specified number of milliseconds for the thread to terminate.  Return TRUE on success.
  /*!
    If the specified time value is negative, the function will
    block indefinitely.  Otherwise, the function will block up to a
    maximum of the specified time.  A return value of FALSE indicates
    the thread did not terminate within the specified time limit.
  */
  bool wait( long milliseconds = -1 );

  //! Test for a thread cancellation request.
  static void test(void);

 protected:

  THREAD_HANDLE thread;

};

class Mutex : public Stk
{
 public:
  //! Default constructor.
  Mutex();

  //! Class destructor.
  ~Mutex();

  //! Lock the mutex.
  void lock(void);

  //! Unlock the mutex.
  void unlock(void);

 protected:

  MUTEX mutex;

};

#endif // defined(__THREAD_H)
