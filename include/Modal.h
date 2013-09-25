/***************************************************/
/*! \class Modal
    \brief STK resonance model instrument.

    This class contains an excitation wavetable,
    an envelope, an oscillator, and N resonances
    (non-sweeping BiQuad filters), where N is set
    during instantiation.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__MODAL_H)
#define __MODAL_H

#include "Instrmnt.h"
#include "Envelope.h"
#include "WaveLoop.h"
#include "BiQuad.h"
#include "OnePole.h"

class Modal : public Instrmnt
{
public:
  //! Class constructor, taking the desired number of modes to create.
  Modal( int modes = 4 );

  //! Class destructor.
  virtual ~Modal();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Set the ratio and radius for a specified mode filter.
  void setRatioAndRadius(int modeIndex, MY_FLOAT ratio, MY_FLOAT radius);

  //! Set the master gain.
  void setMasterGain(MY_FLOAT aGain);

  //! Set the direct gain.
  void setDirectGain(MY_FLOAT aGain);

  //! Set the gain for a specified mode filter.
  void setModeGain(int modeIndex, MY_FLOAT gain);

  //! Initiate a strike with the given amplitude (0.0 - 1.0).
  virtual void strike(MY_FLOAT amplitude);

  //! Damp modes with a given decay factor (0.0 - 1.0).
  void damp(MY_FLOAT amplitude);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value) = 0;

protected:  
  Envelope *envelope; 
  WvIn     *wave;
  BiQuad   **filters;
  OnePole  *onepole;
  WaveLoop *vibrato;
  int nModes;
  MY_FLOAT vibratoGain;
  MY_FLOAT masterGain;
  MY_FLOAT directGain;
  MY_FLOAT stickHardness;
  MY_FLOAT strikePosition;
  MY_FLOAT baseFrequency;
  MY_FLOAT *ratios;
  MY_FLOAT *radii;

};

#endif
