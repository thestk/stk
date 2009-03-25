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

    by Perry R. Cook  1996 - 2002.
*/
/***************************************************/

#if !defined(__WHISTLE_H)
#define __WHISTLE_H

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
  Whistle();

  //! Class destructor.
  ~Whistle();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath velocity with given rate of decrease.
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
	Vector3D *tempVectorP;
  Vector3D *tempVector;
  OnePole onepole;
  Noise noise;
	Envelope envelope;
  Sphere *can;           // Declare a Spherical "can".
  Sphere *pea, *bumper;  // One spherical "pea", and a spherical "bumper".

  WaveLoop *sine;

  MY_FLOAT baseFrequency;
	MY_FLOAT maxPressure;
  MY_FLOAT noiseGain;
  MY_FLOAT fippleFreqMod;
	MY_FLOAT fippleGainMod;
	MY_FLOAT blowFreqMod;
	MY_FLOAT tickSize;
	MY_FLOAT canLoss;
	int subSample, subSampCount;
};

#endif
