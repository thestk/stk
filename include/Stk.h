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

#if !defined(__STK_H)
#define __STK_H

// Most data in STK is passed and calculated with the following
// user-definable floating-point type.  You can change this to "float"
// if you prefer or perhaps a "long double" in the future.
typedef double MY_FLOAT;

//! STK error handling class.
/*!
  This is a fairly abstract exception handling class.  There could
  be sub-classes to take care of more specific error conditions ... or
  not.
*/
class StkError
{
public:
  enum TYPE { 
    WARNING,
    DEBUG_WARNING,
    FUNCTION_ARGUMENT,
    FILE_NOT_FOUND,
    FILE_UNKNOWN_FORMAT,
    FILE_ERROR,
    PROCESS_THREAD,
    PROCESS_SOCKET,
    PROCESS_SOCKET_IPADDR,
    AUDIO_SYSTEM,
    MIDI_SYSTEM,
    UNSPECIFIED
  };

protected:
  char message[256];
  TYPE type;

public:
  //! The constructor.
  StkError(const char *p, TYPE tipe = StkError::UNSPECIFIED);

  //! The destructor.
  virtual ~StkError(void);

  //! Prints "thrown" error message to stdout.
  virtual void printMessage(void);

  //! Returns the "thrown" error message TYPE.
  virtual const TYPE& getType(void) { return type; }

  //! Returns the "thrown" error message string.
  virtual const char *getMessage(void) const { return message; }
};


class Stk
{
 public:

  typedef unsigned long STK_FORMAT;
  static const STK_FORMAT STK_SINT8;   /*!< -128 to +127 */
  static const STK_FORMAT STK_SINT16;  /*!< -32768 to +32767 */
  static const STK_FORMAT STK_SINT32;  /*!< -2147483648 to +2147483647. */
  static const STK_FORMAT STK_FLOAT32; /*!< Normalized between plus/minus 1.0. */
  static const STK_FORMAT STK_FLOAT64; /*!< Normalized between plus/minus 1.0. */

  //! Static method which returns the current STK sample rate.
  static MY_FLOAT sampleRate(void);

  //! Static method which sets the STK sample rate.
  /*!
    The sample rate set using this method is queried by all STK
    classes which depend on its value.  It is initialized to the
    default SRATE set in Stk.h.  Many STK classes use the sample rate
    during instantiation.  Therefore, if you wish to use a rate which
    is different from the default rate, it is imperative that it be
    set \e BEFORE STK objects are instantiated.
   */
  static void setSampleRate(MY_FLOAT newRate);

  //! Static method which byte-swaps a 16-bit data type.
  static void swap16(unsigned char *ptr);

  //! Static method which byte-swaps a 32-bit data type.
  static void swap32(unsigned char *ptr);

  //! Static method which byte-swaps a 64-bit data type.
  static void swap64(unsigned char *ptr);

  //! Static cross-platform method to sleep for a number of milliseconds.
  static void sleep(unsigned long milliseconds);

 private:
  static MY_FLOAT srate;

 protected:

  //! Default constructor.
  Stk(void);

  //! Class destructor.
  virtual ~Stk(void);

  //! Function for error reporting and handling.
  static void handleError( const char *message, StkError::TYPE type );

};

// Here are a few other useful typedefs.
typedef signed short SINT16;
typedef signed int SINT32;
typedef float FLOAT32;
typedef double FLOAT64;

// Boolean values
#define FALSE 0
#define TRUE 1

// The default sampling rate.
#define SRATE (MY_FLOAT) 22050.0

// Real-time audio input and output buffer size.  If clicks are
// occuring in the input and/or output sound stream, a larger buffer
// size may help.  Larger buffer sizes, however, produce more latency.

#define RT_BUFFER_SIZE 512

// The RAWWAVE_PATH definition is concatenated to the beginning of all
// references to rawwave files in the various STK core classes
// (ex. Clarinet.cpp).  If you wish to move the rawwaves directory to
// a different location in your file system, you will need to set this
// path definition appropriately.  The current definition is a
// relative reference that will work "out of the box" for the STK
// distribution.
#define RAWWAVE_PATH "../../"

#define PI (MY_FLOAT) 3.14159265359
#define TWO_PI (MY_FLOAT) (MY_FLOAT) (2 * PI)

#define ONE_OVER_128 (MY_FLOAT) 0.0078125

#if defined(__WINDOWS_DS__)
  #define __OS_WINDOWS__
  #define __STK_REALTIME__
#elif defined(__LINUX_OSS__) || defined(__LINUX_ALSA__)
  #define __OS_LINUX__
  #define __STK_REALTIME__
#elif defined(__IRIX_AL__)
  #define __OS_IRIX__
  #define __STK_REALTIME__
#endif

//#define _STK_DEBUG_

#endif
