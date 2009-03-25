/***************************************************/
/*! \class BlitSquare
    \brief STK band-limited square wave class.

    This class generates a band-limited square wave signal.  It is
    derived in part from the approach reported by Stilson and Smith in
    "Alias-Free Digital Synthesis of Classic Analog Waveforms", 1996.
    The algorithm implemented in this class uses a SincM function with
    an even M value to achieve a bipolar bandlimited impulse train.
    This signal is then integrated to achieve a square waveform.  The
    integration process has an associated DC offset so a DC blocking
    filter is applied at the output.

    The user can specify both the fundamental frequency of the
    waveform and the number of harmonics contained in the resulting
    signal.

    If nHarmonics is 0, then the signal will contain all harmonics up
    to half the sample rate.  Note, however, that this setting may
    produce aliasing in the signal when the frequency is changing (no
    automatic modification of the number of harmonics is performed by
    the setFrequency() function).  Also note that the harmonics of a
    square wave fall at odd integer multiples of the fundamental, so
    aliasing will happen with a lower fundamental than with the other
    Blit waveforms.  This class is not guaranteed to be well behaved
    in the presence of significant aliasing.

    Based on initial code of Robin Davies, 2005.
    Modified algorithm code by Gary Scavone, 2005 - 2006.
*/
/***************************************************/

#include "BlitSquare.h"
#include <cmath>
#include <limits>

BlitSquare:: BlitSquare( StkFloat frequency )
{
  nHarmonics_ = 0;
  this->setFrequency( frequency );
  this->reset();
}

BlitSquare :: ~BlitSquare()
{
}

void BlitSquare :: reset()
{
  phase_ = 0.0;
  lastOutput_ = 0;
  dcbState_ = 0.0;
  lastBlitOutput_ = 0;
}

void BlitSquare :: setFrequency( StkFloat frequency )
{
#if defined(_STK_DEBUG_)
  errorString_ << "BlitSquare::setFrequency: frequency = " << frequency << '.';
  handleError( StkError::DEBUG_WARNING );
#endif

  // By using an even value of the parameter M, we get a bipolar blit
  // waveform at half the blit frequency.  Thus, we need to scale the
  // frequency value here by 0.5. (GPS, 2006).
  p_ = 0.5 * Stk::sampleRate() / frequency;
  rate_ = PI / p_;
  this->updateHarmonics();
}

void BlitSquare :: setHarmonics( unsigned int nHarmonics )
{
  nHarmonics_ = nHarmonics;
  this->updateHarmonics();
}

void BlitSquare :: updateHarmonics( void )
{
  // Make sure we end up with an even value of the parameter M here.
  if ( nHarmonics_ <= 0 ) {
    unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
    m_ = 2 * (maxHarmonics + 1);
  }
  else
    m_ = 2 * (nHarmonics_ + 1);

  a_ = m_ / p_;

#if defined(_STK_DEBUG_)
  errorString_ << "BlitSquare::updateHarmonics: nHarmonics_ = " << nHarmonics_ << ", m_ = " << m_ << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

StkFloat BlitSquare :: computeSample( void )
{
  StkFloat temp = lastBlitOutput_;

  // A fully  optimized version of this would replace the two sin calls
  // with a pair of fast sin oscillators, for which stable fast 
  // two-multiply algorithms are well known. In the spirit of STK,
  // which favors clarity over performance, the optimization has 
  // not been made here.

  // Avoid a divide by zero, or use of a denomralized divisor
  // at the sinc peak, which has a limiting value of 1.0.
  StkFloat denominator = sin( phase_ );
  if ( fabs( denominator )  < std::numeric_limits<StkFloat>::epsilon() ) {
    // Inexact comparison safely distinguishes betwen *close to zero*, and *close to PI*.
    if ( phase_ < 0.1f || phase_ > TWO_PI - 0.1f )
      lastBlitOutput_ = a_;
    else
      lastBlitOutput_ = -a_;
  }
  else {
    lastBlitOutput_ =  sin( m_ * phase_ );
    lastBlitOutput_ /= p_ * denominator;
  }

  lastBlitOutput_ += temp;

  // Now apply DC blocker.
  lastOutput_ = lastBlitOutput_ - dcbState_ + 0.999 * lastOutput_;
  dcbState_ = lastBlitOutput_;

  phase_ += rate_;
  if ( phase_ >= TWO_PI ) phase_ -= TWO_PI;

	return lastOutput_;
}

