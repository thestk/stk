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

    If a[0] is not equal to 1, the filter coeffcients
    are normalized by a[0].

    The \e gain parameter is applied at the filter
    input and does not affect the coefficient values.
    The default gain value is 1.0.  This structure
    results in one extra multiply per computed sample,
    but allows easy control of the overall filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Filter.h"
#include <stdio.h>

Filter :: Filter()
{
  // The default constructor should setup for pass-through.
  gain = 1.0;
  nB = 1;
  nA = 1;
  b = new MY_FLOAT[nB];
  b[0] = 1.0;
  a = new MY_FLOAT[nA];
  a[0] = 1.0;

  inputs = new MY_FLOAT[nB];
  outputs = new MY_FLOAT[nA];
  this->clear();
}

Filter :: Filter(int nb, MY_FLOAT *bCoefficients, int na, MY_FLOAT *aCoefficients)
{
  char message[256];

  // Check the arguments.
  if ( nb < 1 || na < 1 ) {
    sprintf(message, "Filter: nb (%d) and na (%d) must be >= 1!", nb, na);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    sprintf(message, "Filter: a[0] coefficient cannot == 0!");
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  gain = 1.0;
  nB = nb;
  nA = na;
  b = new MY_FLOAT[nB];
  a = new MY_FLOAT[nA];

  inputs = new MY_FLOAT[nB];
  outputs = new MY_FLOAT[nA];
  this->clear();

  this->setCoefficients(nB, bCoefficients, nA, aCoefficients);
}

Filter :: ~Filter()
{
  delete [] b;
  delete [] a;
  delete [] inputs;
  delete [] outputs;
}

void Filter :: clear(void)
{
  int i;
  for (i=0; i<nB; i++)
    inputs[i] = 0.0;
  for (i=0; i<nA; i++)
    outputs[i] = 0.0;
}

void Filter :: setCoefficients(int nb, MY_FLOAT *bCoefficients, int na, MY_FLOAT *aCoefficients)
{
  int i;
  char message[256];

  // Check the arguments.
  if ( nb < 1 || na < 1 ) {
    sprintf(message, "Filter: nb (%d) and na (%d) must be >= 1!", nb, na);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    sprintf(message, "Filter: a[0] coefficient cannot == 0!");
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if (nb != nB) {
    delete [] b;
    delete [] inputs;
    nB = nb;
    b = new MY_FLOAT[nB];
    inputs = new MY_FLOAT[nB];
    for (i=0; i<nB; i++) inputs[i] = 0.0;
  }

  if (na != nA) {
    delete [] a;
    delete [] outputs;
    nA = na;
    a = new MY_FLOAT[nA];
    outputs = new MY_FLOAT[nA];
    for (i=0; i<nA; i++) outputs[i] = 0.0;
  }

  for (i=0; i<nB; i++)
    b[i] = bCoefficients[i];
  for (i=0; i<nA; i++)
    a[i] = aCoefficients[i];

  // scale coefficients by a[0] if necessary
  if (a[0] != 1.0) {
    for (i=0; i<nB; i++)
      b[i] /= a[0];
    for (i=0; i<nA; i++)
      a[i] /= a[0];
  }
}

void Filter :: setNumerator(int nb, MY_FLOAT *bCoefficients)
{
  int i;
  char message[256];

  // Check the arguments.
  if ( nb < 1 ) {
    sprintf(message, "Filter: nb (%d) must be >= 1!", nb);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if (nb != nB) {
    delete [] b;
    delete [] inputs;
    nB = nb;
    b = new MY_FLOAT[nB];
    inputs = new MY_FLOAT[nB];
    for (i=0; i<nB; i++) inputs[i] = 0.0;
  }

  for (i=0; i<nB; i++)
    b[i] = bCoefficients[i];
}

void Filter :: setDenominator(int na, MY_FLOAT *aCoefficients)
{
  int i;
  char message[256];

  // Check the arguments.
  if ( na < 1 ) {
    sprintf(message, "Filter: na (%d) must be >= 1!", na);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    sprintf(message, "Filter: a[0] coefficient cannot == 0!");
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if (na != nA) {
    delete [] a;
    delete [] outputs;
    nA = na;
    a = new MY_FLOAT[nA];
    outputs = new MY_FLOAT[nA];
    for (i=0; i<nA; i++) outputs[i] = 0.0;
  }

  for (i=0; i<nA; i++)
    a[i] = aCoefficients[i];

  // scale coefficients by a[0] if necessary
  if (a[0] != 1.0) {
    for (i=0; i<nB; i++)
      b[i] /= a[0];
    for (i=0; i<nA; i++)
      a[i] /= a[0];
  }
}

void Filter :: setGain(MY_FLOAT theGain)
{
  gain = theGain;
}

MY_FLOAT Filter :: getGain(void) const
{
  return gain;
}

MY_FLOAT Filter :: lastOut(void) const
{
  return outputs[0];
}

MY_FLOAT Filter :: tick(MY_FLOAT sample)
{
  int i;

  outputs[0] = 0.0;
  inputs[0] = gain * sample;
  for (i=nB-1; i>0; i--) {
    outputs[0] += b[i] * inputs[i];
    inputs[i] = inputs[i-1];
  }
  outputs[0] += b[0] * inputs[0];

  for (i=nA-1; i>0; i--) {
    outputs[0] += -a[i] * outputs[i];
    outputs[i] = outputs[i-1];
  }

  return outputs[0];
}

MY_FLOAT *Filter :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
