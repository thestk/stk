#ifndef STK_DRUMMER_H
#define STK_DRUMMER_H

#include "Instrmnt.h"
#include "FileWvIn.h"
#include "OnePole.h"

namespace stk {

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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

const int DRUM_NUMWAVES = 11;
const int DRUM_POLYPHONY = 4;

class Drummer : public Instrmnt
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Drummer( void );

  //! Class destructor.
  ~Drummer( void );

  //! Start a note with the given drum type and amplitude.
  /*!
    Use general MIDI drum instrument numbers, converted to
    frequency values as if MIDI note numbers, to select a particular
    instrument.  An StkError will be thrown if the rawwave path is
    incorrectly set.
  */
  void noteOn( StkFloat instrument, StkFloat amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( StkFloat amplitude );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

 protected:

  FileWvIn waves_[DRUM_POLYPHONY];
  OnePole  filters_[DRUM_POLYPHONY];
  std::vector<int> soundOrder_;
  std::vector<int> soundNumber_;
  int      nSounding_;
};

inline StkFloat Drummer :: tick( unsigned int )
{
  lastFrame_[0] = 0.0;
  if ( nSounding_ == 0 ) return lastFrame_[0];

  for ( int i=0; i<DRUM_POLYPHONY; i++ ) {
    if ( soundOrder_[i] >= 0 ) {
      if ( waves_[i].isFinished() ) {
        // Re-order the list.
        for ( int j=0; j<DRUM_POLYPHONY; j++ ) {
          if ( soundOrder_[j] > soundOrder_[i] )
            soundOrder_[j] -= 1;
        }
        soundOrder_[i] = -1;
        nSounding_--;
      }
      else
        lastFrame_[0] += filters_[i].tick( waves_[i].tick() );
    }
  }

  return lastFrame_[0];
}

} // stk namespace

#endif
