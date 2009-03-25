#include <stdio.h>
#include <math.h>
#include <audio.h>
#include <unistd.h>
#define ABS(x) ((x < 0) ? (-x) : (x))
#define MAX(A,B)  ( (A) > (B) ? (A) : (B)  )

/*
    This version of soundio.c is specific for use on an SGI machine.

    -Doug Scott, 10/95
*/

ALconfig audio_port_config;
ALport audio_port;
long bufsize;   
int lookaheadbuffers = 8;  // number of lookahead buffers
short peakl, peakr;

int
init_sound(float sr,int chans)

{
	long pvbuf[2];
	int nbuf;
	audio_port_config = ALnewconfig();
	if(ALsetchannels(audio_port_config, chans) < 0)
		exit(-1);
	/* make output queue twice the size of our buffer */
	nbuf = (chans == 2) ? lookaheadbuffers : lookaheadbuffers/2;
	if(ALsetqueuesize(audio_port_config, bufsize=BUFFER_SIZE * nbuf) < 0)
		exit(-1);	
	if(audio_port) ALcloseport(audio_port);
	audio_port = ALopenport("sgi.driver", "w", audio_port_config);
	if(!audio_port) exit(-1);
	ALfreeconfig(audio_port_config);
	audio_port_config = 0;

	pvbuf[0] = AL_OUTPUT_RATE;
	pvbuf[1] = (long) sr;
	ALsetparams(AL_DEFAULT_DEVICE, pvbuf, 2); /* set output SR */
	/* tell port to accept refill at buffers - 1 */
	ALsetfillpoint(audio_port,BUFFER_SIZE * (lookaheadbuffers - 1));  
	return(0);
}

int
playbuf(short *buf,int bufsize)
{
    ALwritesamps(audio_port, buf, bufsize);
    return(0);
}

void shuddup()
{
	if(audio_port) ALcloseport(audio_port);
	audio_port=0;
}
