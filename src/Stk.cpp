/***************************************************/
/*! \class Stk
    \brief STK base class

    Nearly all STK classes inherit from this class.
    The global sample rate can be queried and
    modified via Stk.  In addition, this class
    provides error handling and byte-swapping
    functions.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Stk.h"

StkFloat Stk :: srate_ = (StkFloat) SRATE;
std::string Stk :: rawwavepath_ = RAWWAVE_PATH;
const Stk::StkFormat Stk :: STK_SINT8   = 0x1;
const Stk::StkFormat Stk :: STK_SINT16  = 0x2;
const Stk::StkFormat Stk :: STK_SINT32  = 0x8;
const Stk::StkFormat Stk :: STK_FLOAT32 = 0x10;
const Stk::StkFormat Stk :: STK_FLOAT64 = 0x20;

Stk :: Stk(void)
{
}

Stk :: ~Stk(void)
{
}

void Stk :: setRawwavePath( std::string path )
{
  if ( !path.empty() )
    rawwavepath_ = path;

  // Make sure the path includes a "/"
  if ( rawwavepath_[rawwavepath_.length()-1] != '/' )
    rawwavepath_ += "/";
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

void Stk :: handleError( StkError::Type type )
{
  handleError( errorString_.str(), type );
  errorString_.str( std::string() ); // reset the ostringstream buffer
}

void Stk :: handleError( const char *message, StkError::Type type )
{
  std::string msg( message );
  handleError( msg, type );
}

void Stk :: handleError( std::string message, StkError::Type type )
{
  if (type == StkError::WARNING || type == StkError::STATUS )
    std::cerr << '\n' << message << '\n' << std::endl;
  else if (type == StkError::DEBUG_WARNING) {
#if defined(_STK_DEBUG_)
    std::cerr << '\n' << message << '\n' << std::endl;
#endif
  }
  else {
    // Print error message before throwing.
    std::cerr << '\n' << message << '\n' << std::endl;
    throw StkError(message, type);
  }
}

StkFrames :: StkFrames( unsigned int nFrames, unsigned int nChannels, bool interleaved )
  : nFrames_( nFrames ), nChannels_( nChannels ), interleaved_( interleaved )
{
  if ( nChannels == 0 ) {
    std::string message = "StkFrames::StkFrames: nChannels argument should be 1 or greater (even if nFrames = 0) ... correcting to one channel!";
    Stk::handleError( message, StkError::WARNING );
    nChannels_ = 1;
  }

  size_ = nFrames_ * nChannels_;
  if ( size_ > 0 ) data_.resize( size_, 0.0 );
}

StkFrames :: StkFrames( const StkFloat& value, unsigned int nFrames, unsigned int nChannels, bool interleaved )
  : nFrames_( nFrames ), nChannels_( nChannels ), interleaved_( interleaved )
{
  if ( nChannels == 0 ) {
    std::string message = "StkFrames::StkFrames: nChannels argument should be 1 or greater (even if nFrames = 0) ... correcting to one channel!";
    Stk::handleError( message, StkError::WARNING );
    nChannels_ = 1;
  }

  size_ = nFrames_ * nChannels_;
  if ( size_ > 0 ) data_.resize( size_, value );
}

void StkFrames :: resize( unsigned int nFrames, unsigned int nChannels, StkFloat value )
{
  nFrames_ = nFrames;
  nChannels_ = nChannels;

  if ( nChannels == 0 ) {
    std::string message = "StkFrames::resize(): nChannels argument should be 1 or greater (even if nFrames = 0) ... correcting to one channel!";
    Stk::handleError( message, StkError::WARNING );
    nChannels_ = 1;
  }

  size_t newSize = nFrames_ * nChannels_;
  if ( size_ != newSize ) {
    size_ = newSize;
    data_.resize( size_, value );
  }
}
