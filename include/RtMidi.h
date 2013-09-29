/***************************************************/
/*! \class RtMidi
    \brief STK realtime MIDI class.

    At the moment, this object only handles MIDI
    input, though MIDI output code can go here
    when someone decides they need it (and writes
    it).

    This object opens a MIDI input device and
    parses MIDI messages into a MIDI buffer.  Time
    stamp info is converted to a delta-time
    value. MIDI data is stored as MY_FLOAT to
    conform with SKINI.  System exclusive messages
    are currently ignored.

    An optional argument to the constructor can be
    used to specify a device or card.  When no
    argument is given, a default device is opened.
    If a device argument fails, a list of available
    devices is printed to allow selection by the user.

    This code is based in part on work of Perry
    Cook (SGI), Paul Leonard (Linux), the
    RoseGarden team (Linux), and Bill Putnam
    (Windows).

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__RTMIDI_H)
#define __RTMIDI_H

#include "Stk.h"

class RtMidi : public Stk
{
 public:
  //! Default constructor with optional device argument.
  RtMidi(int device = 0);

  //! Class destructor.
  ~RtMidi();

  //! Print out the current message values.
  void printMessage(void) const;

  //! Check for and parse a new MIDI message in the queue, returning its type.
  /*!
    If a new message is found, the return value is greater than zero.
  */
  int nextMessage(void);

  //! Return the current message type.
  int getType() const;

  //! Return the current message channel value.
  int getChannel() const;

  //! Return the current message byte two value.
  MY_FLOAT getByteTwo() const;

  //! Return the current message byte three value.
  MY_FLOAT getByteThree() const;

  //! Return the current message delta time value in seconds.
	MY_FLOAT getDeltaTime() const;

 protected:
  int messageType;
  int channel;
  float byteTwo;
  float byteThree;
	MY_FLOAT deltaTime;
  int readIndex;

};

#endif
