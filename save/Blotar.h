/***************************************************/
/*! \class Blotar
    \brief STK Blotar class.

    This class is a port of Dan Trueman's MSP/PeRColate "blotar"
    instrument, which itself was a modified version of the STK
    waveguide flute model.

    This port was made by Simon de Leon in the Music Technology area
    at McGill University in 2006.

    This is a digital waveguide model, making its use possibly subject
    to patents held by Stanford University, Yamaha, and others.  There
    exist at least two patents, assigned to Stanford, bearing the
    names of Karplus and/or Strong.

    Control Change Numbers: 
       - Jet Delay = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#ifndef STK_BLOTAR_H
#define STK_BLOTAR_H

#define WATCHIT 0.00001

#include "Instrmnt.h"
#include "DelayL.h"
#include "DelayA.h"
#include "OneZero.h"
#include "OnePole.h"
#include "PoleZero.h"
#include "Noise.h"
#include "WaveLoop.h"
#include "JetTable.h"
#include "SineWave.h"
#include <cmath>

class Blotar : public Instrmnt
{
 public:

	//! Default class consructor, default lowest frequency is 100 Hz 
  Blotar();

  //! Overloaded class constructor, taking the lowest desired playing frequency.
  Blotar( StkFloat lowestFrequency );

  //! Class destructor.
  ~Blotar();

  //! Reset and clear all internal state.
  void clear();

  //! Start a note with the given frequency and amplitude.
  virtual void noteOn(StkFloat frequency, StkFloat amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(StkFloat amplitude);

  //! Set boreDelay_ line length parameters for a particular frequency
  virtual void setFrequency(StkFloat frequency);

  //! Set combDelay_ line length parameters to filter impulse response for particular position
  void setPluckPosition(StkFloat position);

  //! Set mandolin impulse response playback rate
  void setBodySize(StkFloat size);

  //! Set breath pressure
  void setBreathPressure(StkFloat bp);

  //! Set jetDelay_ line length parameter
  void setJetDelay(StkFloat frequency);

  //! Set gain of random breath pressure component
  void setNoiseGain(StkFloat noiseGain);

  //! Set read rate from sinusoidal table, which is used to scale random breath pressure component
  void setVibatoFrequency( StkFloat frequency );

  //! Set amplitude of sinusoidal table used to scale random breath pressure component
  void setVibratoGain( StkFloat gain );

  //! Set the reflection coefficient for the jet delay (-1.0 - 1.0)
  void setJetReflection(StkFloat jetReflection);

  //! Set the reflection coefficient for the air column delay (-1.0 - 1.0)
  void setEndReflection(StkFloat endReflection);

  //! Interpolates between one-pole filter of the flute and lowpass filter of plucked string
  void setFilterRatio(StkFloat filterRatio);

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, StkFloat value);

 protected:  
 
  void initialize( StkFloat lowestFrequency );
  StkFloat computeSample( void );
  void pluck( StkFloat amplitude );

  DelayL boreDelay_;
  DelayL jetDelay_;
  DelayL combDelay_;
  PoleZero dcBlock_;
  OnePole fluteFilter_;
  OneZero lowpass_;
  Noise noise_;
  JetTable jetTable_;
  SineWave vibrato_;
  FileWvIn *soundfile_[12];

  // Performance variables
  StkFloat pluckAmplitude_;
  StkFloat pluckPosition_;
  StkFloat maxPressure_;    // breath pressure	
  StkFloat noiseGain_;
  StkFloat vibratoGain_;
  StkFloat jetReflection_;
  StkFloat endReflection_;
  StkFloat filterRatio_;    // one-zero vs. one-pole filter ratio
  StkFloat jetRatio_;    // one-zero vs. one-pole filter ratio

  int mic_;                 // directional position (NBody)
  bool waveDone_;

  long length_;
  StkFloat lastFrequency_;
};

#endif
