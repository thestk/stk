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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__FM_H)
#define __FM_H

#include "Instrmnt.h"
#include "ADSR.h"
#include "WaveLoop.h"
#include "TwoZero.h"

class FM : public Instrmnt
{
 public:
  //! Class constructor, taking the number of wave/envelope operators to control.
  FM( int operators = 4 );

  //! Class destructor.
  virtual ~FM();

  //! Reset and clear all wave and envelope states.
  void clear();

  //! Load the rawwave filenames in waves.
  void loadWaves(const char **filenames);

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Set the frequency ratio for the specified wave.
  void setRatio(int waveIndex, MY_FLOAT ratio);

  //! Set the gain for the specified wave.
  void setGain(int waveIndex, MY_FLOAT gain);

  //! Set the modulation speed in Hz.
  void setModulationSpeed(MY_FLOAT mSpeed);

  //! Set the modulation depth.
  void setModulationDepth(MY_FLOAT mDepth);

  //! Set the value of control1.
  void setControl1(MY_FLOAT cVal);

  //! Set the value of control1.
  void setControl2(MY_FLOAT cVal);

  //! Start envelopes toward "on" targets.
  void keyOn();

  //! Start envelopes toward "off" targets.
  void keyOff();

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Pure virtual function ... must be defined in subclasses.
  virtual MY_FLOAT tick() = 0;

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

 protected:  
  ADSR     **adsr; 
  WaveLoop **waves;
  WaveLoop *vibrato;
  TwoZero  *twozero;
  int nOperators;
  MY_FLOAT baseFrequency;
  MY_FLOAT *ratios;
  MY_FLOAT *gains;
  MY_FLOAT modDepth;
  MY_FLOAT control1;
  MY_FLOAT control2;
  MY_FLOAT __FM_gains[100];
  MY_FLOAT __FM_susLevels[16];
  MY_FLOAT __FM_attTimes[32];

};

#endif
