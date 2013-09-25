/******************************************/
/*
  StrmWvIn Audio Input Class,
  by Gary P. Scavone, 2000

  This object inherits from WvIn and is used
  to accept 16-bit data (signed integer) via
  a socket connection (streamed audio).
  Streamed data must be in big-endian format,
  which conforms to network byte ordering.

  This class starts a socket server, which
  waits for a remote connection.  Actual data
  reading and buffering takes place in a thread.
*/
/******************************************/

#include "StrmWvIn.h"

#ifdef __LITTLE_ENDIAN__
  #include "ByteSwap.h"
#endif

#define NUM_STREAM_IN_FRAGMENTS 10
#define STK_STREAM_PORT 2005

char *stream_buffer;
int local_socket = 0;
int g_nfrags_filled = 0;
int g_thread_done = 0;

// Do OS dependent declarations and includes
#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>

pthread_t strm_thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define MUTEX_LOCK(A)      pthread_mutex_lock(A)
#define MUTEX_UNLOCK(A)    pthread_mutex_unlock(A)

#elif defined(__OS_Win_)
#include <process.h>
#include <winsock.h>

unsigned long strm_thread;
CRITICAL_SECTION mutex;

#define MUTEX_LOCK(A)      EnterCriticalSection(A)
#define MUTEX_UNLOCK(A)    LeaveCriticalSection(A)

#endif

#if (defined(__OS_IRIX_) || defined(__OS_Linux_))

void *streamInThread(void * chan)

#elif defined(__OS_Win_)

void streamInThread(void * chan)

#endif

{
  extern char *stream_buffer;

  int remote_socket;
  char msg[256];
  int fd, i = -1, writefrag = 0;
  long fill_size = RT_BUFFER_SIZE * (int)chan * 2; // bytes
  fd_set mask, rmask;
  struct sockaddr_in mysocket;
  static struct timeval timeout = {0, 10000}; // ten milliseconds

#if defined(__OS_Win_)  // Windoze-only stuff
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(1,1);

  WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested)  {   
    sprintf(msg, "StrmWvIn: Wrong Windoze socket library version!\n");
    throw StkError(msg, StkError::PROCESS_SOCKET);
  }
#endif

  // Create the server-side socket
  local_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (local_socket < 0) {
    sprintf(msg, "StrmWvIn: Couldn't create socket!\n");
    throw StkError(msg, StkError::PROCESS_SOCKET);
  }

  mysocket.sin_family=AF_INET;
  mysocket.sin_addr.s_addr=INADDR_ANY;
  mysocket.sin_port=htons(STK_STREAM_PORT);

  /* Bind socket to the appropriate port and interface (INADDR_ANY) */
  if (bind(local_socket, (struct sockaddr *)&mysocket, sizeof(mysocket)) < 0) {
    sprintf(msg, "StrmWvIn: Couldn't bind socket!\n");
    throw StkError(msg, StkError::PROCESS_SOCKET);
  }

  /* Listen for one incoming connection */
  if (listen(local_socket, 1) < 0) {
    sprintf(msg, "StrmWvIn: Couldn't set up listen on socket!\n");
    throw StkError(msg, StkError::PROCESS_SOCKET);
  }
  printf("\nStrmWvIn listening for a connection on port %d\n", STK_STREAM_PORT);

  remote_socket=accept(local_socket, NULL, NULL);
  if (remote_socket < 0) {
    sprintf(msg, "StrmWvIn: Couldn't accept connection request!\n");
    throw StkError(msg, StkError::PROCESS_SOCKET);
  }
  printf(" ... connection made!\n");

  FD_ZERO(&mask);
  fd = remote_socket;
  FD_SET(fd, &mask);

  for (;;) {
    rmask = mask;
    // select will block until data is available for reading
    select(fd+1, &rmask, (fd_set *)0, (fd_set *)0, NULL);
    if (FD_ISSET(fd, &rmask)) { // data is available
      MUTEX_LOCK(&mutex);
      // don't fill if we're ahead
      if (g_nfrags_filled < NUM_STREAM_IN_FRAGMENTS) {
#if defined(__OS_Win_)
        // Yet again, we have to make an extra special effort to get things to work
        // under windoze ... why do I waste my time supporting this shitty OS?
        // The windoze recv() call doesn't allow the use of the MSG_WAITALL flag.
        // The following scheme should work but could cause even worse performance if
        // we're in an underrun situation.
        int j;
        i = recv(fd, &stream_buffer[fill_size*writefrag], fill_size, 0);
        if (i == SOCKET_ERROR) i = 0;
        while (i < fill_size && i > 0) {
          j = recv(fd, &stream_buffer[fill_size*writefrag+i], fill_size-i, 0);
          if (j == 0 || j == SOCKET_ERROR) {
            i = 0;
            break;
          }
          i += j;
        }
#else
        i = recv(fd, &stream_buffer[fill_size*writefrag], fill_size, MSG_WAITALL);
#endif
        if (i==0) {
          printf("The remote StrmWvIn socket connection has closed!\n");
#if defined(__OS_Win_)
          closesocket(fd);
#else
          close(fd);
#endif
          g_thread_done = 1;
          MUTEX_UNLOCK(&mutex);
          break;
        }
        g_nfrags_filled++;
        if (++writefrag == NUM_STREAM_IN_FRAGMENTS)
          writefrag = 0;
        MUTEX_UNLOCK(&mutex);
      }
      else { // wait a little
        MUTEX_UNLOCK(&mutex);
#if defined(__OS_Win_)
        // the windoze select function doesn't work as a timer
        Sleep((DWORD) 10);
#else
        // reset the timeout values because of linux "select" implementation
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000; // 0.01 seconds
        select(0, NULL, NULL, NULL, &timeout);
#endif
      }
    }
  }

  // We can only get here if the remote socket shuts down
#if defined(__OS_Win_)
  closesocket(local_socket);
  WSACleanup();
  _endthread();
#else
  close(local_socket);
  return NULL;
#endif
}

StrmWvIn :: StrmWvIn(int chans, MY_FLOAT srate)
{
  char msg[256];
  long buffer_size = RT_BUFFER_SIZE * NUM_STREAM_IN_FRAGMENTS * chans * 2;
  chunking = 1;
  looping = 0;

  stream_buffer = (char *) new char[buffer_size];
  memset(stream_buffer, 0, buffer_size);

  // start the input stream thread
#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
  if (pthread_create(&strm_thread, NULL, streamInThread, (void *)chans)) {
    sprintf(msg, "StrmWvIn: unable to create input stream thread!\n");
    throw StkError(msg, StkError::PROCESS_THREAD);
  }
#elif defined(__OS_Win_)
  InitializeCriticalSection(&mutex);
  strm_thread = _beginthread(streamInThread, 0, (void *)chans);
  if (strm_thread == 0)  {
    sprintf(msg, "StrmWvIn: unable to create input stream thread!\n");
    throw StkError(msg, StkError::PROCESS_THREAD);
  }
#endif

  channels = chans;
  bufferSize = RT_BUFFER_SIZE;
  data = 0;
  strmdata = (INT16 *) new INT16[(bufferSize+1)*channels];

  lastSamples = (INT16 *) new INT16[channels];
  for (int i=0;i<channels;i++) {
    lastSamples[i] = (INT16) 0.0;
  }

  this->getData(0);

  rate = (MY_FLOAT) srate / SRATE;
  if (fmod(rate, 1.0) > 0.0) interpolate = 1;
  else interpolate = 0;
  phaseOffset = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
  this->reset();
}

StrmWvIn :: ~StrmWvIn()
{
#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
  shutdown(local_socket,0);
  pthread_cancel(strm_thread);
  pthread_join(strm_thread, NULL);
  pthread_mutex_destroy(&mutex);
#elif defined(__OS_Win_)
  closesocket(local_socket);
  WSACleanup();
  TerminateThread((HANDLE)strm_thread,0);
  DeleteCriticalSection(&mutex);
#endif

  if (strmdata) {
    delete [ ] strmdata;
    strmdata = 0;
  }
  if (stream_buffer) {
    delete [] stream_buffer;
    stream_buffer = 0;
  }
  if (lastSamples) {
    delete [ ] lastSamples;
    lastSamples = 0;
  }
}

void StrmWvIn :: setRate(MY_FLOAT aRate)
{
  // Negative rates not allowed for realtime input
  rate = fabs(aRate);

  if (fmod(rate, 1.0) != 0.0) interpolate = 1;
  else interpolate = 0;
}

void StrmWvIn :: addTime(MY_FLOAT aTime)   
{
  // Negative time shift no allowed for realtime input
  time += fabs(aTime);
}

void StrmWvIn :: setLooping(int aLoopStatus)
{
  // No looping for realtime data.
  looping = 0;
}

long StrmWvIn :: getSize()
{
  return bufferSize;
}

void StrmWvIn :: getData(long index)
{
  /* We have two potential courses of action should this method
   * be called and the stream_buffer isn't sufficiently filled.
   * One solution is to fill the data buffer with zeros and return.
   * The other solution is to wait until the necessary data exists.
   * I chose the latter, as it works for both streamed files
   * (non-realtime data transport) and realtime playback (given
   * adequate network bandwidth and speed).
   */
  static int readfrag = 0, i;
  static struct timeval timer = {0, 10000}; // ten milliseconds
  long temp = RT_BUFFER_SIZE*channels;

  /* wait until data is ready */
  while (g_nfrags_filled == 0) {
#if defined(__OS_Win_)
    // the windoze select function doesn't work as a timer
    Sleep((DWORD) 10);
#else
    timer.tv_sec = 0;
    timer.tv_usec = 10000; // 0.01 seconds
    select(0, NULL, NULL, NULL, &timer);
#endif
    if (g_thread_done) {
      finished = 1;
      return;
    }
  }

  /* copy data from stream_buffer to strmdata */
  MUTEX_LOCK(&mutex);
  memcpy(&strmdata[channels], &stream_buffer[readfrag*temp*2], temp*2);
#ifdef __LITTLE_ENDIAN__
  for (i=0; i<temp+channels; i++)
    swap16((unsigned char *) (strmdata+i));
#endif
  g_nfrags_filled--;
  if (++readfrag == NUM_STREAM_IN_FRAGMENTS)
    readfrag = 0;
  MUTEX_UNLOCK(&mutex);

  /* Fill in the extra sample frame for interpolation.
   * We do this by pre-pending the last sample frame
   * from the previous input buffer to the current one.
   */
  for (i=0;i<channels;i++) {
    strmdata[i] = lastSamples[i];
    lastSamples[i] = strmdata[temp+i];
  }

  if (g_thread_done && g_nfrags_filled == 0) finished = 1;
}

int StrmWvIn :: informTick()
{
  static MY_FLOAT alpha;
  static long index;

  if (finished) return finished;

  if (time >= bufferSize) {
    this->getData(0);
    while (time >= bufferSize)
      time -= bufferSize;
  }

  // integer part of time address
  index = (long) time;

  if (interpolate) {
    // fractional part of time address
    alpha = time - (MY_FLOAT) index;
    index *= channels;
    for (int i=0;i<channels;i++) {
      //  Do linear interpolation
      lastOutput[i] = (MY_FLOAT) strmdata[index];
      lastOutput[i] += alpha * ((MY_FLOAT) strmdata[index+channels] - lastOutput[i]);
      lastOutput[i] *= 0.00003051;
      index++;
    }
  }
  else {
    index *= channels;
    for (int i=0;i<channels;i++) {
      lastOutput[i] = strmdata[index++];
      lastOutput[i] *= 0.00003051;
    }
  }

  // increment time, which can be negative
  time += rate;

  return finished;
}
