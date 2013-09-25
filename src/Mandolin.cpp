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

#include "Mandolin.h"
#include "SKINI.msg"

#include <string.h>

Mandolin :: Mandolin(MY_FLOAT lowestFrequency)
  : PluckTwo(lowestFrequency)
{
  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char temp[128];
  strcpy(temp, RAWWAVE_PATH);
  soundfile[0] = new WvIn( strcat(temp,"rawwaves/mand1.raw"), TRUE );
  strcpy(temp, RAWWAVE_PATH);
  soundfile[1] = new WvIn( strcat(temp,"rawwaves/mand2.raw"), TRUE );
  strcpy(temp, RAWWAVE_PATH);
  soundfile[2] = new WvIn( strcat(temp,"rawwaves/mand3.raw"), TRUE );
  strcpy(temp, RAWWAVE_PATH);
  soundfile[3] = new WvIn( strcat(temp,"rawwaves/mand4.raw"), TRUE );
  strcpy(temp, RAWWAVE_PATH);
  soundfile[4] = new WvIn( strcat(temp,"rawwaves/mand5.raw"), TRUE );
  strcpy(temp, RAWWAVE_PATH);
  soundfile[5] = new WvIn( strcat(temp,"rawwaves/mand6.raw"), TRUE );
  strcpy(temp, RAWWAVE_PATH);
  soundfile[6] = new WvIn( strcat(temp,"rawwaves/mand7.raw"), TRUE );
  strcpy(temp, RAWWAVE_PATH);
  soundfile[7] = new WvIn( strcat(temp,"rawwaves/mand8.raw"), TRUE );
  strcpy(temp, RAWWAVE_PATH);
  soundfile[8] = new WvIn( strcat(temp,"rawwaves/mand9.raw"), TRUE );
  strcpy(temp, RAWWAVE_PATH);
  soundfile[9] = new WvIn( strcat(temp,"rawwaves/mand10.raw"), TRUE );
  strcpy(temp, RAWWAVE_PATH);
  soundfile[10] = new WvIn( strcat(temp,"rawwaves/mand11.raw"), TRUE );
  strcpy(temp, RAWWAVE_PATH);
  soundfile[11] = new WvIn( strcat(temp,"rawwaves/mand12.raw"), TRUE );
  directBody = 1.0;
  mic = 0;
  dampTime = 0;
  waveDone = soundfile[mic]->isFinished();
}

Mandolin :: ~Mandolin()
{
  for ( int i=0; i<12; i++ )
    delete soundfile[i];
}

void Mandolin :: pluck(MY_FLOAT amplitude)
{
  // This function gets interesting, because pluck
  // may be longer than string length, so we just
  // reset the soundfile and add in the pluck in
  // the tick method.
  soundfile[mic]->reset();
  waveDone = false;
  pluckAmplitude = amplitude;
  if ( amplitude < 0.0 ) {
    cerr << "Mandolin: pluck amplitude parameter less than zero!" << endl;
    pluckAmplitude = 0.0;
  }
  else if ( amplitude > 1.0 ) {
    cerr << "Mandolin: pluck amplitude parameter greater than 1.0!" << endl;
    pluckAmplitude = 1.0;
  }

  // Set the pick position, which puts zeroes at position * length.
  combDelay->setDelay((MY_FLOAT) 0.5 * pluckPosition * lastLength); 
  dampTime = (long) lastLength;   // See tick method below.
}

void Mandolin :: pluck(MY_FLOAT amplitude, MY_FLOAT position)
{
  // Pluck position puts zeroes at position * length.
  pluckPosition = position;
  if ( position < 0.0 ) {
    cerr << "Mandolin: pluck position parameter less than zero!" << endl;
    pluckPosition = 0.0;
  }
  else if ( position > 1.0 ) {
    cerr << "Mandolin: pluck position parameter greater than 1.0!" << endl;
    pluckPosition = 1.0;
  }

  this->pluck(amplitude);
}

void Mandolin :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->pluck(amplitude);

#if defined(_STK_DEBUG_)
  cerr << "Mandolin: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void Mandolin :: setBodySize(MY_FLOAT size)
{
  // Scale the commuted body response by its sample rate (22050).
  MY_FLOAT rate = size * 22050.0 / Stk::sampleRate();
  for ( int i=0; i<12; i++ )
    soundfile[i]->setRate(rate);
}

MY_FLOAT Mandolin :: tick()
{
  MY_FLOAT temp = 0.0;
  if ( !waveDone ) {
    // Scale the pluck excitation with comb
    // filtering for the duration of the file.
    temp = soundfile[mic]->tick() * pluckAmplitude;
    temp = temp - combDelay->tick(temp);
    waveDone = soundfile[mic]->isFinished();
  }

  // Damping hack to help avoid overflow on re-plucking.
  if ( dampTime >=0 ) {
    dampTime -= 1;
    // Calculate 1st delay filtered reflection plus pluck excitation.
    lastOutput = delayLine->tick( filter->tick( temp + (delayLine->lastOut() * (MY_FLOAT) 0.7) ) );
    // Calculate 2nd delay just like the 1st.
    lastOutput += delayLine2->tick( filter2->tick( temp + (delayLine2->lastOut() * (MY_FLOAT) 0.7) ) );
  }
  else { // No damping hack after 1 period.
    // Calculate 1st delay filtered reflection plus pluck excitation.
    lastOutput = delayLine->tick( filter->tick( temp + (delayLine->lastOut() * loopGain) ) );
    // Calculate 2nd delay just like the 1st.
    lastOutput += delayLine2->tick( filter2->tick( temp + (delayLine2->lastOut() * loopGain) ) );
  }

  lastOutput *= (MY_FLOAT) 0.3;
  return lastOutput;
}

void Mandolin :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "Mandolin: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "Mandolin: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_BodySize_) // 2
    this->setBodySize( norm * 2.0 );
  else if (number == __SK_PickPosition_) // 4
    this->setPluckPosition( norm );
  else if (number == __SK_StringDamping_) // 11
    this->setBaseLoopGain((MY_FLOAT) 0.97 + (norm * (MY_FLOAT) 0.03));
  else if (number == __SK_StringDetune_) // 1
    this->setDetune((MY_FLOAT) 1.0 - (norm * (MY_FLOAT) 0.1));
  else if (number == __SK_AfterTouch_Cont_) // 128
    mic = (int) (norm * 11.0);
  else
    cerr << "Mandolin: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "Mandolin: controlChange number = " << number << ", value = " << value << endl;
#endif
}
