/***************************************************/
/*! \class VoicDrum
    \brief STK vocal drum sample player class.

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

#if !defined(__VOICDRUM_H)
#define __VOICDRUM_H

#include "Instrmnt.h"
#include "WvIn.h"
#include "OnePole.h"

#define VOICE_NUMWAVES 11
#define VOICE_POLYPHONY 4

class VoicDrum : public Instrmnt
{
 public:
  //! Class constructor.
  VoicDrum();

  //! Class destructor.
  ~VoicDrum();

  //! Start a note with the given drum type and amplitude.
  void noteOn(MY_FLOAT instrument, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

 protected:  
  WvIn    *waves[VOICE_POLYPHONY];
  OnePole *filters[VOICE_POLYPHONY];
  int      sounding[VOICE_POLYPHONY];
  int      nSounding;

};

#endif
