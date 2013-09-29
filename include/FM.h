/***************************************************/
/*! \class FM
    \brief STK abstract FM synthesis base class.

    This class controls an arbitrary number of
    waves and envelopes, determined via a
    constructor argument.

    Control Change Numbers: 
       - Control One = 2
       - Control Two = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#ifndef STK_FM_H
#define STK_FM_H

#include "Instrmnt.h"
#include "ADSR.h"
#include "WaveLoop.h"
#include "SineWave.h"
#include "TwoZero.h"

class FM : public Instrmnt
{
 public:
  //! Class constructor, taking the number of wave/envelope operators to control.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  FM( unsigned int operators = 4 );

  //! Class destructor.
  virtual ~FM();

  //! Reset and clear all wave and envelope states.
  void clear();

  //! Load the rawwave filenames in waves.
  void loadWaves(const char **filenames);

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(StkFloat frequency);

  //! Set the frequency ratio for the specified wave.
  void setRatio(unsigned int waveIndex, StkFloat ratio);

  //! Set the gain for the specified wave.
  void setGain(unsigned int waveIndex, StkFloat gain);

  //! Set the modulation speed in Hz.
  void setModulationSpeed(StkFloat mSpeed);

  //! Set the modulation depth.
  void setModulationDepth(StkFloat mDepth);

  //! Set the value of control1.
  void setControl1(StkFloat cVal);

  //! Set the value of control1.
  void setControl2(StkFloat cVal);

  //! Start envelopes toward "on" targets.
  void keyOn();

  //! Start envelopes toward "off" targets.
  void keyOff();

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(StkFloat amplitude);

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, StkFloat value);

 protected:

  virtual StkFloat computeSample( void ) = 0;

  std::vector<ADSR *> adsr_; 
  std::vector<WaveLoop *> waves_;
  SineWave vibrato_;
  TwoZero  twozero_;
  unsigned int nOperators_;
  StkFloat baseFrequency_;
  std::vector<StkFloat> ratios_;
  std::vector<StkFloat> gains_;
  StkFloat modDepth_;
  StkFloat control1_;
  StkFloat control2_;
  StkFloat fmGains_[100];
  StkFloat fmSusLevels_[16];
  StkFloat fmAttTimes_[32];

};

#endif
