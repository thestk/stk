/***************************************************/
/*! \class Phonemes
    \brief STK phonemes table.

    This class does nothing other than declare a
    set of 32 static phoneme formant parameters
    and provide access to those values.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__PHONEMES_H)
#define __PHONEMES_H

#include "Stk.h"

class Phonemes
{
public:

  Phonemes(void);
  ~Phonemes(void);

  //! Returns the phoneme name for the given index (0-31).
  static const char *name( unsigned int index );

  //! Returns the voiced component gain for the given phoneme index (0-31).
  static MY_FLOAT voiceGain( unsigned int index );

  //! Returns the unvoiced component gain for the given phoneme index (0-31).
  static MY_FLOAT noiseGain( unsigned int index );

  //! Returns the formant frequency for the given phoneme index (0-31) and partial (0-3).
  static MY_FLOAT formantFrequency( unsigned int index, unsigned int partial );

  //! Returns the formant radius for the given phoneme index (0-31) and partial (0-3).
  static MY_FLOAT formantRadius( unsigned int index, unsigned int partial );

  //! Returns the formant gain for the given phoneme index (0-31) and partial (0-3).
  static MY_FLOAT formantGain( unsigned int index, unsigned int partial );

private:

  static const char phonemeNames[][4];
  static const MY_FLOAT phonemeGains[][2];
  static const MY_FLOAT phonemeParameters[][4][3];

};

#endif
