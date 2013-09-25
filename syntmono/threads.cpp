// Thread functions for use with syntmono.
//
// Gary P. Scavone, 1999.

#include "threads.h"

#if defined(__STK_REALTIME_)

#define SERVICE_PORT 2001  // Socket Port ID number

// Do OS dependent declarations and includes
#if defined(__OS_IRIX_)
#include <signal.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>

pid_t string_thread;

#elif defined(__OS_Linux_)
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>

pthread_t string_thread;

#elif defined(__OS_Win_)
#include <process.h>
#include <winsock.h>

unsigned long string_thread;

#endif


// The thread function definition protocols are slightly
// different under Irix, Linux, and Windoze.
#if defined(__OS_IRIX_)

void newStringByPipe(void *)

#elif defined(__OS_Linux_)

void *newStringByPipe(void *)

#elif defined(__OS_Win_)

void newStringByPipe(void *)

#endif

{
  extern int numStrings, notDone;
  extern char **inputString;
  int i;

  // Malloc inputString.
  inputString = (char **) malloc(MAX_IN_STRINGS * sizeof(char *));
  for ( i=0;i<MAX_IN_STRINGS;i++ )
    inputString[i] = (char *) malloc(STRING_LEN * sizeof(char));

  int inOne = 0;
  while (notDone) {
    fgets(inputString[inOne],STRING_LEN,stdin);
    if (inputString[inOne][2] == 'i' && inputString[inOne][3] == 't'
        && inputString[inOne][1] == 'x' && inputString[inOne][0] == 'E') {
      notDone = 0;
    }
    else {
      numStrings++;
      if (numStrings > MAX_IN_STRINGS) {
        fprintf(stderr,"Losing MIDI data ... try increasing MAX_IN_STRINGS.\n");
        numStrings--;
      }
      inOne++;
      if (inOne == MAX_IN_STRINGS) inOne = 0;
    }
  }

  // Free inputString.
  for ( i=0;i<MAX_IN_STRINGS;i++ ) free(inputString[i]);
  free(inputString);
}


#if defined(__OS_IRIX_)

void newStringBySocket(void *)

#elif defined(__OS_Linux_)

void *newStringBySocket(void *)

#elif defined(__OS_Win_)

void newStringBySocket(void *)

#endif

{
  extern int numStrings, notDone;
  extern char **inputString;

  int inOne = 0, i=0, m=0, n, parsing;
  int soc_id, accept_id;
  int maxfd, fd;
  fd_set mask, rmask;
  struct sockaddr_in sockname;
  char socBuf[STRING_LEN];
  static struct timeval timeout = {0, 1000}; // one millisecond

  // Malloc inputString.
  inputString = (char **) malloc(MAX_IN_STRINGS * sizeof(char *));
  for ( i=0;i<MAX_IN_STRINGS;i++ )
    inputString[i] = (char *) malloc(STRING_LEN * sizeof(char));

  memset(socBuf, 0, sizeof(socBuf));

#if defined(__OS_Win_)  // Stupid Windoze only stuff
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(1,1);

  int nRet = WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested)  {   
    fprintf(stderr,"\n Wrong Windoze socket library version!\n");
    exit(0);
  }
#endif

  // Create the server-side socket
  soc_id = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(soc_id < 0) {
    fprintf(stderr,"Couldn't create socket ... aborting!\n");
    exit(0);
  }

  sockname.sin_family=AF_INET;
  sockname.sin_addr.s_addr=INADDR_ANY;
  sockname.sin_port=htons(SERVICE_PORT);

  /* Bind socket to the appropriate port and interface (INADDR_ANY) */
  if (bind(soc_id,(struct sockaddr *)&sockname,sizeof(sockname)) < 0) {
    fprintf(stderr,"Couldn't bind socket ... aborting!\n");
    exit(0);
  }

  /* Listen for incoming connections */
  printf("Listening for socket connections on port %d\n", SERVICE_PORT);
  if (listen(soc_id,SOMAXCONN) < 0) {
    fprintf(stderr,"Couldn't set up listen on socket ... aborting!\n");
    exit(0);
  }

  FD_ZERO(&mask);
  FD_SET(soc_id, &mask);
  maxfd = soc_id;

  while (notDone) {
    rmask = mask;
    select(maxfd+1, &rmask, (fd_set *)0, (fd_set *)0, &timeout);
    if (FD_ISSET(soc_id,&rmask)) { // a new connection is available
      // Accept and service the incoming connection request
      accept_id=accept(soc_id,NULL,NULL);
      if (accept_id < 0) {
        fprintf(stderr,"Couldn't accept incoming connection on socket ... aborting!\n");
        exit(0);
      }
      printf("New socket connection made ... ready to receive SKINI messages.\n");

      FD_SET(accept_id, &mask);
      if (accept_id > maxfd) maxfd = accept_id;
      FD_CLR(soc_id, &rmask);
    }
    for (fd=0;fd<=maxfd;fd++) {   // look for other sockets with data
      if (FD_ISSET(fd, &rmask)) { // process the data
        parsing = 1;
        while (parsing) {
          i = recv(fd, socBuf, STRING_LEN,0);
          if (i==0) {
            printf("Closing a socket connection.\n");
            FD_CLR(fd, &mask);
#if defined(__OS_Win_)
            closesocket(fd);
#else
            close(fd);
#endif
            parsing = 0;
          }
          n = 0;
          while (n < i) {
            inputString[inOne][m++] = socBuf[n];
            if (socBuf[n++] == '\n') {
              if (inputString[inOne][2] == 'i' && inputString[inOne][3] == 't'
                  && inputString[inOne][1] == 'x' && inputString[inOne][0] == 'E') {
                notDone = 0;
                n = i;
                parsing = 0;
              }
              else {
                m = 0;
                if (n >= i) parsing = 0;
                numStrings++;
                if (numStrings > MAX_IN_STRINGS) {
                  fprintf(stderr,"Losing MIDI data ... try increasing MAX_IN_STRINGS.\n");
                  numStrings--;
                }
                inOne++;
                if (inOne == MAX_IN_STRINGS) inOne = 0;
                memset(inputString[inOne], 0, STRING_LEN);
              }
            }
          }
        }
      }
    }
  }
#if defined(__OS_Win_)  // Stupid Windoze only stuff
  closesocket(soc_id);
  WSACleanup();
#else
  shutdown(soc_id,0);
#endif

  // Free inputString.
  for ( i=0;i<MAX_IN_STRINGS;i++ ) free(inputString[i]);
  free(inputString);

	printf("Socket connection closed.\n");
}


void startPipeThread()
{
#if defined(__OS_IRIX_)
  string_thread = sproc(newStringByPipe, PR_SALL);
  if (string_thread == -1)  {
    fprintf(stderr, "unable to create input pipe thread ... aborting.\n");
    exit(0);
  }
#elif defined(__OS_Linux_)
  if (pthread_create(&string_thread, NULL, newStringByPipe, NULL)) {
    fprintf(stderr, "unable to create input pipe thread ... aborting.\n");
    exit(0);
  }
#elif defined(__OS_Win_)
  string_thread = _beginthread(newStringByPipe, 0, NULL);
  if (string_thread == -1)  {
    fprintf(stderr, "unable to create input pipe thread ... aborting.\n");
    exit(0);
  }
#endif
}

void startSocketThread()
{
#if defined(__OS_IRIX_)
  string_thread = sproc(newStringBySocket, PR_SALL);
  if (string_thread == -1)  {
    fprintf(stderr, "unable to create input socket thread...aborting.\n");
    exit(0);
  }
#elif defined(__OS_Linux_)
  if (pthread_create(&string_thread, NULL, newStringBySocket, NULL)) {
    fprintf(stderr, "unable to create input socket thread...aborting.\n");
    exit(0);
  }
#elif defined(__OS_Win_)
  string_thread = _beginthread(newStringBySocket, 0, NULL);
  if (string_thread == -1)  {
    fprintf(stderr, "unable to create input socket thread...aborting.\n");
    exit(0);
  }
#endif
}

#endif
