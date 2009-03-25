/******************************************/
/*  Simple RealTime MIDI Input Object,    */
/*  by Perry R. Cook, 1996                */        
/*					  */
/*  This object takes MIDI from the input */
/*  , parses it, turns it into TSIDI      */
/*  messages, and buffers it up for use by*/
/*  any object that asks for it later.    */
/*					  */
/*  TSIDI (ToolKit Synthesis Instrument   */
/*  Digital Interfaceis like MIDI, but    */
/*  allows for floating point control     */
/*  changes, note numbers, etc. Example:  */
/*  noteOn(1,60.01,111.132) plays a sharp */      
/*  middle C with a velocity of 111.132   */
/*                                        */
/******************************************/

#include "MIDIInpt.h"

int onePending;
MDport inport;
MDevent lastEvent;


// void pollMessage(void *) 
// {
//    int status;
//    while (1)	{
//        while (onePending == 0)	{
//	    mdReceive(inport, &lastEvent, 1);
//	    status = mdGetStatus(lastEvent.msg);
//	    if (status==MD_NOTEON || status==MD_NOTEOFF)
//		onePending = 1;
//	}
//    }
// }

MIDIInpt :: MIDIInpt()
{
    int nports;
    nports = mdInit();
    printf("%d MIDI devices available\n", nports); 
    inport = mdOpenInPort(0);
    if (inport == NULL) 	{
        printf("open failed\n");
	exit(0);
    } 
    mdSetStampMode(inport, MD_NOSTAMP);
    onePending = 0;
//    midi_pid = sproc(pollMessage, PR_SALL);
//    if (midi_pid == -1)
//    {
//       fprintf(stderr, "unable to create midi input thread...aborting.\n");
//       exit(-1);
//    }
}

#define _BSD_SIGNALS
#include <signal.h>

MIDIInpt :: ~MIDIInpt()
{
    mdClosePort(inport);
//    kill(midi_pid, SIGKILL);
}

/*    MIDI File Code 
   if (byte > 15)	{
	     if (byte == 248) printf("MIDI Clock,");
	else if (byte == 249) printf("Undefined,");
	else if (byte == 250) printf("Song Start,");
	else if (byte == 251) printf("Continue,");
	else if (byte == 252) printf("Song Stop,");
	else if (byte == 253) printf("Undefined,");
	else if (byte == 254) printf("ActiveSen,");
	else if (byte == 255) printf("SystReset,");
        else printf("BEATSME");
    }
    else	{
	if (byte == 8)	printf("NoteOff");
	if (byte == 9)	printf("NoteOn");
	if (byte == 10)	printf("Poly Pressure");
	if (byte == 11)	printf("Control Change");
	if (byte == 12)	printf("Program Change");
	if (byte == 13)	printf("Channel Pressure");
	if (byte == 14)	printf("Pitch Wheel");
    }
*/

int MIDIInpt ::  nextMessage()
{
    int status;
    int byte1;
    int byte2;

    messageType = -1;

	    mdReceive(inport, &lastEvent, 1);

//     if (onePending == 1)  {

	status = mdGetStatus(lastEvent.msg);
	byte1 = mdGetByte1(lastEvent.msg);
	byte2 = mdGetByte2(lastEvent.msg);
	channel = mdGetChannel(lastEvent.msg);

	if (status==MD_NOTEON)	{
	    byteTwo = (float) byte1;
	    byteThree = (float) byte2;
	    if (byte2==0) 
	        messageType = 8;
	    else
		messageType = 9;
	}
        else if (status==MD_NOTEOFF)	{
	    byteTwo = (float) byte1;
	    byteThree = (float) byte2;
	    messageType = 8;
	}
        else if (status==MD_CONTROLCHANGE)	{
	    byteTwo = (float) byte1;
	    byteThree = (float) byte2;
	    messageType = 11;
	}
        else if (status==MD_PROGRAMCHANGE)	{
	    byteTwo = (float) byte1;
	    messageType = 12;
	}
        else if (status==MD_CHANNELPRESSURE)	{
	    byteTwo = (float) byte1;
	    messageType = 13;
	}
        else if (status==MD_PITCHBENDCHANGE)	{
	    byteTwo = (float) byte1;
	    byteTwo += (float) byte2 * NORM_7;
	    messageType = 14;
	}
	else {
 	    messageType = -1;
	}	    
//       onePending = 0;

//    }

    return messageType;
}

void MIDIInpt ::  printMessage()
{
    char buffer[128];
    mdPrintEvent(buffer,&lastEvent,1);
    printf(buffer);
}

int MIDIInpt ::  getType()
{
    return messageType;
}

int MIDIInpt ::  getChannel()
{
    return channel;
}

MY_FLOAT MIDIInpt :: getByteTwo()
{
    return byteTwo;
}

MY_FLOAT MIDIInpt :: getByteThree()
{
    return byteThree;
}

/************   Test Main Program   *****************/
/*
void main(int argc,char *argv[])
{
    MIDIInpt testMIDI;
    
    while(1)	{
        if (testMIDI.nextMessage() > 0)
	    testMIDI.printMessage();
    }
}
*/

