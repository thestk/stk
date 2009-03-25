/***************************************************/
/*! \class Whistle
    \brief STK police/referee whistle instrument class.

    This class implements a hybrid physical/spectral
    model of a police whistle (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Fipple Modulation Frequency = 11
       - Fipple Modulation Gain = 1
       - Blowing Frequency Modulation = 2
       - Volume = 128

    by Perry R. Cook  1996 - 2004.
*/
/***************************************************/

#ifndef STK_WHISTLE_H
#define STK_WHISTLE_H

#include "Instrmnt.h"
#include "Sphere.h"
#include "Vector3D.h"
#include "Noise.h"
#include "WaveLoop.h"
#include "OnePole.h"
#include "Envelope.h"

class Whistle : public Instrmnt
{
public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Whistle();

  //! Class destructor.
  ~Whistle();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing(StkFloat amplitude, StkFloat rate);

  //! Decrease breath velocity with given rate of decrease.
  void stopBlowing(StkFloat rate);

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

	Vector3D *tempVectorP_;
  Vector3D tempVector_;
  OnePole onepole_;
  Noise noise_;
	Envelope envelope_;
  Sphere can_;           // Declare a Spherical "can".
  Sphere pea_, bumper_;  // One spherical "pea", and a spherical "bumper".

  WaveLoop *sine_;

  StkFloat baseFrequency_;
  StkFloat noiseGain_;
  StkFloat fippleFreqMod_;
	StkFloat fippleGainMod_;
	StkFloat blowFreqMod_;
	StkFloat tickSize_;
	StkFloat canLoss_;
	int subSample_, subSampCount_;
};

#endif
