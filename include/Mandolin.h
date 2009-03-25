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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_MANDOLIN_H
#define STK_MANDOLIN_H

#include "PluckTwo.h"
#include "WvIn.h"

class Mandolin : public PluckTwo
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Mandolin(StkFloat lowestFrequency);

  //! Class destructor.
  ~Mandolin();

  //! Pluck the strings with the given amplitude (0.0 - 1.0) using the current frequency.
  void pluck(StkFloat amplitude);

  //! Pluck the strings with the given amplitude (0.0 - 1.0) and position (0.0 - 1.0).
  void pluck(StkFloat amplitude,StkFloat position);

  //! Start a note with the given frequency and amplitude (0.0 - 1.0).
  void noteOn(StkFloat frequency, StkFloat amplitude);

  //! Set the body size (a value of 1.0 produces the "default" size).
  void setBodySize(StkFloat size);

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
    WvIn *soundfile_[12];
    int mic_;
    long dampTime_;
    bool waveDone_;
};

#endif
