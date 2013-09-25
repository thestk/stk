/***************************************************/
/*! \class Tabla
    \brief STK tabla drum class.

    This class implements a drum sampling
    synthesizer using WvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) via a #define in the
    Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__TABLA_H)
#define __TABLA_H

#include "Instrmnt.h"
#include "WvIn.h"
#include "OnePole.h"

#define TABLA_NUMWAVES 15
#define TABLA_POLYPHONY 4

class Tabla : public Instrmnt
{
 public:
  //! Class constructor.
  Tabla();

  //! Class destructor.
  ~Tabla();

  //! Start a note with the given drum type and amplitude.
  void noteOn(MY_FLOAT instrument, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

 protected:  
  WvIn    *waves[TABLA_POLYPHONY];
  OnePole *filters[TABLA_POLYPHONY];
  int      sounding[TABLA_POLYPHONY];
  int      nSounding;

};

#endif
