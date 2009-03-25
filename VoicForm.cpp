/*******************************************/
/*  4 Formant Synthesis Instrument         */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains an excitation */
/*  singing wavetable (looping wave with   */
/*  random and periodic vibrato, smoothing */
/*  on frequency, etc.), excitation noise, */
/*  and four sweepable complex resonances. */
/*					   */
/*  Measured Formant data (from me) is     */
/*  included, and enough data is there to  */
/*  support either parallel or cascade     */
/*  synthesis.  In the floating point case */
/*  cascade synthesis is the most natural  */
/*  so that's what you'll find here.       */
/*					   */
/*  For right now, there's a simple command*/
/*  line score interface consisting of 3   */
/*  letter symbols for the phonemes, =xx   */
/*  sets the pitch to x, + and - add and   */
/*  subtract a half step, and ... makes it */
/*  keep doing what it's doing for longer. */
/*******************************************/

#include "VoicForm.h"

VoicForm :: VoicForm() : Instrmnt()
{
    voiced = new SingWave("rawwaves/impuls40.raw");
    voiced->normalize();
    voiced->setGainRate(0.001);
    voiced->setGainTarget(0.0);
    
    noise = new Noise;
    
    filters[0] = new FormSwep;
    filters[1] = new FormSwep;
    filters[2] = new FormSwep;
    filters[3] = new FormSwep;
    filters[0]->setSweepRate(0.001);
    filters[1]->setSweepRate(0.001);
    filters[2]->setSweepRate(0.001);
    filters[3]->setSweepRate(0.001);
    
    onezero = new OneZero;
    onezero->setCoeff(-0.9);
    onepole = new OnePole;
    onepole->setPole(0.9);
    
    noiseEnv = new Envelope;
    noiseEnv->setRate(0.001);
    noiseEnv->setTarget(0.0);
    
    this->setPhoneme("eee");
    this->clear();
}  

VoicForm :: ~VoicForm()
{
    delete voiced;
    delete noise;
    delete filters[0];
    delete filters[1];
    delete filters[2];
    delete filters[3];
    delete onezero;
    delete onepole;
    delete noiseEnv;
}

void VoicForm :: clear()
{    
    onepole->clear();
    filters[0]->clear();
    filters[1]->clear();
    filters[2]->clear();
    filters[3]->clear();
}

void VoicForm :: setFreq(double frequency)
{    
    voiced->setFreq(frequency);
}

void VoicForm :: setFormantAll(int whichOne, double freq,double reson,double gain)
{
    filters[whichOne]->setTargets(freq,reson,gain);
}

    extern double phonGains[32][2];
    extern double phonParams[32][4][3];
    extern char phonemes[32][4];

int VoicForm :: setPhoneme(char *phoneme)
{
     int i=0,found=0;
  
     while(i<32 && !found)      {
         if (!strcmp(phonemes[i],phoneme))      {
             found = 1;
             this->setFormantAll(0,phonParams[i][0][0],phonParams[i][0][1],pow(10.0,phonParams[i][0][2] / 20.0));
             this->setFormantAll(1,phonParams[i][1][0],phonParams[i][1][1],1.0);
             this->setFormantAll(2,phonParams[i][2][0],phonParams[i][2][1],1.0);
             this->setFormantAll(3,phonParams[i][3][0],phonParams[i][3][1],1.0);
             this->setVoicedUnVoiced(phonGains[i][0],phonGains[i][1]);
             printf("Found Formant: %s (number %i)\n",phoneme,i);
         }
         i++;
     }
     if (!found) printf("Phoneme %s not found\n",phoneme);
     return found;
}

void VoicForm :: setVoiced(double vGain)
{
    voiced->setGainTarget(vGain);
}

void VoicForm :: setUnVoiced(double nGain)
{
    noiseEnv->setTarget(nGain);
}

void VoicForm :: setVoicedUnVoiced(double vGain, double nGain)
{
    this->setVoiced(vGain);
    this->setUnVoiced(nGain);
}

void VoicForm :: setFiltSweepRate(int whichOne,double rate)
{
    filters[whichOne]->setSweepRate(rate);
}

void VoicForm :: setPitchSweepRate(double rate)
{
    voiced->setSweepRate(rate);
}

void VoicForm :: speak()
{
    voiced->noteOn();
}

void VoicForm :: quiet()
{
    voiced->noteOff();
    noiseEnv->setTarget(0.0);
}

void VoicForm :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    voiced->setGainTarget(amp);
    onepole->setPole(0.95 - (amp * NORM_7 * 0.2));
    voiced->setFreq(freq);
}

void VoicForm :: noteOff(MY_FLOAT amp)
{
    voiced->noteOff();
}

MY_FLOAT VoicForm :: tick()
{
    MY_FLOAT temp;
    temp   = onepole->tick(onezero->tick(voiced->tick()));
    temp  += noiseEnv->tick() * noise->tick();
    lastOutput  = filters[0]->tick(temp);
    lastOutput  = filters[1]->tick(lastOutput);
    lastOutput  = filters[2]->tick(lastOutput);
    lastOutput  = filters[3]->tick(lastOutput);
    lastOutput *= 0.02;
    return lastOutput;
}
 
void VoicForm :: controlChange(int number, MY_FLOAT value)
{
    MY_FLOAT temp;
    int tempi;

#if defined(_debug_)        
    printf("VoicForm : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
    if (number == MIDI_control1)	{
        this->setVoiced(1.0 - (value*NORM_7));
        this->setUnVoiced(0.01 * value * NORM_7);
    }
    else if (number == MIDI_control2)	{
	tempi = (int) (value / 2);
        if (tempi < 16)	{
	    tempi = tempi;
	    temp = 0.9;
    	}
    	else if (tempi < 32)	{
	    tempi = tempi - 16;
	    temp = 1.0;
    	}
    	else if (tempi < 48)	{
	    tempi = tempi - 32;
	    temp = 1.1;
    	}
    	else if (tempi < 64)	{
	    tempi = tempi - 48;
	    temp = 1.2;
    	}
        this->setFormantAll(0,temp*phonParams[tempi][0][0],
			phonParams[tempi][0][1],
			pow(10.0,phonParams[tempi][0][2] / 20.0));
	this->setFormantAll(1,temp*phonParams[tempi][1][0],
			phonParams[tempi][1][1],1.0);
	this->setFormantAll(2,temp*phonParams[tempi][2][0],
			phonParams[tempi][2][1],1.0);
	this->setFormantAll(3,temp*phonParams[tempi][3][0],
			phonParams[tempi][3][1],1.0);
    }
    else if (number == MIDI_control3)
        voiced->setVibFreq(value * NORM_7 * 12.0);  /* 0 to 12 Hz */
    else if (number == MIDI_mod_wheel)
        voiced->setVibAmt(value * NORM_7 * 0.2);
    else if (number == MIDI_after_touch)	{
        this->setVoiced(value*NORM_7);
        onepole->setPole(0.99 - (value*NORM_7 * 0.2));
    }
    else	{
        printf("VoicForm : Undefined Control Number!!\n");
    }  
}

