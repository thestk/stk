/***************************************************/
/*! \class Moog
    \brief STK moog-like swept filter sampling synthesis class.

    This instrument uses one attack wave, one
    looped wave, and an ADSR envelope (inherited
    from the Sampler class) and adds two sweepable
    formant (FormSwep) filters.

    Control Change Numbers: 
       - Filter Q = 2
       - Filter Sweep Rate = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Moog.h"
#include "SKINI.msg"

Moog :: Moog()
{
  // Concatenate the STK rawwave path to the rawwave file
  attacks[0] = new WvIn( (Stk::rawwavePath() + "mandpluk.raw").c_str(), TRUE );
  loops[0] = new WaveLoop( (Stk::rawwavePath() + "impuls20.raw").c_str(), TRUE );
  loops[1] = new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), TRUE ); // vibrato
  loops[1]->setFrequency((MY_FLOAT) 6.122);

  filters[0] = new FormSwep();
  filters[0]->setTargets( 0.0, 0.7 );

  filters[1] = new FormSwep();
  filters[1]->setTargets( 0.0, 0.7 );

  adsr->setAllTimes((MY_FLOAT) 0.001,(MY_FLOAT) 1.5,(MY_FLOAT) 0.6,(MY_FLOAT) 0.250);
  filterQ = (MY_FLOAT) 0.85;
  filterRate = (MY_FLOAT) 0.0001;
  modDepth = (MY_FLOAT) 0.0;
}  

Moog :: ~Moog()
{
  delete attacks[0];
  delete loops[0];
  delete loops[1];
  delete filters[0];
  delete filters[1];
}

void Moog :: setFrequency(MY_FLOAT frequency)
{
  baseFrequency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "Moog: setFrequency parameter is less than or equal to zero!" << std::endl;
    baseFrequency = 220.0;
  }

  MY_FLOAT rate = attacks[0]->getSize() * 0.01 * baseFrequency / sampleRate();
  attacks[0]->setRate( rate );
  loops[0]->setFrequency(baseFrequency);
}

void Moog :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  MY_FLOAT temp;
    
  this->setFrequency( frequency );
  this->keyOn();
  attackGain = amplitude * (MY_FLOAT) 0.5;
  loopGain = amplitude;

  temp = filterQ + (MY_FLOAT) 0.05;
  filters[0]->setStates( 2000.0, temp );
  filters[1]->setStates( 2000.0, temp );

  temp = filterQ + (MY_FLOAT) 0.099;
  filters[0]->setTargets( frequency, temp );
  filters[1]->setTargets( frequency, temp );

  filters[0]->setSweepRate( filterRate * 22050.0 / Stk::sampleRate() );
  filters[1]->setSweepRate( filterRate * 22050.0 / Stk::sampleRate() );

#if defined(_STK_DEBUG_)
  std::cerr << "Moog: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Moog :: setModulationSpeed(MY_FLOAT mSpeed)
{
  loops[1]->setFrequency(mSpeed);
}

void Moog :: setModulationDepth(MY_FLOAT mDepth)
{
  modDepth = mDepth * (MY_FLOAT) 0.5;
}

MY_FLOAT Moog :: tick()
{
  MY_FLOAT temp;

  if ( modDepth != 0.0 ) {
    temp = loops[1]->tick() * modDepth;    
    loops[0]->setFrequency( baseFrequency * (1.0 + temp) );
  }
  
  temp = Sampler::tick();
  temp = filters[0]->tick( temp );
  lastOutput = filters[1]->tick( temp );
  return lastOutput * 3.0;
}

void Moog :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "Moog: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "Moog: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_FilterQ_) // 2
    filterQ = 0.80 + ( 0.1 * norm );
  else if (number == __SK_FilterSweepRate_) // 4
    filterRate = norm * 0.0002;
  else if (number == __SK_ModFrequency_) // 11
    this->setModulationSpeed( norm * 12.0 );
  else if (number == __SK_ModWheel_)  // 1
    this->setModulationDepth( norm );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    std::cerr << "Moog: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "Moog: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}



