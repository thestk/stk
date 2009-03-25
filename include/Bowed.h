/***************************************************/
/*! \class Bowed
    \brief STK bowed string instrument class.

    This class implements a bowed string model, a
    la Smith (1986), after McIntyre, Schumacher,
    Woodhouse (1983).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Position = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_BOWED_H
#define STK_BOWED_H

#include "Instrmnt.h"
#include "DelayL.h"
#include "BowTable.h"
#include "OnePole.h"
#include "BiQuad.h"
#include "WaveLoop.h"
#include "ADSR.h"

class Bowed : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Bowed(StkFloat lowestFrequency);

  //! Class destructor.
  ~Bowed();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Set vibrato gain.
  void setVibrato(StkFloat gain);

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBowing(StkFloat amplitude, StkFloat rate);

  //! Decrease breath pressure with given rate of decrease.
  void stopBowing(StkFloat rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(StkFloat frequency, StkFloat amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(StkFloat amplitude);

  //! Compute one output sample.
  StkFloat tick();

  //! Computer \e vectorSize outputs and return them in \e vector.
  StkFloat *tick(StkFloat *vector, unsigned int vectorSize);

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, StkFloat value);

 protected:  
  DelayL   neckDelay_;
  DelayL   bridgeDelay_;
  BowTable bowTable_;
  OnePole  stringFilter_;
  BiQuad   bodyFilter_;
  WaveLoop *vibrato_;
  ADSR     adsr_;
  StkFloat maxVelocity_;
  StkFloat baseDelay_;
  StkFloat vibratoGain_;
  StkFloat betaRatio_;

};

#endif
