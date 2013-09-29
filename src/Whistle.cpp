/***************************************************/
/*! \class Whistle
    \brief STK police/referee whistle instrument class.

    This class implements a hybrid physical/spectral
    model of a police whistle (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Fipple Modulation Frequency = 11
       - Fipple Modulation Gain = 1
       - Blowing Frequency Modulation = 2
       - Volume = 128

    by Perry R. Cook  1996 - 2002.
*/
/***************************************************/

#include "Whistle.h"
#include "SKINI.msg"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CAN_RADIUS 100
#define PEA_RADIUS 30
#define BUMP_RADIUS 5

#define NORM_CAN_LOSS 0.97
#define SLOW_CAN_LOSS 0.90
#define GRAVITY 20.0
//  GRAVITY WAS 6.0

#define NORM_TICK_SIZE 0.004
#define SLOW_TICK_SIZE 0.0001

#define ENV_RATE 0.001 

Whistle :: Whistle()
{
	tempVector = new Vector3D(0,0,0);
  can = new Sphere(CAN_RADIUS);
  pea = new Sphere(PEA_RADIUS);
  bumper = new Sphere(BUMP_RADIUS);

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char path[128];
  strcpy(path, RAWWAVE_PATH);
  sine = new WaveLoop( strcat(path,"sinewave.raw"), TRUE );
  sine->setFrequency(2800.0);

  can->setPosition(0, 0, 0); // set can location
  can->setVelocity(0, 0, 0); // and the velocity

  onepole.setPole(0.95);  // 0.99

  bumper->setPosition(0.0, CAN_RADIUS-BUMP_RADIUS, 0);
  bumper->setPosition(0.0, CAN_RADIUS-BUMP_RADIUS, 0);
  pea->setPosition(0, CAN_RADIUS/2, 0);
  pea->setVelocity(35, 15, 0);

  envelope.setRate(ENV_RATE);
  envelope.keyOn();

	fippleFreqMod = 0.5;
	fippleGainMod = 0.5;
	blowFreqMod = 0.25;
	noiseGain = 0.125;
	maxPressure = (MY_FLOAT) 0.0;
	baseFrequency = 2000;

	tickSize = NORM_TICK_SIZE;
	canLoss = NORM_CAN_LOSS;

	subSample = 1;
	subSampCount = subSample;
}

Whistle :: ~Whistle()
{
  delete tempVector;
  delete can;
  delete pea;
  delete bumper;
  delete sine;
}

void Whistle :: clear()
{
}

void Whistle :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency * 4;  // the whistle is a transposing instrument
  if ( frequency <= 0.0 ) {
    cerr << "Whistle: setFrequency parameter is less than or equal to zero!" << endl;
    freakency = 220.0;
  }

  baseFrequency = freakency;
}

void Whistle :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
	envelope.setRate(ENV_RATE);
	envelope.setTarget(amplitude);
}

void Whistle :: stopBlowing(MY_FLOAT rate)
{
  envelope.setRate(rate);
  envelope.keyOff();
}

void Whistle :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing(amplitude*2.0 ,amplitude * 0.2);
#if defined(_STK_DEBUG_)
  cerr << "Whistle: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void Whistle :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.02);

#if defined(_STK_DEBUG_)
  cerr << "Whistle: NoteOff amplitude = " << amplitude << endl;
#endif
}

int frameCount = 0;

MY_FLOAT Whistle :: tick()
{
  MY_FLOAT soundMix, tempFreq;
  double envOut = 0, temp, temp1, temp2, tempX, tempY;
  double phi, cosphi, sinphi;
  double gain = 0.5, mod = 0.0;

	if (--subSampCount <= 0)	{
		tempVectorP = pea->getPosition();
		subSampCount = subSample;
		temp = bumper->isInside(tempVectorP);
#ifdef WHISTLE_ANIMATION
    frameCount += 1;
    if (frameCount >= (1470 / subSample))	{
      frameCount = 0;
      printf("%f %f %f\n",tempVectorP->getX(),tempVectorP->getY(),envOut);
      fflush(stdout);
    }
#endif
    envOut = envelope.tick();

    if (temp < (BUMP_RADIUS + PEA_RADIUS)) {
      tempX = envOut * tickSize * 2000 * noise.tick();
      tempY = -envOut * tickSize * 1000 * (1.0 + noise.tick());
      pea->addVelocity(tempX,tempY,0); 
      pea->tick(tickSize);
    }
        
    mod  = exp(-temp * 0.01);	// exp. distance falloff of fipple/pea effect
    temp = onepole.tick(mod);	// smooth it a little
    gain = (1.0 - (fippleGainMod*0.5)) + (2.0 * fippleGainMod * temp);
    gain *= gain;				// squared distance/gain
    //    tempFreq = 1.0				//  Normalized Base Freq
    //			+ (fippleFreqMod * 0.25) - (fippleFreqMod * temp) // fippleModulation 
    //			- (blowFreqMod) + (blowFreqMod * envOut); // blowingModulation
    // short form of above
    tempFreq = 1.0 + fippleFreqMod*(0.25-temp) + blowFreqMod*(envOut-1.0);
    tempFreq *= baseFrequency;

    sine->setFrequency(tempFreq);
    
    tempVectorP = pea->getPosition();
    temp = can->isInside(tempVectorP);
    temp  = -temp;       // We know (hope) it's inside, just how much??
    if (temp < (PEA_RADIUS * 1.25))        {            
      pea->getVelocity(tempVector);	//  This is the can/pea collision
      tempX = tempVectorP->getX();	// calculation.  Could probably
      tempY = tempVectorP->getY();	// simplify using tables, etc.
      phi = -atan2(tempY,tempX);
      cosphi = cos(phi);
      sinphi = sin(phi);
      temp1 = (cosphi*tempVector->getX()) - (sinphi*tempVector->getY());
      temp2 = (sinphi*tempVector->getX()) + (cosphi*tempVector->getY());
      temp1 = -temp1;
      tempX = (cosphi*temp1) + (sinphi*temp2);
      tempY = (-sinphi*temp1) + (cosphi*temp2);
      pea->setVelocity(tempX, tempY, 0);
      pea->tick(tickSize);
      pea->setVelocity(tempX*canLoss, tempY*canLoss, 0);
      pea->tick(tickSize);
    }
        
    temp = tempVectorP->getLength();	
    if (temp > 0.01)        {
      tempX = tempVectorP->getX();
      tempY = tempVectorP->getY();
      phi = atan2(tempY,tempX);
      phi += 0.3 * temp / CAN_RADIUS;
      cosphi = cos(phi);
      sinphi = sin(phi);
      tempX = 3.0 * temp * cosphi;
      tempY = 3.0 * temp * sinphi;
    }
    else {
      tempX = 0.0;
      tempY = 0.0;
    }
    
    temp = (0.9 + 0.1*subSample*noise.tick()) * envOut * 0.6 * tickSize;
    pea->addVelocity(temp * tempX,
                     (temp*tempY) - (GRAVITY*tickSize),0);
    pea->tick(tickSize);

    //    bumper->tick(0.0);
	}

	temp = envOut * envOut * gain / 2;
	soundMix = temp * (sine->tick() + (noiseGain*noise.tick()));
	lastOutput = 0.25 * soundMix; // should probably do one-zero filter here

	return lastOutput;
}

void Whistle :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "Whistle: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "Whistle: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_NoiseLevel_) // 4
    noiseGain = 0.25 * norm;
  else if (number == __SK_ModFrequency_) // 11
    fippleFreqMod = norm;
  else if (number == __SK_ModWheel_) // 1
    fippleGainMod = norm;
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope.setTarget( norm * 2.0 );
  else if (number == __SK_Breath_) // 2
    blowFreqMod = norm * 0.5;
  else if (number == __SK_Sustain_)	 // 64
	  if (value < 1.0) subSample = 1;
  else
    cerr << "Whistle: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "Whistle: controlChange number = " << number << ", value = " << value << endl;
#endif
}

