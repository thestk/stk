/***************************************************/
/*! \class Tabla
    \brief STK tabla drum class.

    This class implements a drum sampling
    synthesizer using WvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) in Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_TABLA_H
#define STK_TABLA_H

#include "Instrmnt.h"
#include "WvIn.h"
#include "OnePole.h"

const int TABLA_NUMWAVES = 15;
const int TABLA_POLYPHONY = 4;

class Tabla : public Instrmnt
{
 public:
  //! Class constructor.
  Tabla();

  //! Class destructor.
  ~Tabla();

  //! Start a note with the given drum type and amplitude.
  void noteOn(StkFloat instrument, StkFloat amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(StkFloat amplitude);

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

 protected:  
  WvIn    *waves_[TABLA_POLYPHONY];
  OnePole *filters_[TABLA_POLYPHONY];
  int      sounding_[TABLA_POLYPHONY];
  int      nSounding_;

};

#endif
