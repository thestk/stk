#ifndef STK_MANDOLIN_H
#define STK_MANDOLIN_H

#include "PluckTwo.h"
#include "FileWvIn.h"

namespace stk {

/***************************************************/
/*! \class Mandolin
    \brief STK mandolin instrument model class.

    This class inherits from PluckTwo and uses
    "commuted synthesis" techniques to model a
    mandolin instrument.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    Commuted Synthesis, in particular, is covered
    by patents, granted, pending, and/or
    applied-for.  All are assigned to the Board of
    Trustees, Stanford University.  For
    information, contact the Office of Technology
    Licensing, Stanford University.

    Control Change Numbers: 
       - Body Size = 2
       - Pluck Position = 4
       - String Sustain = 11
       - String Detuning = 1
       - Microphone Position = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

class Mandolin : public PluckTwo
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Mandolin( StkFloat lowestFrequency );

  //! Class destructor.
  ~Mandolin( void );

  //! Pluck the strings with the given amplitude (0.0 - 1.0) using the current frequency.
  void pluck( StkFloat amplitude );

  //! Pluck the strings with the given amplitude (0.0 - 1.0) and position (0.0 - 1.0).
  void pluck( StkFloat amplitude,StkFloat position );

  //! Start a note with the given frequency and amplitude (0.0 - 1.0).
  void noteOn( StkFloat frequency, StkFloat amplitude );

  //! Set the body size (a value of 1.0 produces the "default" size).
  void setBodySize( StkFloat size );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange( int number, StkFloat value );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

  protected:

  FileWvIn *soundfile_[12];
  int mic_;
  long dampTime_;
  bool waveDone_;
};

inline StkFloat Mandolin :: tick( unsigned int )
{
  StkFloat temp = 0.0;
  if ( !waveDone_ ) {
    // Scale the pluck excitation with comb
    // filtering for the duration of the file.
    temp = soundfile_[mic_]->tick() * pluckAmplitude_;
    temp = temp - combDelay_.tick(temp);
    waveDone_ = soundfile_[mic_]->isFinished();
  }

  // Damping hack to help avoid overflow on re-plucking.
  if ( dampTime_ >=0 ) {
    dampTime_ -= 1;
    // Calculate 1st delay filtered reflection plus pluck excitation.
    lastFrame_[0] = delayLine_.tick( filter_.tick( temp + (delayLine_.lastOut() * 0.7) ) );
    // Calculate 2nd delay just like the 1st.
    lastFrame_[0] += delayLine2_.tick( filter2_.tick( temp + (delayLine2_.lastOut() * 0.7) ) );
  }
  else { // No damping hack after 1 period.
    // Calculate 1st delay filtered reflection plus pluck excitation.
    lastFrame_[0] = delayLine_.tick( filter_.tick( temp + (delayLine_.lastOut() * loopGain_) ) );
    // Calculate 2nd delay just like the 1st.
    lastFrame_[0] += delayLine2_.tick( filter2_.tick( temp + (delayLine2_.lastOut() * loopGain_) ) );
  }

  lastFrame_[0] *= 0.3;
  return lastFrame_[0];
}

} // stk namespace

#endif
