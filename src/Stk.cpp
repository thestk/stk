/***************************************************/
/*! \class Stk
    \brief STK base class

    Nearly all STK classes inherit from this class.
    The global sample rate can be queried and
    modified via Stk.  In addition, this class
    provides error handling and byte-swapping
    functions.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation files
    (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    Any person wishing to distribute modifications to the Software is
    asked to send the modifications to the original developer so that
    they can be incorporated into the canonical version.  This is,
    however, not a binding provision of this license.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/***************************************************/

#include "Stk.h"

StkFloat Stk :: srate_ = (StkFloat) SRATE;
std::string Stk :: rawwavepath_ = RAWWAVE_PATH;
const Stk::StkFormat Stk :: STK_SINT8   = 0x1;
const Stk::StkFormat Stk :: STK_SINT16  = 0x2;
const Stk::StkFormat Stk :: STK_SINT24  = 0x4;
const Stk::StkFormat Stk :: STK_SINT32  = 0x8;
const Stk::StkFormat Stk :: STK_FLOAT32 = 0x10;
const Stk::StkFormat Stk :: STK_FLOAT64 = 0x20;
bool Stk :: showWarnings_ = true;
bool Stk :: printErrors_ = true;
std::vector<Stk *> Stk :: alertList_;

Stk :: Stk( void )
  : ignoreSampleRateChange_(false)
{
}

Stk :: ~Stk( void )
{
}

void Stk :: setSampleRate( StkFloat rate )
{
  if ( rate > 0.0 && rate != srate_ ) {
    StkFloat oldRate = srate_;
    srate_ = rate;

    for ( unsigned int i=0; i<alertList_.size(); i++ )
      alertList_[i]->sampleRateChanged( srate_, oldRate );
  }
}

void Stk :: sampleRateChanged( StkFloat newRate, StkFloat oldRate )
{
  // This function should be reimplemented in classes that need to
  // make internal variable adjustments in response to a global sample
  // rate change.
}

void Stk :: addSampleRateAlert( Stk *ptr )
{
  for ( unsigned int i=0; i<alertList_.size(); i++ )
    if ( alertList_[i] == ptr ) return;

  alertList_.push_back( ptr );
}

void Stk :: removeSampleRateAlert( Stk *ptr )
{
  for ( unsigned int i=0; i<alertList_.size(); i++ ) {
    if ( alertList_[i] == ptr ) {
      alertList_.erase( alertList_.begin() + i );
      return;
    }
  }
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
  if ( type == StkError::WARNING || type == StkError::STATUS ) {
    if ( !showWarnings_ ) return;
    std::cerr << '\n' << message << '\n' << std::endl;
  }
  else if (type == StkError::DEBUG_WARNING) {
#if defined(_STK_DEBUG_)
    std::cerr << '\n' << message << '\n' << std::endl;
#endif
  }
  else {
    if ( printErrors_ ) {
      // Print error message before throwing.
      std::cerr << '\n' << message << '\n' << std::endl;
    }
    throw StkError(message, type);
  }
}

//
// StkFrames definitions
//

StkFrames :: StkFrames( unsigned int nFrames, unsigned int nChannels, bool interleaved )
  : nFrames_( nFrames ), nChannels_( nChannels ), interleaved_( interleaved )
{
  size_ = nFrames_ * nChannels_;
  bufferSize_ = size_;

  if ( size_ > 0 ) {
    data_ = (StkFloat *) calloc( size_, sizeof( StkFloat ) );
#if defined(_STK_DEBUG_)
    if ( data_ == NULL ) {
      std::string error = "StkFrames: memory allocation error in constructor!";
      Stk::handleError( error, StkError::MEMORY_ALLOCATION );
    }
#endif
  }
  else data_ = 0;

  dataRate_ = Stk::sampleRate();
}

StkFrames :: StkFrames( const StkFloat& value, unsigned int nFrames, unsigned int nChannels, bool interleaved )
  : nFrames_( nFrames ), nChannels_( nChannels ), interleaved_( interleaved )
{
  size_ = nFrames_ * nChannels_;
  bufferSize_ = size_;
  if ( size_ > 0 ) {
    data_ = (StkFloat *) malloc( size_ * sizeof( StkFloat ) );
#if defined(_STK_DEBUG_)
    if ( data_ == NULL ) {
      std::string error = "StkFrames: memory allocation error in constructor!";
      Stk::handleError( error, StkError::MEMORY_ALLOCATION );
    }
#endif
    for ( long i=0; i<(long)size_; i++ ) data_[i] = value;
  }
  else data_ = 0;

  dataRate_ = Stk::sampleRate();
}

StkFrames :: ~StkFrames()
{
  if ( data_ ) free( data_ );
}

bool StkFrames :: empty() const
{
  if ( size_ > 0 ) return false;
  else return true;
}

void StkFrames :: resize( size_t nFrames, unsigned int nChannels )
{
  nFrames_ = nFrames;
  nChannels_ = nChannels;

  size_ = nFrames_ * nChannels_;
  if ( size_ > bufferSize_ ) {
    if ( data_ ) free( data_ );
    data_ = (StkFloat *) malloc( size_ * sizeof( StkFloat ) );
#if defined(_STK_DEBUG_)
    if ( data_ == NULL ) {
      std::string error = "StkFrames::resize: memory allocation error!";
      Stk::handleError( error, StkError::MEMORY_ALLOCATION );
    }
#endif
    bufferSize_ = size_;
  }
}

void StkFrames :: resize( size_t nFrames, unsigned int nChannels, StkFloat value )
{
  this->resize( nFrames, nChannels );

  for ( size_t i=0; i<size_; i++ ) data_[i] = value;
}

StkFloat& StkFrames :: operator[] ( size_t n )
{
#if defined(_STK_DEBUG_)
    if ( n >= size_ ) {
      std::ostringstream error;
      error << "StkFrames::operator[]: invalid index (" << n << ") value!";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
#endif

  return data_[n];
}

StkFloat StkFrames :: operator[] ( size_t n ) const
{
#if defined(_STK_DEBUG_)
    if ( n >= size_ ) {
      std::ostringstream error;
      error << "StkFrames::operator[]: invalid index (" << n << ") value!";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
#endif

  return data_[n];
}

StkFloat& StkFrames :: operator() ( size_t frame, unsigned int channel )
{
#if defined(_STK_DEBUG_)
    if ( frame >= nFrames_ || channel >= nChannels_ ) {
      std::ostringstream error;
      error << "StkFrames::operator(): invalid frame (" << frame << ") or channel (" << channel << ") value!";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
#endif

  if ( interleaved_ )
    return data_[ frame * nChannels_ + channel ];
  else
    return data_[ channel * nFrames_ + frame ];
}

StkFloat StkFrames :: operator() ( size_t frame, unsigned int channel ) const
{
#if defined(_STK_DEBUG_)
    if ( frame >= nFrames_ || channel >= nChannels_ ) {
      std::ostringstream error;
      error << "StkFrames::operator(): invalid frame (" << frame << ") or channel (" << channel << ") value!";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
#endif

  if ( interleaved_ )
    return data_[ frame * nChannels_ + channel ];
  else
    return data_[ channel * nFrames_ + frame ];
}

StkFloat StkFrames :: interpolate( StkFloat frame, unsigned int channel ) const
{
#if defined(_STK_DEBUG_)
    if ( frame >= (StkFloat) nFrames_ || channel >= nChannels_ ) {
      std::ostringstream error;
      error << "StkFrames::interpolate: invalid frame (" << frame << ") or channel (" << channel << ") value!";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
#endif

  size_t iIndex = ( size_t ) frame;                    // integer part of index
  StkFloat output, alpha = frame - (StkFloat) iIndex;  // fractional part of index

  if ( interleaved_ ) {
    iIndex = iIndex * nChannels_ + channel;
    output = data_[ iIndex ];
    output += ( alpha * ( data_[ iIndex + nChannels_ ] - output ) );
  }
  else {
    iIndex += channel * nFrames_;
    output = data_[ iIndex ];
    output += ( alpha * ( data_[ ++iIndex ] - output ) );
  }

  return output;
}
