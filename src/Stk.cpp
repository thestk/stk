/***************************************************/
/*! \class Stk
    \brief STK base class

    Nearly all STK classes inherit from this class.
    The global sample rate can be queried and
    modified via Stk.  In addition, this class
    provides error handling and byte-swapping
    functions.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Stk.h"
#include <stdio.h>
#include <string.h>

MY_FLOAT Stk :: srate = (MY_FLOAT) SRATE;
std::string Stk :: rawwavepath = RAWWAVE_PATH;
const Stk::STK_FORMAT Stk :: STK_SINT8 = 1;
const Stk::STK_FORMAT Stk :: STK_SINT16 = 2;
const Stk::STK_FORMAT Stk :: STK_SINT32 = 8;
const Stk::STK_FORMAT Stk :: MY_FLOAT32 = 16;
const Stk::STK_FORMAT Stk :: MY_FLOAT64 = 32;

Stk :: Stk(void)
{
}

Stk :: ~Stk(void)
{
}

MY_FLOAT Stk :: sampleRate(void)
{
  return srate;
}

void Stk :: setSampleRate(MY_FLOAT newRate)
{
  if (newRate > 0)
    srate = newRate;
}

std::string Stk :: rawwavePath(void)
{
  return rawwavepath;
}

void Stk :: setRawwavePath(std::string newPath)
{
  if ( !newPath.empty() )
    rawwavepath = newPath;

  // Make sure the path includes a "/"
  if ( rawwavepath[rawwavepath.length()-1] != '/' )
    rawwavepath += "/";
}

void Stk :: swap16(unsigned char *ptr)
{
  register unsigned char val;

  // Swap 1st and 2nd bytes
  val = *(ptr);
  *(ptr) = *(ptr+1);
  *(ptr+1) = val;
}

void Stk :: swap32(unsigned char *ptr)
{
  register unsigned char val;

  // Swap 1st and 4th bytes
  val = *(ptr);
  *(ptr) = *(ptr+3);
  *(ptr+3) = val;

  //Swap 2nd and 3rd bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+1);
  *(ptr+1) = val;
}

void Stk :: swap64(unsigned char *ptr)
{
  register unsigned char val;

  // Swap 1st and 8th bytes
  val = *(ptr);
  *(ptr) = *(ptr+7);
  *(ptr+7) = val;

  // Swap 2nd and 7th bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+5);
  *(ptr+5) = val;

  // Swap 3rd and 6th bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+3);
  *(ptr+3) = val;

  // Swap 4th and 5th bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+1);
  *(ptr+1) = val;
}

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))
  #include <unistd.h>
#elif defined(__OS_WINDOWS__)
  #include <windows.h>
#endif

void Stk :: sleep(unsigned long milliseconds)
{
#if defined(__OS_WINDOWS__)
  Sleep((DWORD) milliseconds);
#elif (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))
  usleep( (unsigned long) (milliseconds * 1000.0) );
#endif
}

void Stk :: handleError( const char *message, StkError::TYPE type )
{
  if (type == StkError::WARNING)
    fprintf(stderr, "\n%s\n\n", message);
  else if (type == StkError::DEBUG_WARNING) {
#if defined(_STK_DEBUG_)
    fprintf(stderr, "\n%s\n\n", message);
#endif
  }
  else {
    // Print error message before throwing.
    fprintf(stderr, "\n%s\n\n", message);
    throw StkError(message, type);
  }
}

StkError :: StkError(const char *p, TYPE tipe)
  : type(tipe)
{
  strncpy(message, p, 256);
}

StkError :: ~StkError(void)
{
}

void StkError :: printMessage(void)
{
  printf("\n%s\n\n", message);
}
