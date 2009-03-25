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

#include "Ublotar.h"

Ublotar :: Ublotar (StkFloat lowestFrequency)
{
	// initialize variables
	int i;
	unsigned long length_ = (unsigned long) Stk::sampleRate()/((unsigned long) lowestFrequency + 1); 
	for (i=0; i<NUM_STRINGS; i++) {
		// alloc and clear boreDelay
		boreDelay_[i].setMaximumDelay( length_ );	
		boreDelay_[i].clear();
		flute_filter_[i].setPole( 0.7 - (0.1 * 22050 / Stk::sampleRate()) );
		flute_filter_[i].setGain( -1 );
		last_output_ = 0;
		lastLength_[i] = 0;
		lastFreq_[i] = 0;	
		vibTime_ = 0;
		vibRate_ = 0;
		fr_[i] = 100 * (i + 1);
		bp_ = 0;
		ng_ = 0;
		va_ = 0;
		er_ = 0;
		filterRatio_ = 1;
		pluckAmp_[i] = 0;
		pluckPos_[i] = 0;	
		killdc_[i].input = 0;
		killdc_[i].output = 0;
		killdc_[i].lastOutput = 0;	
	}	

	// alloc, clear, setDelay on jetDelay 
	jetDelay_.setMaximumDelay( 2*length_ ); 
	jetDelay_.clear();
	jetDelay_.setDelay( 49 );

	vibLength_ = (int) length_ / 2;
	vibTable_ = new StkFloat[vibLength_];
	for (i=0; i<vibLength_; i++)
		vibTable_[i] = sin( i*TWO_PI/vibLength_ ); 	

	// initialize all other variables
	jd_ = 49;
	jr_ = 0;
	predistortion_gain_ = 1;
	predistortion_outgain_ = 0;
	postdistortion_outgain_ = 1;	
	limit_ = 10;

	stringtopluck_ = 0; // 0-(NUM_STRINGS-1)
	position_ = 5;

}

Ublotar :: ~Ublotar()
{

}

void Ublotar :: noteOn ( StkFloat frequency, StkFloat amplitude )
{
	pluckAmp_[stringtopluck_] = amplitude;
	pluckPos_[stringtopluck_] = position_; 
	squish_[stringtopluck_] = (int) position_; // bug in ublotar~, squish_ is an int!

	this->setFrequency( frequency ); 

	// pluckPos never < 0 in ublotar~, so always do Karplus with the DC block
	int i, j;
	int borelength;
	StkFloat temp = 0;
	for ( j=0; j<NUM_STRINGS; j++) {
		if ( pluckAmp_[j] > 0 ) {
			borelength = (int) boreDelay_[j].getDelay() - 1;

			// load with noise and DC block	
			for ( i=0; i< borelength; i++) {
				temp = makenoise_.tick() * pluckAmp_[j]; // input to DC block
				// DC block
				pluckblock_.output = temp - pluckblock_.input + (0.99 * pluckblock_.output);
				pluckblock_.input = temp;
				pluckblock_.lastOutput = pluckblock_.output;
				temp = pluckblock_.lastOutput; // output from DC block, should go into bore j at location i 

				// ublotar~ adds noise manually into input pointer, just tick in the noise for bore length here
				boreDelay_[j].tick( temp ); 

				// ublotar~ does in-place LPF of delay line here, effect is negligible
			}

			// prevent this string from filling up with noise again after filling it with noise
			pluckAmp_[j] = 0;
		}
	}

}

void Ublotar :: noteOff (StkFloat amplitude)
{

}

void Ublotar :: setFrequency (StkFloat *frequency)
{
	StkFloat temp;
	int i;
	// Set string/bore frequency for each one
	for ( i=0; i<NUM_STRINGS; i++) {
		if (*(frequency + i) < 20)	
			in_[i] = 20;	
		
		lastLength_[i] = *(frequency + i);	
		lastFreq_[i] = *(frequency + i) * 0.66666;

		if (lastFreq_[i] < WATCHIT)
			lastFreq_[i] = WATCHIT;

		temp = ( Stk::sampleRate() / lastFreq_[i] ) - 2;
		boreDelay_[i].setDelay( temp ); 
	}
}


void Ublotar :: setFrequency (StkFloat frequency)
{
	StkFloat temp;
	// Set string/bore frequency for stringtopluck_
	if (frequency < 20)	
		in_[stringtopluck_] = 20;	
		
	lastLength_[stringtopluck_] = frequency;	
	lastFreq_[stringtopluck_] = frequency * 0.66666;

	if (lastFreq_[stringtopluck_] < WATCHIT)
		lastFreq_[stringtopluck_] = WATCHIT;

	temp = ( Stk::sampleRate() / lastFreq_[stringtopluck_] ) - 2;
	boreDelay_[stringtopluck_].setDelay( temp ); 
}


void Ublotar :: setPredistortionGain (StkFloat predistortionGain)
{
	predistortion_outgain_ = predistortionGain;
}

void Ublotar :: setPostdistortionGain (StkFloat postdistortionGain)
{
	postdistortion_outgain_ = postdistortionGain;
}

void Ublotar :: setBreathPressure (StkFloat bp)
{
	bp_ = bp;
}

void Ublotar :: setJetDelay (StkFloat frequency)
{
	StkFloat temp;
	if (frequency < WATCHIT)
		frequency = WATCHIT;
	temp = ( Stk::sampleRate() / frequency ) - 2;	

	// Control jet length directly, not as a function of bore length
	jetDelay_.setDelay( temp );

}

void Ublotar :: setStringToPluck (int stringtopluck)
{
	if (stringtopluck >= NUM_STRINGS)
		stringtopluck_ = NUM_STRINGS - 1;
	else if (stringtopluck < 0)
		stringtopluck_ = 0;
	else 
		stringtopluck_ = stringtopluck;
}

StkFloat Ublotar :: vib()
{
	long temp;
	StkFloat temp_time, alpha, output;

	vibTime_ += vibRate_;
	while (vibTime_ >= (StkFloat) vibLength_) 
		vibTime_ -= (StkFloat) vibLength_;

	while (vibTime_ < 0.) 
		vibTime_ += (StkFloat) vibLength_;
   
	temp_time = vibTime_;
	
	temp = (long) temp_time;
	alpha = temp_time - (StkFloat) temp;
	output = vibTable_[temp];
	output = output + (alpha * (vibTable_[temp+1] - output));
	return output;

}

StkFloat Ublotar :: computeSample ()
{
	StkFloat temp = 0;
	int i;
	StkFloat randPressure = ng_ * makenoise_.tick();
	randPressure += va_ * vib();
	randPressure *= bp_;	

	StkFloat string_bore_output = 0;
	StkFloat tempsave, sample; 
	for ( i=0; i<NUM_STRINGS; i++ ) {
		sample = jr_ * last_output_;
		temp = boreDelay_[i].tick( sample + er_ * last_output_ );
		tempsave = temp;
		temp = flute_filter_[i].tick( temp );
		temp = filterRatio_ * temp + ( 1 - filterRatio_ ) * lowpass_[i].tick( tempsave );

		// kill DC component
		killdc_[i].output = temp - killdc_[i].input + ( 0.99 * killdc_[i].output );
		killdc_[i].input = temp;
		killdc_[i].lastOutput = killdc_[i].output;
		temp = killdc_[i].lastOutput;	

		// limit the output
		if (temp > limit_ || temp < -limit_) temp = 0;
		last_output_ = temp; // return statement of blostring_tick
		string_bore_output += last_output_;
	}

	StkFloat pressureDiff = bp_ + randPressure - string_bore_output * 1/NUM_STRINGS;

	// feedback delay line
	temp = jetDelay_.tick( pressureDiff );

	// distortion
	last_output_ = flutejet_.tick( predistortion_gain_ * temp );
	StkFloat distortion_output = last_output_;
	
	return predistortion_outgain_ * string_bore_output + postdistortion_outgain_ * distortion_output;
}	

void Ublotar :: setJetReflection( StkFloat jetReflection ) // feedgain in ublotar~ 
{
	jr_ = jetReflection;
}

void Ublotar :: setEndReflection( StkFloat endReflection ) // setsustain in ublotar~
{
	// default mode for all presets set same er for all strings
	er_ = endReflection;	
}

void Ublotar :: setFilterRatio( StkFloat filterRatio ) // lowpasscross in ublotar~
{
	// default mode for all presets set same filterRatio for all strings
	filterRatio_ = filterRatio;
}

void Ublotar :: setDistortGain( StkFloat distortGain ) // distortgain in ublotar~
{
	 predistortion_gain_ = distortGain;
}

void Ublotar :: setLimit( StkFloat limit ) // setlimit in ublotar~
{
	// set to 10 for  most presets, sometimes much larger
	limit_ = limit;
}

void Ublotar :: setNoiseGain( StkFloat noiseGain ) // noisegain in ublotar~
{
	ng_ = noiseGain;
}

void Ublotar :: setVib( StkFloat vibFreq, StkFloat vibAmount ) // vib in ublotar~ 
{
	va_ = vibAmount;
	vibRate_ = vibLength_ / Stk::sampleRate() * vibFreq;	
}
