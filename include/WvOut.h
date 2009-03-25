/***************************************************/
/*! \class WvOut
    \brief STK audio output abstract base class.

    This class provides common functionality for a variety of audio
    data output subclasses.

    WvOut supports multi-channel data.  It is important to distinguish
    the tick() methods, which output single samples to all channels in
    a sample frame, from the tickFrame() methods, which take a pointer
    or reference to multi-channel sample frame data.

    Both interleaved and non-interleaved data is supported via the use
    of StkFrames objects.

    Currently, WvOut is non-interpolating and the output rate is
    always Stk::sampleRate().

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#ifndef STK_WVOUT_H
#define STK_WVOUT_H

#include "Stk.h"
#include <vector>

class WvOut : public Stk
{
 public:

  //! Default constructor.
  WvOut();

  //! Class destructor.
  virtual ~WvOut();

  //! Return the number of sample frames output.
  unsigned long getFrameCount( void ) const;

  //! Return the number of seconds of data output.
  StkFloat getTime( void ) const;

  //! Returns \c true if clipping has been detected during output since instantiation or the last reset.
  bool clipStatus( void ) { return clipping_; };

  //! Reset the clipping status to \c false.
  void resetClipStatus( void ) { clipping_ = false; };

  //! Output a single sample to all channels in a sample frame.
  /*!
    An StkError is thrown if an output error occurs.
  */
  void tick( const StkFloat sample );

  //! Output a channel of the StkFrames object to all channels of the WvOut object.
  /*!
    The \c channel argument should be 0 or greater (the first
    channel is specified by 0).  An StkError will be thrown if an
    output error occurs or if the \c channel argument is equal to or
    greater than the number of channels in the StkFrames object.
  */
  void tick( const StkFrames& frames, unsigned int channel = 0 );

  //! Output the StkFrames data.
  /*!
    An StkError will be thrown if an output error occurs or if
    there is an incompatability between the number of channels in the
    WvOut object and that in the StkFrames object.
  */
  void tickFrame( const StkFrames& frames );

 protected:

  // These abstract functions must be implemented in all subclasses.
  // They are used to get around a C++ problem with overloaded virtual
  // functions.
  virtual void computeSample( const StkFloat sample ) = 0;

  virtual void computeFrames( const StkFrames& frames ) = 0;

  // Check for sample clipping and clamp.
  StkFloat& clipTest( StkFloat& sample );

  StkFrames data_;
  unsigned long frameCounter_;
  bool clipping_;

};

#endif
