/***************************************************/
/*! \class FM
    \brief STK abstract FM synthesis base class.

    This class controls an arbitrary number of
    waves and envelopes, determined via a
    constructor argument.

    Control Change Numbers: 
       - Control One = 2
       - Control Two = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "FM.h"
#include "SKINI.msg"
#include <string.h>
#include <stdlib.h>

FM :: FM(int operators)
  : nOperators(operators)
{
  if ( nOperators <= 0 ) {
    char msg[256];
    sprintf(msg, "FM: Invalid number of operators (%d) argument to constructor!", operators);
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  }

  twozero = new TwoZero();
  twozero->setB2( -1.0 );
  twozero->setGain( 0.0 );

  // Concatenate the STK RAWWAVE_PATH to the rawwave file.
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  vibrato = new WaveLoop( strcat(file,"rawwaves/sinewave.raw"), TRUE );
  vibrato->setFrequency(6.0);

  int i;
  ratios = (MY_FLOAT *) new MY_FLOAT[nOperators];
  gains = (MY_FLOAT *) new MY_FLOAT[nOperators];
  adsr = (ADSR **) calloc( nOperators, sizeof(ADSR *) );
  waves = (WaveLoop **) calloc( nOperators, sizeof(WaveLoop *) );
  for (i=0; i<nOperators; i++ ) {
    ratios[i] = 1.0;
    gains[i] = 1.0;
    adsr[i] = new ADSR();
  }

  modDepth = (MY_FLOAT) 0.0;
  control1 = (MY_FLOAT) 1.0;
  control2 = (MY_FLOAT) 1.0;
  baseFrequency = (MY_FLOAT) 440.0;

  MY_FLOAT temp = 1.0;
  for (i=99; i>=0; i--) {
    __FM_gains[i] = temp;
    temp *= 0.933033;
  }

  temp = 1.0;
  for (i=15; i>=0; i--) {
    __FM_susLevels[i] = temp;
    temp *= 0.707101;
  }

  temp = 8.498186;
  for (i=0; i<32; i++) {
    __FM_attTimes[i] = temp;
    temp *= 0.707101;
  }
}

FM :: ~FM()
{
  delete vibrato;
  delete twozero;

  delete [] ratios;
  delete [] gains;
  for (int i=0; i<nOperators; i++ ) {
    delete adsr[i];
    delete waves[i];
  }

  free(adsr);
  free(waves);
}

void FM :: loadWaves(const char **filenames )
{
  for (int i=0; i<nOperators; i++ )
    waves[i] = new WaveLoop( filenames[i], TRUE );
}

void FM :: setFrequency(MY_FLOAT frequency)
{    
  baseFrequency = frequency;

  for (int i=0; i<nOperators; i++ )
    waves[i]->setFrequency( baseFrequency * ratios[i] );
}

void FM :: setRatio(int waveIndex, MY_FLOAT ratio)
{
  if ( waveIndex < 0 ) {
    cerr << "FM: setRatio waveIndex parameter is less than zero!" << endl;
    return;
  }
  else if ( waveIndex >= nOperators ) {
    cerr << "FM: setRatio waveIndex parameter is greater than the number of operators!" << endl;
    return;
  }

  ratios[waveIndex] = ratio;
  if (ratio > 0.0) 
    waves[waveIndex]->setFrequency(baseFrequency * ratio);
  else
    waves[waveIndex]->setFrequency(ratio);
}

void FM :: setGain(int waveIndex, MY_FLOAT gain)
{
  if ( waveIndex < 0 ) {
    cerr << "FM: setGain waveIndex parameter is less than zero!" << endl;
    return;
  }
  else if ( waveIndex >= nOperators ) {
    cerr << "FM: setGain waveIndex parameter is greater than the number of operators!" << endl;
    return;
  }

  gains[waveIndex] = gain;
}

void FM :: setModulationSpeed(MY_FLOAT mSpeed)
{
  vibrato->setFrequency(mSpeed);
}

void FM :: setModulationDepth(MY_FLOAT mDepth)
{
  modDepth = mDepth;
}

void FM :: setControl1(MY_FLOAT cVal)
{
  control1 = cVal * (MY_FLOAT) 2.0;
}

void FM :: setControl2(MY_FLOAT cVal)
{
  control2 = cVal * (MY_FLOAT) 2.0;
}

void FM :: keyOn()
{
  for (int i=0; i<nOperators; i++ )
    adsr[i]->keyOn();
}

void FM :: keyOff()
{
  for (int i=0; i<nOperators; i++ )
    adsr[i]->keyOff();
}

void FM :: noteOff(MY_FLOAT amplitude)
{
  keyOff();

#if defined(_STK_DEBUG_)
  cerr << "FM: NoteOff amplitude = " << amplitude << endl;
#endif
}

void FM :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "FM: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "FM: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_Breath_) // 2
    setControl1( norm );
  else if (number == __SK_FootControl_) // 4
    setControl2( norm );
  else if (number == __SK_ModFrequency_) // 11
    setModulationSpeed( norm * 12.0);
  else if (number == __SK_ModWheel_) // 1
    setModulationDepth( norm );
  else if (number == __SK_AfterTouch_Cont_)	{ // 128
    //adsr[0]->setTarget( norm );
    adsr[1]->setTarget( norm );
    //adsr[2]->setTarget( norm );
    adsr[3]->setTarget( norm );
  }
  else
    cerr << "FM: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "FM: controlChange number = " << number << ", value = " << value << endl;
#endif
}

