/**************  Test Main Program Individual Voice *********************/

#include "miditabl.h"
#include "MIDIInpt.h"

#define ATOUCHMAX 2

int main(int argc,char *argv[])
{
    long i, j;
    int oneOn = 0;
    int lastTouch = 0;
    int aTouchThin = 0;
    MY_FLOAT temp,temp2;
    MIDIInpt controller;

    if (argc!=1) 	{
	printf("useage: testMIDI\n");
	exit(0);
    }

    while(1)	{
 	if (controller.nextMessage() > 0)	{
	    temp2 = controller.getByteThree();
	    temp = controller.getByteTwo();
	    if (controller.getType()==9)	{
	        if (temp2 < 1.0) {
 		    if (oneOn == 1) {
			printf("NoteOff         0.0 1 %f %f\n",temp,64.0);
		    }
		    oneOn -= 1;
 	        }
		else {
 		    printf("NoteOn          0.0 1 %f %f\n",temp,temp2);
		    oneOn += 1;
	        }
	    }
	    else if (controller.getType()==8)	{
		if (temp2 < 2.0) temp2 = 64.0;
 		if (oneOn ==1) {
		    printf("NoteOff         0.0 1 %f %f\n",temp,temp2);
		}
		oneOn -= 1;
	    }
            else if (controller.getType() == 11)	{
	        j = (int) temp;
		printf("ControlChange   0.0 1 %i %f\n",j,temp2);
	    }
	    else if (controller.getType() == 13)	{
//	        aTouchThin += 1;
//		if (aTouchThin == ATOUCHMAX)	{
//		    temp2 = fabs(temp - lastTouch);
//		    if (temp2 > 10)	{
//		        printf("AfterTouch      0.0 1 %f\n",temp);
//			lastTouch = temp;
//		    }
//		    aTouchThin = 0;
//		}
	    }
	    else if (controller.getType() == 12)	{
	        j = (int) temp;
	        printf("ProgramChange   0.0 1 %i\n",j);
	    }
	    fflush(stdout);
	}
    }


}

    
