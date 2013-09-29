/***************************************************/
/*! \class Skini
    \brief STK SKINI parsing class

    This class parses SKINI formatted text
    messages.  It can be used to parse individual
    messages or it can be passed an entire file.
    The SKINI specification is Perry's and his
    alone, but it's all text so it shouldn't be too
    hard to figure out.

    SKINI (Synthesis toolKit Instrument Network
    Interface) is like MIDI, but allows for
    floating-point control changes, note numbers,
    etc.  The following example causes a sharp
    middle C to be played with a velocity of 111.132:

    \code
    noteOn  60.01  111.132
    \endcode

    \sa \ref skini

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_SKINI_H
#define STK_SKINI_H

#include "Stk.h"
#include <vector>
#include <string>
#include <fstream>

class Skini : public Stk
{
 public:

  //! A message structure to store and pass parsed SKINI messages.
  struct Message { 
    long type;                         /*!< The message type, as defined in SKINI.msg. */
    long channel;                      /*!< The message channel (not limited to 16!). */
    StkFloat time;                     /*!< The message time stamp in seconds (delta or absolute). */
    std::vector<StkFloat> floatValues; /*!< The message values read as floats (values are type-specific). */
    std::vector<long> intValues;       /*!< The message values read as ints (number and values are type-specific). */
    std::string remainder;             /*!< Any remaining message data, read as ascii text. */

    // Default constructor.
    Message()
      :type(0), channel(0), time(0.0), floatValues(2), intValues(2) {}
  };

  //! Default constructor.
  Skini();

  //! Class destructor
  ~Skini();

  //! Set a SKINI formatted file for reading.
  /*!
    If the file is successfully opened, this function returns \e
    true.  Otherwise, \e false is returned.
   */
  bool setFile( std::string fileName );

  //! Parse the next file message (if a file is loaded) and return the message type.
  /*!
    This function skips over lines in a file which cannot be
    parsed.  A type value equal to zero in the referenced message
    structure (and the returned value) indicates the file end is
    reached or no file is open for reading.
  */
  long nextMessage( Skini::Message& message );

  //! Attempt to parse the given string and returning the message type.
  /*!
    A type value equal to zero in the referenced message structure
    indicates an invalid message.
  */
  long parseString( std::string& line, Skini::Message& message );

  //! Return the SKINI type string for the given type value.
  static std::string whatsThisType(long type);

  //! Return the SKINI controller string for the given controller number.
  static std::string whatsThisController(long number);

 protected:

  void tokenize( const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters );

  std::ifstream file_;
};

static const double Midi2Pitch[129] = {
8.18,8.66,9.18,9.72,10.30,10.91,11.56,12.25,
12.98,13.75,14.57,15.43,16.35,17.32,18.35,19.45,
20.60,21.83,23.12,24.50,25.96,27.50,29.14,30.87,
32.70,34.65,36.71,38.89,41.20,43.65,46.25,49.00,
51.91,55.00,58.27,61.74,65.41,69.30,73.42,77.78,
82.41,87.31,92.50,98.00,103.83,110.00,116.54,123.47,
130.81,138.59,146.83,155.56,164.81,174.61,185.00,196.00,
207.65,220.00,233.08,246.94,261.63,277.18,293.66,311.13,
329.63,349.23,369.99,392.00,415.30,440.00,466.16,493.88,
523.25,554.37,587.33,622.25,659.26,698.46,739.99,783.99,
830.61,880.00,932.33,987.77,1046.50,1108.73,1174.66,1244.51,
1318.51,1396.91,1479.98,1567.98,1661.22,1760.00,1864.66,1975.53,
2093.00,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,
3322.44,3520.00,3729.31,3951.07,4186.01,4434.92,4698.64,4978.03,
5274.04,5587.65,5919.91,6271.93,6644.88,7040.00,7458.62,7902.13,
8372.02,8869.84,9397.27,9956.06,10548.08,11175.30,11839.82,12543.85,
13289.75};

#endif


