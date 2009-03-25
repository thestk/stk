/***************************************************/
/*! \class BeeThree
    \brief STK Hammond-oid organ FM synthesis instrument.

    This class implements a simple 4 operator
    topology, also referred to as algorithm 8 of
    the TX81Z.

    \code
    Algorithm 8 is :
                     1 --.
                     2 -\|
                         +-> Out
                     3 -/|
                     4 --
    \endcode

    Control Change Numbers: 
       - Operator 4 (feedback) Gain = 2
       - Operator 3 Gain = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "BeeThree.h"

BeeThree :: BeeThree()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( unsigned int i=0; i<3; i++ )
    waves_[i] = new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), true );
  waves_[3] = new WaveLoop( (Stk::rawwavePath() + "fwavblnk.raw").c_str(), true );

  this->setRatio(0, 0.999);
  this->setRatio(1, 1.997);
  this->setRatio(2, 3.006);
  this->setRatio(3, 6.009);

  gains_[0] = fmGains_[95];
  gains_[1] = fmGains_[95];
  gains_[2] = fmGains_[99];
  gains_[3] = fmGains_[95];

  adsr_[0]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr_[1]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr_[2]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr_[3]->setAllTimes( 0.005, 0.001, 0.4, 0.03);

  twozero_.setGain( 0.1 );
}  

BeeThree :: ~BeeThree()
{
}

void BeeThree :: noteOn(StkFloat frequency, StkFloat amplitude)
{
  gains_[0] = amplitude * fmGains_[95];
  gains_[1] = amplitude * fmGains_[95];
  gains_[2] = amplitude * fmGains_[99];
  gains_[3] = amplitude * fmGains_[95];
  this->setFrequency( frequency );
  this->keyOn();

#if defined(_STK_DEBUG_)
  errorString_ << "BeeThree::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

StkFloat BeeThree :: tick()
{
  register StkFloat temp;

  if (modDepth_ > 0.0)	{
    temp = 1.0 + (modDepth_ * vibrato_->tick() * 0.1);
    waves_[0]->setFrequency(baseFrequency_ * temp * ratios_[0]);
    waves_[1]->setFrequency(baseFrequency_ * temp * ratios_[1]);
    waves_[2]->setFrequency(baseFrequency_ * temp * ratios_[2]);
    waves_[3]->setFrequency(baseFrequency_ * temp * ratios_[3]);
  }

  waves_[3]->addPhaseOffset( twozero_.lastOut() );
  temp = control1_ * 2.0 * gains_[3] * adsr_[3]->tick() * waves_[3]->tick();
  twozero_.tick(temp);

  temp += control2_ * 2.0 * gains_[2] * adsr_[2]->tick() * waves_[2]->tick();
  temp += gains_[1] * adsr_[1]->tick() * waves_[1]->tick();
  temp += gains_[0] * adsr_[0]->tick() * waves_[0]->tick();

  lastOutput_ = temp * 0.125;
  return lastOutput_;
}

StkFloat *BeeThree :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Instrmnt::tick( vector, vectorSize );
}

StkFrames& BeeThree :: tick( StkFrames& frames, unsigned int channel )
{
  return Instrmnt::tick( frames, channel );
}
