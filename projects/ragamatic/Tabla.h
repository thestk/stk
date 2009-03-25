/***************************************************/
/*! \class Tabla
    \brief STK tabla drum class.

    This class implements a drum sampling synthesizer using FileWvIn
    objects and one-pole filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately interpolated for other
    sample rates.  You can specify the maximum polyphony (maximum
    number of simultaneous voices) in Tabla.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#ifndef STK_TABLA_H
#define STK_TABLA_H

#include "Instrmnt.h"
#include "FileWvIn.h"
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

 protected:

  StkFloat computeSample( void );

  FileWvIn waves_[TABLA_POLYPHONY];
  OnePole  filters_[TABLA_POLYPHONY];
  std::vector<int> soundOrder_;
  std::vector<int> soundNumber_;
  int      nSounding_;

};

#endif
