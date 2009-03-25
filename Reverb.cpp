/******************************************/  
/*  Reverb Effect Applied to Soundfile    */
/*  by Perry Cook, 1996                   */
/*					  */
/*  This is based on some of the famous   */
/*  Stanford CCRMA reverbs (NRev, KipRev) */
/*  all based on the the Chowning/Moorer/ */
/*  Schroeder reverberators, which use    */
/*  networks of simple allpass and comb   */
/*  delay filters.                        */
/******************************************/

#include "Reverb.h"

Reverb :: Reverb(MY_FLOAT longestDelay)
{
    delayLine[0] = new DLineN((long) (longestDelay * 0.164631) + 2);
    delayLine[1] = new DLineN((long) (longestDelay * 0.513434) + 2);
    delayLine[2] = new DLineN((long) (longestDelay * 1.000000) + 2);
    delayLine[3] = new DLineN((long) (longestDelay * 0.830484) + 2);
    delayLine[0]->setDelay((long) (longestDelay * 0.164631));
    delayLine[1]->setDelay((long) (longestDelay * 0.513434));
    delayLine[2]->setDelay((long) (longestDelay * 1.000000));
    delayLine[3]->setDelay((long) (longestDelay * 0.830484));
    allPassCoeff = 0.7;
    combCoeff1 = 0.62;
    combCoeff2 = -0.71;
    effectMix = 0.5;
    this->clear();
}

Reverb :: ~Reverb()
{
    delete delayLine[0];
    delete delayLine[1];
    delete delayLine[2];
    delete delayLine[3];
}

void Reverb :: clear()
{
    delayLine[0]->clear();
    delayLine[1]->clear();
    delayLine[2]->clear();
    delayLine[3]->clear();
    lastOutL = 0.0;
    lastOutR = 0.0;
}

void Reverb :: setEffectMix(MY_FLOAT mix)
{
    effectMix = mix;
}

MY_FLOAT Reverb :: lastOutput()
{
    return (lastOutL + lastOutR) * 0.5;
}

MY_FLOAT Reverb :: lastOutputL()
{
    return lastOutL;
}

MY_FLOAT Reverb :: lastOutputR()
{
    return lastOutR;
}

MY_FLOAT Reverb :: tick(MY_FLOAT input)
{
    MY_FLOAT temp,temp0,temp1,temp2,temp3;

    temp = delayLine[0]->lastOut();
    temp0 = allPassCoeff * temp;
    temp0 += input;
    delayLine[0]->tick(temp0);
    temp0 = -(allPassCoeff * temp0) + temp;
    
    temp = delayLine[1]->lastOut();
    temp1 = allPassCoeff * temp;
    temp1 += temp0;
    delayLine[1]->tick(temp1);
    temp1 = -(allPassCoeff * temp1) + temp;
    
    temp2 = temp1 + (combCoeff1 * delayLine[2]->lastOut());
    temp3 = temp1 + (combCoeff2 * delayLine[3]->lastOut());

    lastOutL = effectMix * (delayLine[2]->tick(temp2));
    lastOutR = effectMix * (delayLine[3]->tick(temp3));
    temp = (1.0 - effectMix) * input;
    lastOutL += temp;
    lastOutR += temp;
    
    return (lastOutL + lastOutR) * 0.5;

}

/**************  Test Main Program  *********************/


struct headerform {    /***  NeXT Soundfile Header Struct  ***/
    char pref[4];
    long hdr_length;
    long file_length;
    long mode;
    long samp_rate;
    long num_channels;
    char comment[1024];
};

/*
int main(int argc, char *argv[])
{
    FILE *soundIn,*soundOut;
    long i,dLength;
    short data[2];
    MY_FLOAT temp,efMix;
    Reverb *effect;    
    struct headerform hdr = {".sn",28,0,3,(long) SRATE,1,"TK"};
    hdr.pref[3] = 'd';

    if (argc==5)	{
	soundIn = fopen(argv[3],"rb");
	soundOut = fopen(argv[4],"wb");
	if (soundIn && soundOut)	{
	    dLength = atoi(argv[2]);
	    efMix = atof(argv[1]);
	    fread(&hdr,4,2,soundIn);
	    fread(&hdr.file_length,hdr.hdr_length,1,soundIn);
	    hdr.file_length += 4 * (2 * dLength * hdr.num_channels);
	    if (hdr.mode != 3)	{
	        fclose(soundIn);
		fclose(soundOut);
		printf("Only 16 bit linear data supported in this demo");
		exit(0);
	    }
	    fwrite(&hdr,1,hdr.hdr_length,soundOut);
	    effect = new Reverb(dLength);
	    effect->setEffectMix(1.0);
	    for (i=0;i<hdr.file_length;i++)	{
		if (hdr.num_channels==2)	{
	            if (fread(data,2,2,soundIn))	{
		        temp = (data[0] + data[1]) * 0.5;
		        temp *= efMix;
		        temp = effect->tick(temp);
		        data[0] = (short) (temp + (data[0] * (1.0 - efMix)));
		        data[1] = (short) (temp + (data[1] * (1.0 - efMix)));
		    }
		    else	{
		        temp = effect->tick(0);
			data[0] = (short) temp;
			data[1] = (short) temp;
		    }
		    fwrite(data,2,2,soundOut);
		    i++;
	        }
		else	{
	            if (fread(data,2,1,soundIn))	{
		        temp = data[0] * efMix;
		        temp = effect->tick(temp);
		        data[0] = (short) (temp + (data[0] * (1.0 - efMix)));
		    }
		    else	{
		        temp = effect->tick(0);
			data[0] = (short) temp;
		    }
		    fwrite(data,2,1,soundOut);
		}
	    }
	    delete effect;
	    fclose(soundIn);
	    fclose(soundOut);
	}
        else	{
            printf("Can't open one of the files\n");
        }
    }
    else	{
        printf("useage: Reverb        mix maxDelay soundIn.snd soundOut.snd\n");
        printf("0.0 <= mix <= 1.0\n");
        printf("maxDelay is in samples\n");
        printf("soundfiles are 16 bit linear mono or stereo\n");
    }
}
			    
*/
