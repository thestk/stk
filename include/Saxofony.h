/***************************************************/
/*! \class Saxofony
    \brief STK faux conical bore reed instrument class.

    This class implements a "hybrid" digital
    waveguide instrument that can generate a
    variety of wind-like sounds.  It has also been
    referred to as the "blowed string" model.  The
    waveguide section is essentially that of a
    string, with one rigid and one lossy
    termination.  The non-linear function is a
    reed table.  The string can be "blown" at any
    point between the terminations, though just as
    with strings, it is impossible to excite the
    system at either end.  If the excitation is
    placed at the string mid-point, the sound is
    that of a clarinet.  At points closer to the
    "bridge", the sound is closer to that of a
    saxophone.  See Scavone (2002) for more details.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Reed Aperture = 26
       - Noise Gain = 4
       - Blow Position = 11
       - Vibrato Frequency = 29
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SAXOFONY_H)
#define __SAXOFONY_H

#include "Instrmnt.h"
#include "DelayL.h"
#include "ReedTabl.h"
#include "OneZero.h"
#include "Envelope.h"
#include "Noise.h"
#include "WaveLoop.h"

class Saxofony : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Saxofony(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Saxofony();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the "blowing" position between the air column terminations (0.0 - 1.0).
  void setBlowPosition(MY_FLOAT aPosition);

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath pressure with given rate of decrease.
  void stopBlowing(MY_FLOAT rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 protected:
  DelayL *delays[2];
  ReedTabl *reedTable;
  OneZero *filter;
  Envelope *envelope;
  Noise *noise;
  WaveLoop *vibrato;
  long length;
  MY_FLOAT outputGain;
  MY_FLOAT noiseGain;
  MY_FLOAT vibratoGain;
  MY_FLOAT position;

};

#endif
