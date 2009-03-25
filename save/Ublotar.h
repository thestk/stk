/***************************************************/
/*! \class Ublotar
    \brief STK Ublotar model class.

    WaveGuide Flute ala Karjalainen,      
    Smith, Waryznyk, etc.                 
    with polynomial Jet ala Cook          
		by Perry Cook, 1995-96                
										  
    ported to MSP by Dan Trueman, 2000	  
    and modified to become the nearly	  
    righteous Ublotar. Extension of the Blotar to 
		include 6 strings and pre/post distortion outputs,
		more plucking options; closer to the Sullivan electric
		guitar model, while retaining some of the flute
		potentials of the blotar~

		Ublotar modifications ported to STK by
    Simon de Leon, 2006.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#ifndef STK_PLUCKED_H
#define STK_PLUCKED_H

#define WATCHIT 0.00001
#define NUM_STRINGS 6
#define MAX_INPUTS 10
#define VIBLENGTH 1024

#include "Instrmnt.h"
#include "DelayL.h"
#include "DelayA.h"
#include "OneZero.h"
#include "OnePole.h"
#include "Noise.h"
#include "WaveLoop.h"
#include "JetTable.h"
#include <cmath>
#include <vector>
#include "Flute.h"
#include "Blotar.h"
#include "TwoPole.h"

class Ublotar : public Instrmnt 
{
 public:

  //! Default class destructor.
  ~Ublotar ();

  //! Class constructor, taking the lowest desired playing frequency.
  Ublotar (StkFloat lowestFrequency);

	//! Start a note with the given frequency and amplitude
	virtual void noteOn (StkFloat frequency, StkFloat amplitude);

	//! Stop a note with the given amplitude (speed of decay).
	virtual void noteOff (StkFloat amplitude);
 
	//! Set boreDelay_ line length parameter of stringtopluck_ for a particular frequency
	virtual void setFrequency (StkFloat frequency);

	//! Set boreDelay_ line length parameters of all strings  for certain frequencies
	void setFrequency (StkFloat *frequency);

	//! Set pre-distortion gain
	void setPredistortionGain (StkFloat predistortionGain);

	//! Set post-distortion gain
	void setPostdistortionGain (StkFloat postdistortionGain);

	//! Set breath pressure 
	void setBreathPressure (StkFloat bp);

	//! Set jetDelay_ line length parameter
	void setJetDelay (StkFloat frequency);

	//! Set the reflection coefficient for the jet delay (-1.0 - 1.0)
	void setJetReflection( StkFloat jetReflection ); // feedgain in ublotar~ 

	//! Set the reflection coefficient for the air column delay (-1.0 - 1.0)
	void setEndReflection( StkFloat endReflection ); // setsustain in ublotar~

	//! Interpolates between one-pole filter of the flute and lowpass filter of plucked string
	void setFilterRatio( StkFloat filterRatio ); // lowpasscross in ublotar~

	//! Set distortion gain
	void setDistortGain( StkFloat distortGain ); // distortgain in ublotar~

	//! Set hard limiter value to prevent infinitely growing amplitudes
	void setLimit( StkFloat limit ); // setlimit in ublotar~

	//! Set gain of random breath pressure component
	void setNoiseGain( StkFloat noiseGain ); // noisegain in ublotar~

	//! Set vibrato parameters,  which are used to scale random breath pressure component
	void setVib( StkFloat vibFreq, StkFloat vibAmount ); // vib in ublotar~

	//! Set string that frequency and amplitude parameters in subsequent call to noteOn will alter	
	void setStringToPluck (int stringtopluck);

 protected:  
	StkFloat computeSample ( void );
	StkFloat vib ( void );

	
	// Global string parameters
	StkFloat in_[MAX_INPUTS]; 
	StkFloat jd_;
	StkFloat jr_;
	StkFloat predistortion_gain_;
	StkFloat predistortion_outgain_;
	StkFloat postdistortion_outgain_;	
	StkFloat limit_;
	StkFloat *vibTable_;
	DelayL jetDelay_;
	StkFloat bp_;
	StkFloat va_;
	StkFloat ng_;

	// String-Bore variables
	StkFloat fr_[NUM_STRINGS];
	StkFloat er_;
	StkFloat pluckAmp_[NUM_STRINGS];
	StkFloat pluckPos_[NUM_STRINGS];
	int squish_[NUM_STRINGS]; // number of times to LPF the blostring
	StkFloat filterRatio_;
	StkFloat resetDelay_[NUM_STRINGS];
	OnePole flute_filter_[NUM_STRINGS];
	OneZero lowpass_[NUM_STRINGS];
	StkFloat vibTime_, vibRate_;
	DelayL boreDelay_[NUM_STRINGS];
	StkFloat last_output_;
	StkFloat lastFreq_[NUM_STRINGS], lastLength_[NUM_STRINGS];

	// Auxiliary variables
	StkFloat length_;
	int vibLength_;
	StkFloat position_;
	int stringtopluck_;
	struct {
		StkFloat output, input, lastOutput;
	} killdc_[NUM_STRINGS], pluckblock_;
	Noise makenoise_;
	JetTable flutejet_;
};

#endif
