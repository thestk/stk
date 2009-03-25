/***************************************************/
/*! \class BlitSaw
    \brief STK band-limited sawtooth wave class.

    This class generates a band-limited sawtooth waveform using a
    closed-form algorithm reported by Stilson and Smith in "Alias-Free
    Digital Synthesis of Classic Analog Waveforms", 1996.  The user
    can specify both the fundamental frequency of the sawtooth and the
    number of harmonics contained in the resulting signal.

    If nHarmonics is 0, then the signal will contain all harmonics up
    to half the sample rate.  Note, however, that this setting may
    produce aliasing in the signal when the frequency is changing (no
    automatic modification of the number of harmonics is performed by
    the setFrequency() function).

    Based on initial code of Robin Davies, 2005.
    Modified algorithm code by Gary Scavone, 2005.
*/
/***************************************************/

#ifndef STK_BLITSAW_H
#define STK_BLITSAW_H

#include "Generator.h"

class BlitSaw: public Generator
{
 public:
  //! Class constructor.
  BlitSaw( StkFloat frequency = 220.0 );

  //! Class destructor.
  ~BlitSaw();

  //! Resets the oscillator state and phase to 0.
  void reset();

  //! Set the sawtooth oscillator rate in terms of a frequency in Hz.
  void setFrequency( StkFloat frequency );

  //! Set the number of harmonics generated in the signal.
  /*!
    This function sets the number of harmonics contained in the
    resulting signal.  It is equivalent to (2 * M) + 1 in the BLIT
    algorithm.  The default value of 0 sets the algorithm for maximum
    harmonic content (harmonics up to half the sample rate).  This
    parameter is not checked against the current sample rate and
    fundamental frequency.  Thus, aliasing can result if one or more
    harmonics for a given fundamental frequency exceeds fs / 2.  This
    behavior was chosen over the potentially more problematic solution
    of automatically modifying the M parameter, which can produce
    audible clicks in the signal.
  */
  void setHarmonics( unsigned int nHarmonics = 0 );

 protected:

  void updateHarmonics( void );
  StkFloat computeSample( void );

  unsigned int nHarmonics_;
  unsigned int m_;
  StkFloat rate_;
  StkFloat phase_;
  StkFloat p_;
  StkFloat C2_;
  StkFloat a_;
  StkFloat state_;

};

#endif
