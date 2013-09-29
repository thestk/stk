#ifndef STK_FILTER_H
#define STK_FILTER_H

#include "Stk.h"
#include <vector>

namespace stk {

/***************************************************/
/*! \class Filter
    \brief STK abstract filter class.

    This class provides limited common functionality for STK digital
    filter subclasses.  It is general enough to support both
    monophonic and polyphonic input/output classes.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

class Filter : public Stk
{
public:
  //! Class constructor.
  Filter( void ) { gain_ = 1.0; channelsIn_ = 1; lastFrame_.resize( 1, 1, 0.0 ); };

  //! Return the number of input channels for the class.
  unsigned int channelsIn( void ) const { return channelsIn_; };

  //! Return the number of output channels for the class.
  unsigned int channelsOut( void ) const { return lastFrame_.channels(); };

  //! Clears all internal states of the filter.
  virtual void clear( void );

  //! Set the filter gain.
  /*!
    The gain is applied at the filter input and does not affect the
    coefficient values.  The default gain value is 1.0.
   */
  void setGain( StkFloat gain ) { gain_ = gain; };

  //! Return the current filter gain.
  StkFloat getGain( void ) const { return gain_; };

  //! Return an StkFrames reference to the last output sample frame.
  const StkFrames& lastFrame( void ) const { return lastFrame_; };

  //! Take a channel of the StkFrames object as inputs to the filter and replace with corresponding outputs.
  /*!
    The StkFrames argument reference is returned.  The \c channel
    argument must be less than the number of channels in the
    StkFrames argument (the first channel is specified by 0).
    However, range checking is only performed if _STK_DEBUG_ is
    defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ) = 0;

protected:

  unsigned int channelsIn_;
  StkFrames lastFrame_;

  StkFloat gain_;
  std::vector<StkFloat> b_;
  std::vector<StkFloat> a_;
  StkFrames outputs_;
  StkFrames inputs_;

};

inline void Filter :: clear( void )
{
  unsigned int i;
  for ( i=0; i<inputs_.size(); i++ )
    inputs_[i] = 0.0;
  for ( i=0; i<outputs_.size(); i++ )
    outputs_[i] = 0.0;
  for ( i=0; i<lastFrame_.size(); i++ )
    lastFrame_[i] = 0.0;  
}

} // stk namespace

#endif
