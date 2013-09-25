/***************************************************/
/*! \class BlowHole
    \brief STK clarinet physical model with one
           register hole and one tonehole.

    This class is based on the clarinet model,
    with the addition of a two-port register hole
    and a three-port dynamic tonehole
    implementation, as discussed by Scavone and
    Cook (1998).

    In this implementation, the distances between
    the reed/register hole and tonehole/bell are
    fixed.  As a result, both the tonehole and
    register hole will have variable influence on
    the playing frequency, which is dependent on
    the length of the air column.  In addition,
    the highest playing freqeuency is limited by
    these fixed lengths.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Tonehole State = 11
       - Register State = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BLOWHOLE_H)
#define __BLOWHOLE_H

#include "Instrmnt.h"
#include "DelayL.h"
#include "ReedTabl.h"
#include "OneZero.h"
#include "PoleZero.h"
#include "Envelope.h"
#include "Noise.h"
#include "WaveLoop.h"

class BlowHole : public Instrmnt
{
 public:
  //! Class constructor.
  BlowHole(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~BlowHole();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the tonehole state (0.0 = closed, 1.0 = fully open).
  void setTonehole(MY_FLOAT newValue);

  //! Set the register hole state (0.0 = closed, 1.0 = fully open).
  void setVent(MY_FLOAT newValue);

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
  DelayL *delays[3];
  ReedTabl *reedTable;
  OneZero *filter;
  PoleZero *tonehole;
  PoleZero *vent;
  Envelope *envelope;
  Noise *noise;
  WaveLoop *vibrato;
  long length;
  MY_FLOAT scatter;
  MY_FLOAT th_coeff;
  MY_FLOAT r_th;
  MY_FLOAT rh_coeff;
  MY_FLOAT rh_gain;
  MY_FLOAT outputGain;
  MY_FLOAT noiseGain;
  MY_FLOAT vibratoGain;

};

#endif
