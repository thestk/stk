/***************************************************/
/*! \class BandedWG
    \brief Banded waveguide modeling class.

    This class uses banded waveguide techniques to
    model a variety of sounds, including bowed
    bars, glasses, and bowls.  For more
    information, see Essl, G. and Cook, P. "Banded
    Waveguides: Towards Physical Modelling of Bar
    Percussion Instruments", Proceedings of the
    1999 International Computer Music Conference.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Motion = 4
       - Strike Position = 8 (not implemented)
       - Vibrato Frequency = 11
       - Gain = 1
       - Bow Velocity = 128
       - Set Striking = 64
       - Instrument Presets = 16
         - Uniform Bar = 0
         - Tuned Bar = 1
         - Glass Harmonica = 2
         - Tibetan Bowl = 3

    by Georg Essl, 1999 - 2002.
    Modified for Stk 4.0 by Gary Scavone.
*/
/***************************************************/

#if !defined(__BANDEDWG_H)
#define __BANDEDWG_H

#define MAX_BANDED_MODES 17

#include "Instrmnt.h"
#include "Delay.h"
#include "BowTabl.h"
#include "ADSR.h"
#include "BiQuad.h"

class BandedWG : public Instrmnt
{
 public:
  //! Class constructor.
  BandedWG();

  //! Class destructor.
  ~BandedWG();

  //! Reset and clear all internal state.
  void clear();

  //! Set strike position (0.0 - 1.0).
  void setStrikePosition(MY_FLOAT position);

  //! Select a preset.
  void setPreset(int preset);

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Apply bow velocity/pressure to instrument with given amplitude and rate of increase.
  void startBowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease bow velocity/breath pressure with given rate of decrease.
  void stopBowing(MY_FLOAT rate);

  //! Pluck the instrument with given amplitude.
  void pluck(MY_FLOAT amp);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 protected:

  bool doPluck;
  bool trackVelocity;
  int nModes;
  int presetModes;
  BowTabl *bowTabl;
  ADSR *adsr;
  BiQuad *bandpass;
  Delay *delay;
  MY_FLOAT maxVelocity;
  MY_FLOAT modes[MAX_BANDED_MODES];
  MY_FLOAT freakency;
  MY_FLOAT baseGain;
  MY_FLOAT gains[MAX_BANDED_MODES];
  MY_FLOAT integrationConstant;
  MY_FLOAT bowVelocity;
  MY_FLOAT bowTarget;
  MY_FLOAT bowPosition;
  int strikePosition;

};

#endif
