/*******************************************/
/*  Raw Wave File Output Class,            */
/*  by Perry R. Cook, 1995-96              */ 
/*  This version opens a mono NeXT .snd    */
/*  file 16bit data at 22KHz, and          */
/*  pokes buffers of samples into it.      */
/*******************************************/

/*******************************************/
/*  SGI Real-Time Wave File Output Class,  */
/*  by Perry R. Cook, 1996                 */ 
/*  This Object can opens the SGI soundout */
/*  device, and pokes buffers of samples   */
/*  into it.  The real code that does the  */
/*  is from Doug Scott of SGI.             */
/*******************************************/

/*******************************************/
/*  USS Real-Time Wave File Output Class,  */
/*  by Tim Stilson, 1996                   */
/*  based on code by Perry R. Cook, 1996   */ 
/*  This Object opens the USS sound output */
/*  device, and pokes buffers of samples   */
/*  into it.                               */
/*******************************************/

#include "RTWvOut.h"

RTWvOut :: RTWvOut()
{
  soundIO = new RTSoundIO(SRATE, 1);
  counter = 0;
}

RTWvOut :: ~RTWvOut()
{
  soundIO->playBuffer(data,counter);
  counter = 0;
  while (counter<RT_BUFFER_SIZE)	{
	data[counter++] = 0;
  }
  soundIO->playBuffer(data,counter);
  soundIO->playBuffer(data,counter);  // Are these extra writes necessary in USS?
  soundIO->playBuffer(data,counter);
  delete soundIO;
}

void RTWvOut :: tick(MY_FLOAT sample)
{

  data[counter++] = (short) (sample * 32000.0);
  if (counter >= RT_BUFFER_SIZE) {
	soundIO->playBuffer(data,counter);
	counter = 0;
  }
}
