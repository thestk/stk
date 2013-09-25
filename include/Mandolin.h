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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__MANDOLIN_H)
#define __MANDOLIN_H

#include "PluckTwo.h"
#include "WvIn.h"

class Mandolin : public PluckTwo
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Mandolin(MY_FLOAT lowestFrequency);

  //! Class destructor.
  virtual ~Mandolin();

  //! Pluck the strings with the given amplitude (0.0 - 1.0) using the current frequency.
  void pluck(MY_FLOAT amplitude);

  //! Pluck the strings with the given amplitude (0.0 - 1.0) and position (0.0 - 1.0).
  void pluck(MY_FLOAT amplitude,MY_FLOAT position);

  //! Start a note with the given frequency and amplitude (0.0 - 1.0).
  virtual void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Set the body size (a value of 1.0 produces the "default" size).
  void setBodySize(MY_FLOAT size);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

  protected:  
    WvIn *soundfile[12];
    MY_FLOAT directBody;
    int mic;
    long dampTime;
    bool waveDone;
};

#endif
