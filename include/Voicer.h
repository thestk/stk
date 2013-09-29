/***************************************************/
/*! \class Voicer
    \brief STK voice manager class.

    This class can be used to manage a group of
    STK instrument classes.  Individual voices can
    be controlled via unique note tags.
    Instrument groups can be controlled by channel
    number.

    A previously constructed STK instrument class
    is linked with a voice manager using the
    addInstrument() function.  An optional channel
    number argument can be specified to the
    addInstrument() function as well (default
    channel = 0).  The voice manager does not
    delete any instrument instances ... it is the
    responsibility of the user to allocate and
    deallocate all instruments.

    The tick() function returns the mix of all
    sounding voices.  Each noteOn returns a unique
    tag (credits to the NeXT MusicKit), so you can
    send control changes to specific voices within
    an ensemble.  Alternately, control changes can
    be sent to all voices on a given channel.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__VOICER_H)
#define __VOICER_H

#include "Stk.h"
#include "Instrmnt.h"

class Voicer : public Stk
{
public:
  //! Class constructor taking the maximum number of instruments to control and an optional note decay time (in seconds).
  Voicer( int maxInstruments, MY_FLOAT decayTime=0.2 );

  //! Class destructor.
  ~Voicer();

  //! Add an instrument with an optional channel number to the voice manager.
  /*!
    A set of instruments can be grouped by channel number and
    controlled via the functions which take a channel number argument.
  */
  void addInstrument( Instrmnt *instrument, int channel=0 );

  //! Remove the given instrument pointer from the voice manager's control.
  /*!
    It is important that any instruments which are to be deleted by
    the user while the voice manager is running be first removed from
    the manager's control via this function!!
   */
  void removeInstrument( Instrmnt *instrument );

  //! Initiate a noteOn event with the given note number and amplitude and return a unique note tag.
  /*!
    Send the noteOn message to the first available unused voice.
    If all voices are sounding, the oldest voice is interrupted and
    sent the noteOn message.  If the optional channel argument is
    non-zero, only voices on that channel are used.  If no voices are
    found for a specified non-zero channel value, the function returns
    -1.  The amplitude value should be in the range 0.0 - 128.0.
  */
  long noteOn( MY_FLOAT noteNumber, MY_FLOAT amplitude, int channel=0 );

  //! Send a noteOff to all voices having the given noteNumber and optional channel (default channel = 0).
  /*!
    The amplitude value should be in the range 0.0 - 128.0.
  */
  void noteOff( MY_FLOAT noteNumber, MY_FLOAT amplitude, int channel=0 );

  //! Send a noteOff to the voice with the given note tag.
  /*!
    The amplitude value should be in the range 0.0 - 128.0.
  */
  void noteOff( long tag, MY_FLOAT amplitude );

  //! Send a frequency update message to all voices assigned to the optional channel argument (default channel = 0).
  /*!
    The \e noteNumber argument corresponds to a MIDI note number, though it is a floating-point value and can range beyond the normal 0-127 range.
   */
  void setFrequency( MY_FLOAT noteNumber, int channel=0 );

  //! Send a frequency update message to the voice with the given note tag.
  /*!
    The \e noteNumber argument corresponds to a MIDI note number, though it is a floating-point value and can range beyond the normal 0-127 range.
   */
  void setFrequency( long tag, MY_FLOAT noteNumber );

  //! Send a pitchBend message to all voices assigned to the optional channel argument (default channel = 0).
  void pitchBend( MY_FLOAT value, int channel=0 );

  //! Send a pitchBend message to the voice with the given note tag.
  void pitchBend( long tag, MY_FLOAT value );

  //! Send a controlChange to all instruments assigned to the optional channel argument (default channel = 0).
  void controlChange( int number, MY_FLOAT value, int channel=0 );

  //! Send a controlChange to the voice with the given note tag.
  void controlChange( long tag, int number, MY_FLOAT value );

  //! Send a noteOff message to all existing voices.
  void silence( void );

  //! Mix the output for all sounding voices.
  MY_FLOAT tick();

  //! Compute \e vectorSize output mixes and return them in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the last left output value.
  MY_FLOAT lastOutLeft() const;

  //! Return the last right output value.
  MY_FLOAT lastOutRight() const;

protected:

  typedef struct {
    Instrmnt *instrument;
    long tag;
    MY_FLOAT noteNumber;
    MY_FLOAT frequency;
    int sounding;
    int channel;
  } Voice;

  int  nVoices;
  int maxVoices;
  Voice *voices;
  long tags;
  int muteTime;
  MY_FLOAT lastOutput;
  MY_FLOAT lastOutputLeft;
  MY_FLOAT lastOutputRight;
};

#endif
