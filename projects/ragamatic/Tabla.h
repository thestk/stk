#ifndef STK_TABLA_H
#define STK_TABLA_H

#include "Instrmnt.h"
#include "FileWvIn.h"
#include "OnePole.h"

namespace stk {

/***************************************************/
/*! \class Tabla
    \brief STK tabla drum class.

    This class implements a drum sampling synthesizer using FileWvIn
    objects and one-pole filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately interpolated for other
    sample rates.  You can specify the maximum polyphony (maximum
    number of simultaneous voices) in Tabla.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

const int TABLA_NUMWAVES = 15;
const int TABLA_POLYPHONY = 4;

class Tabla : public Instrmnt
{
 public:
  //! Class constructor.
  Tabla( void );

  //! Class destructor.
  ~Tabla( void );

  //! Start a note with the given drum type and amplitude.
  void noteOn( StkFloat instrument, StkFloat amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( StkFloat amplitude );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

 protected:

  FileWvIn waves_[TABLA_POLYPHONY];
  OnePole  filters_[TABLA_POLYPHONY];
  std::vector<int> soundOrder_;
  std::vector<int> soundNumber_;
  int nSounding_;

};

inline StkFloat Tabla :: tick( unsigned int )
{
  lastFrame_[0] = 0.0;
  if ( nSounding_ == 0 ) return lastFrame_[0];

  for ( int i=0; i<TABLA_POLYPHONY; i++ ) {
    if ( soundOrder_[i] >= 0 ) {
      if ( waves_[i].isFinished() ) {
        // Re-order the list.
        for ( int j=0; j<TABLA_POLYPHONY; j++ ) {
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
