#ifndef STK_RECORDER_H
#define STK_RECORDER_H

#include "Instrmnt.h"
#include "Iir.h"
#include "DelayL.h"
#include "Noise.h"
#include "SineWave.h"
#include "ADSR.h"

namespace stk {

/***************************************************/
/*! \class Recorder
    \brief A recorder / flute physical model.

    This class implements a physical model of a recorder / 
    flute instrument, based on the paper "Sound production
    in recorderlike instruments. II. A simulation model."
    by M.P. Verge, A. Hirschberg and R. Causse, Journal of
    the Acoustical Society of America, 1997.

    Control Change Numbers: 
       - Softness = 2
       - Noise Gain = 4
       - Noise Cutoff = 16
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Mathias Bredholt, McGill University.
    Formatted for STK by Gary Scavone, 2023.
*/
/***************************************************/

class Recorder : public Instrmnt
{
 public:
  //! Class constructor.
  Recorder( void );

  //! Class destructor.
  ~Recorder( void );

  //! Reset and clear all internal state.
  void clear( void );
  
  //! Set instrument parameters for a particular frequency.
  void setFrequency( StkFloat val );

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing( StkFloat amplitude, StkFloat rate );

  //! Decrease breath velocity with given rate of decrease.
  void stopBlowing( StkFloat rate );
  
  //! Start a note with the given frequency and amplitude.
  void noteOn( StkFloat frequency, StkFloat amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( StkFloat amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange( int number, StkFloat value );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument must be less than the number of
    channels in the StkFrames argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );

  void setBlowPressure( StkFloat val );
  void setVibratoGain( StkFloat val );
  void setVibratoFrequency( StkFloat val );
  void setNoiseGain( StkFloat val );
  void setBreathCutoff( StkFloat val );
  void setSoftness( StkFloat val );

 private:
    DelayL pinDelay_;
    DelayL poutDelay_;
    DelayL jetDelay_;
    Iir radiation_filter_;
    Iir visco_in_filter_;
    Iir visco_out_filter_;
    Iir jetFilter_;
    Noise turb_;
    Iir turbFilter_;
    SineWave vibrato_;
    ADSR adsr_;

    //StkFloat M{ 0 };
    //StkFloat maxPressure_( 0 );
    double maxPressure_;
    //StkFloat blow{ 0 };
    StkFloat vibratoGain_;
    StkFloat noiseGain_;
    StkFloat breathCutoff_;
    StkFloat outputGain_;
    StkFloat psi_;

    StkFloat poutL_;
    StkFloat pout_;
    StkFloat poutm1_;
    StkFloat poutm2_;
    StkFloat pin_;
    StkFloat pinm1_;
    StkFloat pinm2_;

    StkFloat b1;
    StkFloat b3;
    StkFloat b4;

    StkFloat Uj_;
    StkFloat Ujm1_;

    StkFloat Qj_;
    StkFloat Qjm1_;
    StkFloat Qjm2_;

    StkFloat Q1_;
    StkFloat Q1m1_;
    StkFloat Q1m2_;

    StkFloat Qp_;
    StkFloat Qpm1_;

    StkFloat pm_;
};

inline StkFrames& Recorder :: tick( StkFrames& frames, unsigned int channel )
{
  unsigned int nChannels = lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Recorder::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  StkFloat *samples = &frames[channel];
  unsigned int j, hop = frames.channels() - nChannels;
  if ( nChannels == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
      *samples++ = tick();
  }
  else {
    for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
      *samples++ = tick();
      for ( j=1; j<nChannels; j++ )
        *samples++ = lastFrame_[j];
    }
  }

  return frames;
}

} // stk namespace

#endif
