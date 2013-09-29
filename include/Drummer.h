/***************************************************/
/*! \class Drummer
    \brief STK drum sample player class.

    This class implements a drum sampling
    synthesizer using WvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) via a #define in the
    Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#ifndef STK_DRUMMER_H
#define STK_DRUMMER_H

#include "Instrmnt.h"
#include "FileWvIn.h"
#include "OnePole.h"

const int DRUM_NUMWAVES = 11;
const int DRUM_POLYPHONY = 4;

class Drummer : public Instrmnt
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Drummer();

  //! Class destructor.
  ~Drummer();

  //! Start a note with the given drum type and amplitude.
  /*!
    Use general MIDI drum instrument numbers, converted to
    frequency values as if MIDI note numbers, to select a particular
    instrument.  An StkError will be thrown if the rawwave path is
    incorrectly set.
  */
  void noteOn(StkFloat instrument, StkFloat amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(StkFloat amplitude);

 protected:

  StkFloat computeSample( void );

  FileWvIn waves_[DRUM_POLYPHONY];
  OnePole  filters_[DRUM_POLYPHONY];
  std::vector<int> soundOrder_;
  std::vector<int> soundNumber_;
  int      nSounding_;
};

#endif
