/***************************************************/
/*! \class BlitSaw
    \brief STK band-limited sawtooth wave class.

    This class generates a band-limited sawtooth waveform using a
    closed-form algorithm reported by Stilson and Smith in "Alias-Free
    Digital Synthesis of Classic Analog Waveforms", 1996.  The user
    can specify both the fundamental frequency of the sawtooth and the
    number of harmonics contained in the resulting signal.

    If nHarmonics is 0, then the signal will contain all harmonics up
    to half the sample rate.  Note, however, that this setting may
    produce aliasing in the signal when the frequency is changing (no
    automatic modification of the number of harmonics is performed by
    the setFrequency() function).

    Based on initial code of Robin Davies, 2005.
    Modified algorithm code by Gary Scavone, 2005.
*/
/***************************************************/

#include "BlitSaw.h"
#include <cmath>
#include <limits>
 
BlitSaw:: BlitSaw( StkFloat frequency )
{
  nHarmonics_ = 0;
  this->reset();
  this->setFrequency( frequency );
}

BlitSaw :: ~BlitSaw()
{
}

void BlitSaw :: reset()
{
  phase_ = 0.0f;
  state_ = 0.0;
  lastOutput_ = 0;
}

void BlitSaw :: setFrequency( StkFloat frequency )
{
#if defined(_STK_DEBUG_)
  errorString_ << "BlitSaw::setFrequency: frequency = " << frequency << '.';
  handleError( StkError::DEBUG_WARNING );
#endif

  p_ = Stk::sampleRate() / frequency;
  C2_ = 1 / p_;
  rate_ = PI * C2_;
  this->updateHarmonics();
}

void BlitSaw :: setHarmonics( unsigned int nHarmonics )
{
  nHarmonics_ = nHarmonics;
  this->updateHarmonics();

  // I found that the initial DC offset could be minimized with an
  // initial state setting as given below.  This initialization should
  // only happen before starting the oscillator for the first time
  // (but after setting the frequency and number of harmonics).  I
  // struggled a bit to decide where best to put this and finally
  // settled on here.  In general, the user shouldn't be messing with
  // the number of harmonics once the oscillator is running because
  // this is automatically taken care of in the setFrequency()
  // function.  (GPS - 1 October 2005)
  state_ = -0.5 * a_;
}

void BlitSaw :: updateHarmonics( void )
{
  if ( nHarmonics_ <= 0 ) {
    unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
    m_ = 2 * maxHarmonics + 1;
  }
  else
    m_ = 2 * nHarmonics_ + 1;

  a_ = m_ / p_;

#if defined(_STK_DEBUG_)
  errorString_ << "BlitSaw::updateHarmonics: nHarmonics_ = " << nHarmonics_ << ", m_ = " << m_ << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

StkFloat BlitSaw :: computeSample( void )
{
  // The code below implements the BLIT algorithm of Stilson and
  // Smith, followed by a summation and filtering operation to produce
  // a sawtooth waveform.  After experimenting with various approaches
  // to calculate the average value of the BLIT over one period, I
  // found that an estimate of C2_ = 1.0 / period (in samples) worked
  // most consistently.  A "leaky integrator" is then applied to the
  // difference of the BLIT output and C2_. (GPS - 1 October 2005)

  // A fully  optimized version of this code would replace the two sin 
  // calls with a pair of fast sin oscillators, for which stable fast 
  // two-multiply algorithms are well known. In the spirit of STK,
  // which favors clarity over performance, the optimization has 
  // not been made here.

  // Avoid a divide by zero, or use of a denormalized divisor 
  // at the sinc peak, which has a limiting value of m_ / p_.
  StkFloat denominator = sin( phase_ );
  if ( fabs(denominator) <= std::numeric_limits<StkFloat>::epsilon() )
    lastOutput_ = a_;
  else {
    lastOutput_ =  sin( m_ * phase_ );
    lastOutput_ /= p_ * denominator;
  }

  lastOutput_ += state_ - C2_;
  state_ = lastOutput_ * 0.995;

  phase_ += rate_;
  if ( phase_ >= PI ) phase_ -= PI;
    
	return lastOutput_;
}

