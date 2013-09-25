/***************************************************/
/*! \class BandedWG
    \brief Banded waveguide modeling class.

    This class uses banded waveguide techniques to
    model a variety of sounds, including bowed
    bars, glasses, and bowls.  For more
    information, see Essl, G. and Cook, P. "Banded
    Waveguides: Towards Physical Modelling of Bar
    Percussion Instruments", Proceedings of the
    1999 International Computer Music Conference.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Motion = 4
       - Strike Position = 8 (not implemented)
       - Vibrato Frequency = 11
       - Gain = 1
       - Bow Velocity = 128
       - Set Striking = 64
       - Instrument Presets = 16
         - Uniform Bar = 0
         - Tuned Bar = 1
         - Glass Harmonica = 2
         - Tibetan Bowl = 3

    by Georg Essl, 1999 - 2002.
    Modified for Stk 4.0 by Gary Scavone.
*/
/***************************************************/

#include "BandedWG.h"
#include "SKINI.msg"
#include "Noise.h"
#include <math.h>

BandedWG :: BandedWG()
{
  doPluck = true;

  delay = new Delay[MAX_BANDED_MODES];
  bandpass = new BiQuad[MAX_BANDED_MODES];
  
  bowTabl = new BowTabl;
  bowTabl->setSlope( 3.0 );

  adsr = new ADSR;
  adsr->setAllTimes( 0.02, 0.005, 0.9, 0.01);

  freakency = 220.0;
  setPreset(0);

  bowPosition = 0;
  baseGain = (MY_FLOAT) 0.999;
  
  integrationConstant = 0.0;
  trackVelocity = false;

  bowVelocity = 0.0;
  bowTarget = 0.0;
}

BandedWG :: ~BandedWG()
{
  delete bowTabl;
  delete adsr;
  delete [] bandpass;
  delete [] delay;
}

void BandedWG :: clear()
{
  for (int i=0; i<nModes; i++) {
    delay[i].clear();
    bandpass[i].clear();
  }
}

void BandedWG :: setPreset(int preset)
{
  int i;
  switch (preset){

  case 1: // Tuned Bar
    presetModes = 4;
    modes[0] = (MY_FLOAT) 1.0;
    modes[1] = (MY_FLOAT) 4.0198391420;
    modes[2] = (MY_FLOAT) 10.7184986595;
    modes[3] = (MY_FLOAT) 18.0697050938;

    for (i=0; i<presetModes; i++)
      gains[i] = (MY_FLOAT) pow(0.999,(double) i);

    break;

  case 2: // Glass Harmonica
    presetModes = 5;
    modes[0] = (MY_FLOAT) 1.0;
    modes[1] = (MY_FLOAT) 2.32;
    modes[2] = (MY_FLOAT) 4.25;
    modes[3] = (MY_FLOAT) 6.63;
    modes[4] = (MY_FLOAT) 9.38;
    // modes[5] = (MY_FLOAT) 12.22;

    for (i=0; i<presetModes; i++)
      gains[i] = (MY_FLOAT) pow(0.999,(double) i);
    /*
    baseGain = (MY_FLOAT) 0.99999;
    for (i=0; i<presetModes; i++) 
      gains[i]= (MY_FLOAT) pow(baseGain, delay[i].getDelay()+i);
    */

    break;

  case 3: // Tibetan Prayer Bowl
    presetModes = 17;
    modes[0] = (MY_FLOAT) 1.0;
    modes[1] = (MY_FLOAT) 1.135;
    modes[2] = (MY_FLOAT) 2.329;
    modes[3] = (MY_FLOAT) 3.210;
    modes[4] = (MY_FLOAT) 6.046;
    modes[5] = (MY_FLOAT) 6.106;
    modes[6] = (MY_FLOAT) 6.419;
    modes[7] = (MY_FLOAT) 9.689;
    modes[8] = (MY_FLOAT) 12.212;
    modes[9] = (MY_FLOAT) 13.869;
    modes[10] = (MY_FLOAT) 15.735;
    modes[11] = (MY_FLOAT) 15.795;
    modes[12] = (MY_FLOAT) 18.601;
    modes[13] = (MY_FLOAT) 18.661;
    modes[14] = (MY_FLOAT) 19.363;
    modes[15] = (MY_FLOAT) 23.901;
    modes[16] = (MY_FLOAT) 32.470;
  
    for (i=0; i<presetModes; i++)
      gains[i]=0.9995;

    break;

  default: // Uniform Bar
    presetModes = 4;
    modes[0] = (MY_FLOAT) 1.0;
    modes[1] = (MY_FLOAT) 2.756;
    modes[2] = (MY_FLOAT) 5.404;
    modes[3] = (MY_FLOAT) 8.933;

    for (i=0; i<presetModes; i++)
      gains[i] = (MY_FLOAT) pow(0.9,(double) i);

    break;
  }

  nModes = presetModes;
  setFrequency( freakency );
}

void BandedWG :: setFrequency(MY_FLOAT frequency)
{
  freakency = frequency;
  if ( frequency <= 0.0 ) {
    cerr << "BandedWG: setFrequency parameter is less than or equal to zero!" << endl;
    freakency = 220.0;
  }
  if (freakency > 1568.0) freakency = 1568.0;

  MY_FLOAT radius;
  MY_FLOAT base = Stk::sampleRate() / freakency;
  int length;
  for (int i=0; i<presetModes; i++) {
    // Calculate the delay line lengths for each mode.
    length = (int) (base / modes[i]);
    if ( length > 2)
      delay[i].setDelay( length );
    else	{
      nModes = i;
      break;
    }

    // Set the bandpass filter resonances
    radius = 1.0 - PI * freakency * modes[i] / Stk::sampleRate();
    if ( radius < 0.0 ) radius = 0.0;
    bandpass[i].setResonance(freakency * modes[i], radius, true);

    delay[i].clear();
    bandpass[i].clear();
  }

  //int olen = (int)(delay[0].getDelay());
  //strikePosition = (int)(strikePosition*(length/modes[0])/olen);
}

void BandedWG :: setStrikePosition(MY_FLOAT position)
{
  strikePosition = (int)(delay[0].getDelay() * position / 2);
}

void BandedWG :: startBowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOn();
  maxVelocity = 0.03 + (0.1 * amplitude); 
}

void BandedWG :: stopBowing(MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOff();
}

void BandedWG :: pluck(MY_FLOAT amplitude)
{
  for (int i=0; i<nModes; i++)
    delay[i].tick( amplitude / nModes );
}

void BandedWG :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);

  if ( doPluck )
    this->pluck(amplitude);
  else
    this->startBowing(amplitude, amplitude * 0.001);

#if defined(_STK_DEBUG_)
  cerr << "BandedWG: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void BandedWG :: noteOff(MY_FLOAT amplitude)
{
  if ( !doPluck )
    this->stopBowing((1.0 - amplitude) * 0.005);

#if defined(_STK_DEBUG_)
  cerr << "BandedWG: NoteOff amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT BandedWG :: tick()
{
  int k;
  MY_FLOAT velocityInput = 0.0;

  MY_FLOAT input = 0.0;
  if ( doPluck )
    input = 0.0;
  else {
    if (integrationConstant == 0.0)
      velocityInput = 0.0;
    else
      velocityInput = integrationConstant * velocityInput;

    for (k=0; k<nModes; k++)
      velocityInput += baseGain * delay[k].lastOut();
      
    if ( trackVelocity )  {
      bowVelocity *= 0.9995;
      bowVelocity += bowTarget;
      bowTarget *= 0.995;
    }
    else
      bowVelocity = adsr->tick() * maxVelocity;

    input = bowVelocity - velocityInput;
    input = input * bowTabl->tick(input);
    input = input/(MY_FLOAT)nModes;
  }

  MY_FLOAT data = 0.0;  
  for (k=0; k<nModes; k++) {
    bandpass[k].tick(input + gains[k] * delay[k].lastOut());
    delay[k].tick(bandpass[k].lastOut());
    data += bandpass[k].lastOut();
  }
  
  //lastOutput = data * nModes;
  lastOutput = data * 4;
  return lastOutput;
}

void BandedWG :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "BandedWG: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "BandedWG: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_BowPressure_) { // 2
    if ( norm == 0.0 )
      doPluck = true;
    else {
      doPluck = false;
      bowTabl->setSlope( 10.0 - (9.0 * norm));
    }
  }
  else if (number == 4)	{ // 4
    if ( !trackVelocity ) trackVelocity = true;
    bowTarget += 0.005 * (norm - bowPosition);
    bowPosition = norm;
    //adsr->setTarget(bowPosition);
  }
  else if (number == 8) // 8
    this->setStrikePosition( norm );
  else if (number == __SK_AfterTouch_Cont_) { // 128
    //bowTarget += 0.02 * (norm - bowPosition);
    //bowPosition = norm;
    if ( trackVelocity ) trackVelocity = false;
    maxVelocity = 0.13 * norm; 
    adsr->setTarget(norm);
  }      
  else if (number == __SK_ModWheel_) { // 1
    baseGain = 0.9989999999 + (0.001 * norm );
    for (int i=0; i<nModes; i++)
      gains[i]=(MY_FLOAT) pow(baseGain, delay[i].getDelay()+i);
  }
  else if (number == __SK_ModFrequency_) // 11
    integrationConstant = norm;
  else if (number == __SK_Sustain_)	{ // 64
    if (value < 65) doPluck = true;
    else doPluck = false;
  }
  else if (number == __SK_Portamento_)	{ // 65
    if (value < 65) trackVelocity = false;
    else trackVelocity = true;
  }
  else if (number == __SK_ProphesyRibbon_) // 16
		this->setPreset((int) value);  
  else
    cerr << "BandedWG: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "BandedWG: controlChange number = " << number << ", value = " << value << endl;
#endif
}


