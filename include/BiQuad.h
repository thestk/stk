#ifndef STK_BIQUAD_H
#define STK_BIQUAD_H

#include "Filter.h"

namespace stk {

/***************************************************/
/*! \class BiQuad
    \brief STK biquad (two-pole, two-zero) filter class.

    This class implements a two-pole, two-zero digital filter.
    Methods are provided for creating a resonance or notch in the
    frequency response while maintaining a constant filter gain.

    Formulae used calculate coefficients for lowpass, highpass,
    bandpass, bandreject and allpass are found on pg. 55 of
    Udo Zölzer's "DAFX - Digital Audio Effects" (2011 2nd ed).   

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

const StkFloat RECIP_SQRT_2 = static_cast<StkFloat>( M_SQRT1_2 );  
class BiQuad : public Filter
{
public:

  //! Default constructor creates a second-order pass-through filter.
  BiQuad();

  //! Class destructor.
  ~BiQuad();

  //! A function to enable/disable the automatic updating of class data when the STK sample rate changes.
  void ignoreSampleRateChange( bool ignore = true ) { ignoreSampleRateChange_ = ignore; };

  //! Set all filter coefficients.
  void setCoefficients( StkFloat b0, StkFloat b1, StkFloat b2, StkFloat a1, StkFloat a2, bool clearState = false );

  //! Set the b[0] coefficient value.
  void setB0( StkFloat b0 ) { b_[0] = b0; };

  //! Set the b[1] coefficient value.
  void setB1( StkFloat b1 ) { b_[1] = b1; };

  //! Set the b[2] coefficient value.
  void setB2( StkFloat b2 ) { b_[2] = b2; };

  //! Set the a[1] coefficient value.
  void setA1( StkFloat a1 ) { a_[1] = a1; };

  //! Set the a[2] coefficient value.
  void setA2( StkFloat a2 ) { a_[2] = a2; };

  //! Sets the filter coefficients for a resonance at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate poles with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  If \e normalize is true,
    the filter zeros are placed at z = 1, z = -1, and the coefficients
    are then normalized to produce a constant unity peak gain
    (independent of the filter \e gain parameter).  The resulting
    filter frequency response has a resonance at the given \e
    frequency.  The closer the poles are to the unit-circle (\e radius
    close to one), the narrower the resulting resonance width.
    An unstable filter will result for \e radius >= 1.0.  The
    \e frequency value should be between zero and half the sample rate.
  */
  void setResonance( StkFloat frequency, StkFloat radius, bool normalize = false );

  //! Set the filter coefficients for a notch at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate zeros with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  No filter normalization is
    attempted.  The \e frequency value should be between zero and half
    the sample rate.  The \e radius value should be positive.
  */
  void setNotch( StkFloat frequency, StkFloat radius );

  //! Set the filter coefficients for a low-pass with cutoff frequency \e fc (in Hz) and Q-factor \e Q.
  /*!
    This method determines the filter coefficients corresponding to a 
    low-pass filter with cutoff placed at \e fc, where sloping behaviour 
    and resonance are determined by \e Q. The default value for \e Q is 
    1/sqrt(2), resulting in a gradual attenuation of frequencies higher than
    \e fc without added resonance. Values greater than this will more 
    aggressively attenuate frequencies above \e fc while also adding a 
    resonance at \e fc. Values less than this will result in a more gradual
    attenuation of frequencies above \e fc, but will also attenuate 
    frequencies below \e fc as well. Both \e fc and \e Q must be positive.
  */
  void setLowPass( StkFloat fc, StkFloat Q=RECIP_SQRT_2 );

  //! Set the filter coefficients for a high-pass with cutoff frequency \e fc (in Hz) and Q-factor \e Q.
  /*!
    This method determines the filter coefficients corresponding to a high-pass 
    filter with cutoff placed at \e fc, where sloping behaviour and resonance 
    are determined by \e Q. The default value for \e Q is 1/sqrt(2), resulting 
    in a gradual attenuation of frequencies lower than \e fc without added 
    resonance. Values greater than this will more aggressively attenuate 
    frequencies below \e fc while also adding a resonance at \e fc. Values less 
    than this will result in a more gradual attenuation of frequencies below 
    \e fc, but will also attenuate frequencies above \e fc as well. 
    Both \e fc and \e Q must be positive.
  */
  void setHighPass( StkFloat fc, StkFloat Q=RECIP_SQRT_2 );

  //! Set the filter coefficients for a band-pass centered at \e fc (in Hz) with Q-factor \e Q.
  /*!
    This method determines the filter coefficients corresponding to a band-pass
    filter with pass-band centered at \e fc, where band width and slope a 
    determined by \e Q. Values for \e Q that are less than 1.0 will attenuate
    frequencies above and below \e fc more gradually, resulting in a convex 
    slope and a wider band. Values for \e Q greater than 1.0 will attenuate 
    frequencies above and below \e fc more aggressively, resulting in a 
    concave slope and a narrower band. Both \e fc and \e Q must be positive.
  */
  void setBandPass( StkFloat fc, StkFloat Q );

  //! Set the filter coefficients for a band-reject centered at \e fc (in Hz) with Q-factor \e Q.
  /*!
    This method determines the filter coefficients corresponding to a 
    band-reject filter with stop-band centered at \e fc, where band width 
    and slope are determined by \e Q. Values for \e Q that are less than 1.0 
    will yield a wider band with greater attenuation of \e fc. Values for \e Q
    greater than 1.0 will yield a narrower band with less attenuation of \e fc. 
    Both \e fc and \e Q must be positive.
  */
  void setBandReject( StkFloat fc, StkFloat Q );

  //! Set the filter coefficients for an all-pass centered at \e fc (in Hz) with Q-factor \e Q.
  /*!
    This method determines the filter coefficients corresponding to
    an all-pass filter whose phase response crosses -pi radians at \e fc.
    High values for \e Q will result in a more instantaenous shift in phase 
    response at \e fc. Lower values will result in a more gradual shift in
    phase response around \e fc. Both \e fc and \e Q must be positive.
  */
  void setAllPass( StkFloat fc, StkFloat Q );

  //! Sets the filter zeroes for equal resonance gain.
  /*!
    When using the filter as a resonator, zeroes places at z = 1, z
    = -1 will result in a constant gain at resonance of 1 / (1 - R),
    where R is the pole radius setting.
  */
  void setEqualGainZeroes( void );

  //! Return the last computed output value.
  StkFloat lastOut( void ) const { return lastFrame_[0]; };

  //! Input one sample to the filter and return a reference to one output.
  StkFloat tick( StkFloat input );

  //! Take a channel of the StkFrames object as inputs to the filter and replace with corresponding outputs.
  /*!
    The StkFrames argument reference is returned.  The \c channel
    argument must be less than the number of channels in the
    StkFrames argument (the first channel is specified by 0).
    However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the filter and write outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  Each channel
    argument must be less than the number of channels in the
    corresponding StkFrames argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );

 protected:

  virtual void sampleRateChanged( StkFloat newRate, StkFloat oldRate );
  
  // Helper function to update the three intermediate values for the predefined filter types
  // along with the feedback filter coefficients. Performs the debug check for fc and Q-factor arguments.
  void setCommonFilterValues( StkFloat fc, StkFloat Q );

  StkFloat K_;
  StkFloat kSqr_;
  StkFloat denom_;
};

inline StkFloat BiQuad :: tick( StkFloat input )
{
  inputs_[0] = gain_ * input;
  lastFrame_[0] = b_[0] * inputs_[0] + b_[1] * inputs_[1] + b_[2] * inputs_[2];
  lastFrame_[0] -= a_[2] * outputs_[2] + a_[1] * outputs_[1];
  inputs_[2] = inputs_[1];
  inputs_[1] = inputs_[0];
  outputs_[2] = outputs_[1];
  outputs_[1] = lastFrame_[0];

  return lastFrame_[0];
}

inline StkFrames& BiQuad :: tick( StkFrames& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "BiQuad::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  StkFloat *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    inputs_[0] = gain_ * *samples;
    *samples = b_[0] * inputs_[0] + b_[1] * inputs_[1] + b_[2] * inputs_[2];
    *samples -= a_[2] * outputs_[2] + a_[1] * outputs_[1];
    inputs_[2] = inputs_[1];
    inputs_[1] = inputs_[0];
    outputs_[2] = outputs_[1];
    outputs_[1] = *samples;
  }

  lastFrame_[0] = outputs_[1];
  return frames;
}

inline StkFrames& BiQuad :: tick( StkFrames& iFrames, StkFrames& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "BiQuad::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  StkFloat *iSamples = &iFrames[iChannel];
  StkFloat *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop ) {
    inputs_[0] = gain_ * *iSamples;
    *oSamples = b_[0] * inputs_[0] + b_[1] * inputs_[1] + b_[2] * inputs_[2];
    *oSamples -= a_[2] * outputs_[2] + a_[1] * outputs_[1];
    inputs_[2] = inputs_[1];
    inputs_[1] = inputs_[0];
    outputs_[2] = outputs_[1];
    outputs_[1] = *oSamples;
  }

  lastFrame_[0] = outputs_[1];
  return iFrames;
}

} // stk namespace

#endif

