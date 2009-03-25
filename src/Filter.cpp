/***************************************************/
/*! \class Filter
    \brief STK filter class.

    This class implements a generic structure which
    can be used to create a wide range of filters.
    It can function independently or be subclassed
    to provide more specific controls based on a
    particular filter type.

    In particular, this class implements the standard
    difference equation:

    a[0]*y[n] = b[0]*x[n] + ... + b[nb]*x[n-nb] -
                a[1]*y[n-1] - ... - a[na]*y[n-na]

    If a[0] is not equal to 1, the filter coefficients
    are normalized by a[0].

    The \e gain parameter is applied at the filter
    input and does not affect the coefficient values.
    The default gain value is 1.0.  This structure
    results in one extra multiply per computed sample,
    but allows easy control of the overall filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Filter.h"
#include <stdio.h>

Filter :: Filter()
{
  // The default constructor should setup for pass-through.
  gain_ = 1.0;
  b_.push_back( 1.0 );
  a_.push_back( 1.0 );

  inputs_.push_back( 0.0 );
  outputs_.push_back( 0.0 );
}

Filter :: Filter( std::vector<StkFloat> &bCoefficients, std::vector<StkFloat> &aCoefficients )
{
  // Check the arguments.
  if ( bCoefficients.size() == 0 || aCoefficients.size() == 0 ) {
    errorString_ << "Filter: a and b coefficient vectors must both have size > 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    errorString_ << "Filter: a[0] coefficient cannot == 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  gain_ = 1.0;
  b_ = bCoefficients;
  a_ = aCoefficients;

  inputs_ = std::vector<StkFloat> ( b_.size() );
  outputs_ = std::vector<StkFloat> ( a_.size() );
  this->clear();
}

Filter :: ~Filter()
{
}

void Filter :: clear(void)
{
  unsigned int i;
  for (i=0; i<inputs_.size(); i++)
    inputs_[i] = 0.0;
  for (i=0; i<outputs_.size(); i++)
    outputs_[i] = 0.0;
}

void Filter :: setCoefficients( std::vector<StkFloat> &bCoefficients, std::vector<StkFloat> &aCoefficients )
{
  // Check the arguments.
  if ( bCoefficients.size() == 0 || aCoefficients.size() == 0 ) {
    errorString_ << "Filter::setCoefficients: a and b coefficient vectors must both have size > 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    errorString_ << "Filter::setCoefficients: a[0] coefficient cannot == 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( b_.size() != bCoefficients.size() ) {
    b_ = bCoefficients;
    inputs_.clear();
    inputs_ = std::vector<StkFloat> ( b_.size() );
  }
  else {
    for ( unsigned int i=0; i<b_.size(); i++ ) b_[i] = bCoefficients[i];
  }

  if ( a_.size() != aCoefficients.size() ) {
    a_ = aCoefficients;
    outputs_.clear();
    outputs_ = std::vector<StkFloat> ( a_.size() );
  }
  else {
    for ( unsigned int i=0; i<a_.size(); i++ ) a_[i] = aCoefficients[i];
  }

  this->clear();

  // Scale coefficients by a[0] if necessary
  if ( a_[0] != 1.0 ) {
    unsigned int i;
    for ( i=0; i<b_.size(); i++ ) b_[i] /= a_[0];
    for ( i=1; i<a_.size(); i++ )  a_[i] /= a_[0];
  }
}

void Filter :: setNumerator( std::vector<StkFloat> &bCoefficients )
{
  // Check the argument.
  if ( bCoefficients.size() == 0 ) {
    errorString_ << "Filter::setNumerator: coefficient vector must have size > 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( b_.size() != bCoefficients.size() ) {
    b_ = bCoefficients;
    inputs_.clear();
    inputs_ = std::vector<StkFloat> ( b_.size() );
  }
  else {
    for ( unsigned int i=0; i<b_.size(); i++ ) b_[i] = bCoefficients[i];
  }

  this->clear();
}

void Filter :: setDenominator( std::vector<StkFloat> &aCoefficients )
{
  // Check the argument.
  if ( aCoefficients.size() == 0 ) {
    errorString_ << "Filter::setDenominator: coefficient vector must have size > 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    errorString_ << "Filter::setDenominator: a[0] coefficient cannot == 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( a_.size() != aCoefficients.size() ) {
    a_ = aCoefficients;
    outputs_.clear();
    outputs_ = std::vector<StkFloat> ( a_.size() );
  }
  else {
    for ( unsigned int i=0; i<a_.size(); i++ ) a_[i] = aCoefficients[i];
  }

  this->clear();

  // Scale coefficients by a[0] if necessary
  if ( a_[0] != 1.0 ) {
    unsigned int i;
    for ( i=0; i<b_.size(); i++ ) b_[i] /= a_[0];
    for ( i=1; i<a_.size(); i++ )  a_[i] /= a_[0];
  }
}

void Filter :: setGain(StkFloat gain)
{
  gain_ = gain;
}

StkFloat Filter :: getGain(void) const
{
  return gain_;
}

StkFloat Filter :: lastOut(void) const
{
  return outputs_[0];
}

StkFloat Filter :: tick(StkFloat sample)
{
  unsigned int i;

  outputs_[0] = 0.0;
  inputs_[0] = gain_ * sample;
  for (i=b_.size()-1; i>0; i--) {
    outputs_[0] += b_[i] * inputs_[i];
    inputs_[i] = inputs_[i-1];
  }
  outputs_[0] += b_[0] * inputs_[0];

  for (i=a_.size()-1; i>0; i--) {
    outputs_[0] += -a_[i] * outputs_[i];
    outputs_[i] = outputs_[i-1];
  }

  return outputs_[0];
}

StkFloat *Filter :: tick(StkFloat *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}

StkFrames& Filter :: tick( StkFrames& frames, unsigned int channel )
{
  if ( channel == 0 || frames.channels() < channel ) {
    errorString_ << "Filter::tick(): channel argument (" << channel << ") is zero or > channels in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( frames.channels() == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[i] = tick( frames[i] );
  }
  else if ( frames.interleaved() ) {
    unsigned int hop = frames.channels();
    unsigned int index = channel - 1;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      frames[index] = tick( frames[index] );
      index += hop;
    }
  }
  else {
    unsigned int iStart = (channel - 1) * frames.frames();
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[iStart + i] = tick( frames[iStart + i] );
  }

  return frames;
}
