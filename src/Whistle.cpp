/***************************************************/
/*! \class Whistle
    \brief STK police/referee whistle instrument class.

    This class implements a hybrid physical/spectral
    model of a police whistle (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Fipple Modulation Frequency = 11
       - Fipple Modulation Gain = 1
       - Blowing Frequency Modulation = 2
       - Volume = 128

    by Perry R. Cook  1996 - 2004.
*/
/***************************************************/

#include "Whistle.h"
#include "SKINI.msg"
#include <math.h>

const int CAN_RADIUS = 100;
const int PEA_RADIUS = 30;
const int BUMP_RADIUS = 5;

const StkFloat NORM_CAN_LOSS = 0.97;
const StkFloat SLOW_CAN_LOSS = 0.90;
const StkFloat GRAVITY = 20.0;

const StkFloat NORM_TICK_SIZE = 0.004;
const StkFloat SLOW_TICK_SIZE = 0.0001;

const StkFloat ENV_RATE = 0.001;

Whistle :: Whistle()
{
  // Concatenate the STK rawwave path to the rawwave file
  sine_ = new WaveLoop( ( Stk::rawwavePath() + "sinewave.raw").c_str(), true );
  sine_->setFrequency( 2800.0 );

  can_.setRadius( CAN_RADIUS );
  can_.setPosition(0, 0, 0); // set can location
  can_.setVelocity(0, 0, 0); // and the velocity

  onepole_.setPole(0.95);  // 0.99

  bumper_.setRadius( BUMP_RADIUS );
  bumper_.setPosition(0.0, CAN_RADIUS-BUMP_RADIUS, 0);
  bumper_.setPosition(0.0, CAN_RADIUS-BUMP_RADIUS, 0);

  pea_.setRadius( PEA_RADIUS );
  pea_.setPosition(0, CAN_RADIUS/2, 0);
  pea_.setVelocity(35, 15, 0);

  envelope_.setRate( ENV_RATE );
  envelope_.keyOn();

	fippleFreqMod_ = 0.5;
	fippleGainMod_ = 0.5;
	blowFreqMod_ = 0.25;
	noiseGain_ = 0.125;
	baseFrequency_ = 2000;

	tickSize_ = NORM_TICK_SIZE;
	canLoss_ = NORM_CAN_LOSS;

	subSample_ = 1;
	subSampCount_ = subSample_;
}

Whistle :: ~Whistle()
{
  delete sine_;
}

void Whistle :: clear()
{
}

void Whistle :: setFrequency(StkFloat frequency)
{
  StkFloat freakency = frequency * 4;  // the whistle is a transposing instrument
  if ( frequency <= 0.0 ) {
    errorString_ << "Whistle::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING );
    freakency = 220.0;
  }

  baseFrequency_ = freakency;
}

void Whistle :: startBlowing(StkFloat amplitude, StkFloat rate)
{
	envelope_.setRate( ENV_RATE );
	envelope_.setTarget( amplitude );
}

void Whistle :: stopBlowing(StkFloat rate)
{
  envelope_.setRate( rate );
  envelope_.keyOff();
}

void Whistle :: noteOn(StkFloat frequency, StkFloat amplitude)
{
  this->setFrequency( frequency );
  this->startBlowing( amplitude*2.0 ,amplitude * 0.2 );
#if defined(_STK_DEBUG_)
  errorString_ << "Whistle::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Whistle :: noteOff(StkFloat amplitude)
{
  this->stopBlowing( amplitude * 0.02 );

#if defined(_STK_DEBUG_)
  errorString_ << "Whistle::NoteOff: amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

int frameCount = 0;

StkFloat Whistle :: tick()
{
  StkFloat soundMix, tempFreq;
  StkFloat envOut = 0, temp, temp1, temp2, tempX, tempY;
  double phi, cosphi, sinphi;
  double gain = 0.5, mod = 0.0;

	if ( --subSampCount_ <= 0 )	{
		tempVectorP_ = pea_.getPosition();
		subSampCount_ = subSample_;
		temp = bumper_.isInside( tempVectorP_ );
#ifdef WHISTLE_ANIMATION
    frameCount += 1;
    if ( frameCount >= (1470 / subSample_) ) {
      frameCount = 0;
      printf("%f %f %f\n",tempVectorP->getX(),tempVectorP->getY(),envOut);
      fflush(stdout);
    }
#endif
    envOut = envelope_.tick();

    if (temp < (BUMP_RADIUS + PEA_RADIUS)) {
      tempX = envOut * tickSize_ * 2000 * noise_.tick();
      tempY = -envOut * tickSize_ * 1000 * (1.0 + noise_.tick());
      pea_.addVelocity( tempX, tempY, 0 ); 
      pea_.tick( tickSize_ );
    }
        
    mod  = exp(-temp * 0.01);	  // exp. distance falloff of fipple/pea effect
    temp = onepole_.tick(mod);	// smooth it a little
    gain = (1.0 - (fippleGainMod_*0.5)) + (2.0 * fippleGainMod_ * temp);
    gain *= gain;	              // squared distance/gain
    //    tempFreq = 1.0				//  Normalized Base Freq
    //			+ (fippleFreqMod_ * 0.25) - (fippleFreqMod_ * temp) // fippleModulation 
    //			- (blowFreqMod_) + (blowFreqMod_ * envOut); // blowingModulation
    // short form of above
    tempFreq = 1.0 + fippleFreqMod_*(0.25-temp) + blowFreqMod_*(envOut-1.0);
    tempFreq *= baseFrequency_;

    sine_->setFrequency(tempFreq);
    
    tempVectorP_ = pea_.getPosition();
    temp = can_.isInside(tempVectorP_);
    temp  = -temp;       // We know (hope) it's inside, just how much??
    if (temp < (PEA_RADIUS * 1.25)) {
      pea_.getVelocity( &tempVector_ );  // This is the can/pea collision
      tempX = tempVectorP_->getX();     // calculation.  Could probably
      tempY = tempVectorP_->getY();     // simplify using tables, etc.
      phi = -atan2(tempY,tempX);

      cosphi = cos(phi);
      sinphi = sin(phi);
      temp1 = (cosphi*tempVector_.getX()) - (sinphi*tempVector_.getY());
      temp2 = (sinphi*tempVector_.getX()) + (cosphi*tempVector_.getY());
      temp1 = -temp1;
      tempX = (cosphi*temp1) + (sinphi*temp2);
      tempY = (-sinphi*temp1) + (cosphi*temp2);
      pea_.setVelocity(tempX, tempY, 0);
      pea_.tick(tickSize_);
      pea_.setVelocity( tempX*canLoss_, tempY*canLoss_, 0 );
      pea_.tick(tickSize_);
    }

    temp = tempVectorP_->getLength();	
    if (temp > 0.01) {
      tempX = tempVectorP_->getX();
      tempY = tempVectorP_->getY();
      phi = atan2( tempY, tempX );
      phi += 0.3 * temp / CAN_RADIUS;
      cosphi = cos(phi);
      sinphi = sin(phi);
      tempX = 3.0 * temp * cosphi;
      tempY = 3.0 * temp * sinphi;
    }
    else {
      tempX = 0.0;
      tempY = 0.0;
    }
    
    temp = (0.9 + 0.1*subSample_*noise_.tick()) * envOut * 0.6 * tickSize_;
    pea_.addVelocity( temp * tempX, (temp*tempY) - (GRAVITY*tickSize_), 0 );
    pea_.tick( tickSize_ );

    // bumper_.tick(0.0);
	}

	temp = envOut * envOut * gain / 2;
	soundMix = temp * ( sine_->tick() + ( noiseGain_*noise_.tick() ) );
	lastOutput_ = 0.25 * soundMix; // should probably do one-zero filter here

	return lastOutput_;
}

StkFloat *Whistle :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Instrmnt::tick( vector, vectorSize );
}

StkFrames& Whistle :: tick( StkFrames& frames, unsigned int channel )
{
  return Instrmnt::tick( frames, channel );
}

void Whistle :: controlChange(int number, StkFloat value)
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "Whistle::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "Whistle::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

  if (number == __SK_NoiseLevel_) // 4
    noiseGain_ = 0.25 * norm;
  else if (number == __SK_ModFrequency_) // 11
    fippleFreqMod_ = norm;
  else if (number == __SK_ModWheel_) // 1
    fippleGainMod_ = norm;
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope_.setTarget( norm * 2.0 );
  else if (number == __SK_Breath_) // 2
    blowFreqMod_ = norm * 0.5;
  else if (number == __SK_Sustain_)	 // 64
	  if (value < 1.0) subSample_ = 1;
  else {
    errorString_ << "Whistle::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "Whistle::controlChange: number = " << number << ", value = " << value << '.';
    handleError( StkError::DEBUG_WARNING );
#endif
}

