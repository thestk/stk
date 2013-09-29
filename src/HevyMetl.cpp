/***************************************************/
/*! \class HevyMetl
    \brief STK heavy metal FM synthesis instrument.

    This class implements 3 cascade operators with
    feedback modulation, also referred to as
    algorithm 3 of the TX81Z.

    Algorithm 3 is :     4--\
                    3-->2-- + -->1-->Out

    Control Change Numbers: 
       - Total Modulator Index = 2
       - Modulator Crossfade = 4
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

#include "HevyMetl.h"

HevyMetl :: HevyMetl()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( unsigned int i=0; i<3; i++ )
    waves_[i] = new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), true );
  waves_[3] = new WaveLoop( (Stk::rawwavePath() + "fwavblnk.raw").c_str(), true );

  this->setRatio(0, 1.0 * 1.000);
  this->setRatio(1, 4.0 * 0.999);
  this->setRatio(2, 3.0 * 1.001);
  this->setRatio(3, 0.5 * 1.002);

  gains_[0] = fmGains_[92];
  gains_[1] = fmGains_[76];
  gains_[2] = fmGains_[91];
  gains_[3] = fmGains_[68];

  adsr_[0]->setAllTimes( 0.001, 0.001, 1.0, 0.01);
  adsr_[1]->setAllTimes( 0.001, 0.010, 1.0, 0.50);
  adsr_[2]->setAllTimes( 0.010, 0.005, 1.0, 0.20);
  adsr_[3]->setAllTimes( 0.030, 0.010, 0.2, 0.20);

  twozero_.setGain( 2.0 );
  vibrato_->setFrequency( 5.5 );
  modDepth_ = 0.0;
}  

HevyMetl :: ~HevyMetl()
{
}

void HevyMetl :: noteOn(StkFloat frequency, StkFloat amplitude)
{
  gains_[0] = amplitude * fmGains_[92];
  gains_[1] = amplitude * fmGains_[76];
  gains_[2] = amplitude * fmGains_[91];
  gains_[3] = amplitude * fmGains_[68];
  this->setFrequency( frequency );
  this->keyOn();

#if defined(_STK_DEBUG_)
  errorString_ << "HevyMetl::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

StkFloat HevyMetl :: tick()
{
  register StkFloat temp;

  temp = vibrato_->tick() * modDepth_ * 0.2;    
  waves_[0]->setFrequency(baseFrequency_ * (1.0 + temp) * ratios_[0]);
  waves_[1]->setFrequency(baseFrequency_ * (1.0 + temp) * ratios_[1]);
  waves_[2]->setFrequency(baseFrequency_ * (1.0 + temp) * ratios_[2]);
  waves_[3]->setFrequency(baseFrequency_ * (1.0 + temp) * ratios_[3]);
    
  temp = gains_[2] * adsr_[2]->tick() * waves_[2]->tick();
  waves_[1]->addPhaseOffset( temp );
    
  waves_[3]->addPhaseOffset( twozero_.lastOut() );
  temp = (1.0 - (control2_ * 0.5)) * gains_[3] * adsr_[3]->tick() * waves_[3]->tick();
  twozero_.tick(temp);
    
  temp += control2_ * 0.5 * gains_[1] * adsr_[1]->tick() * waves_[1]->tick();
  temp = temp * control1_;
    
  waves_[0]->addPhaseOffset( temp );
  temp = gains_[0] * adsr_[0]->tick() * waves_[0]->tick();
    
  lastOutput_ = temp * 0.5;
  return lastOutput_;
}

StkFloat *HevyMetl :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Instrmnt::tick( vector, vectorSize );
}

StkFrames& HevyMetl :: tick( StkFrames& frames, unsigned int channel )
{
  return Instrmnt::tick( frames, channel );
}

