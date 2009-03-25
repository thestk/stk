/***************************************************/
/*! \class Blit
    \brief STK band-limited impulse train class.

    This class generates a band-limited impulse train using a
    closed-form algorithm reported by Stilson and Smith in "Alias-Free
    Digital Synthesis of Classic Analog Waveforms", 1996.  The user
    can specify both the fundamental frequency of the impulse train
    and the number of harmonics contained in the resulting signal.

    The signal is normalized so that the peak value is +/-1.0.

    If nHarmonics is 0, then the signal will contain all harmonics up
    to half the sample rate.  Note, however, that this setting may
    produce aliasing in the signal when the frequency is changing (no
    automatic modification of the number of harmonics is performed by
    the setFrequency() function).

    Original code by Robin Davies, 2005.
    Revisions by Gary Scavone for STK, 2005.
*/
/***************************************************/

#include "Blit.h"
#include <cmath>
#include <limits>
 
Blit:: Blit( StkFloat frequency )
{
  nHarmonics_ = 0;
  this->setFrequency( frequency );
  this->reset();
}

Blit :: ~Blit()
{
}

void Blit :: reset()
{
  phase_ = 0.0;
  lastOutput_ = 0;
}

void Blit :: setFrequency( StkFloat frequency )
{
#if defined(_STK_DEBUG_)
  errorString_ << "Blit::setFrequency: frequency = " << frequency << '.';
  handleError( StkError::DEBUG_WARNING );
#endif

  p_ = Stk::sampleRate() / frequency;
  rate_ = PI / p_;
  this->updateHarmonics();
}

void Blit :: setHarmonics( unsigned int nHarmonics )
{
  nHarmonics_ = nHarmonics;
  this->updateHarmonics();
}

void Blit :: updateHarmonics( void )
{
  if ( nHarmonics_ <= 0 ) {
    unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
    m_ = 2 * maxHarmonics + 1;
  }
  else
    m_ = 2 * nHarmonics_ + 1;

#if defined(_STK_DEBUG_)
  errorString_ << "Blit::updateHarmonics: nHarmonics_ = " << nHarmonics_ << ", m_ = " << m_ << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

StkFloat Blit :: computeSample( void )
{
  // The code below implements the SincM algorithm of Stilson and
  // Smith with an additional scale factor of P / M applied to
  // normalize the output.

  // A fully optimized version of this code would replace the two sin
  // calls with a pair of fast sin oscillators, for which stable fast
  // two-multiply algorithms are well known. In the spirit of STK,
  // which favors clarity over performance, the optimization has not
  // been made here.

  // Avoid a divide by zero at the sinc peak, which has a limiting
  // value of 1.0.
  StkFloat denominator = sin( phase_ );
  if ( denominator <= std::numeric_limits<StkFloat>::epsilon() ) {
    lastOutput_ = 1.0;
  } else {
    lastOutput_ =  sin( m_ * phase_ );
    lastOutput_ /= m_ * denominator;
  }

  phase_ += rate_;
  if ( phase_ >= PI ) phase_ -= PI;
    
	return lastOutput_;
}

