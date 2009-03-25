/******************************************/
/*  Simple MIDI Text File Reader Class,   */
/*  by Perry R. Cook, 1996                */ 
/*  This Object can open a MIDI Text File */
/*  and parse it.  The file spec is mine  */
/*  and mine alone, but it's all text so  */
/*  you should be able to figure it out.  */        
/*					  */
/*  SKINI (Synthesis toolKit Instrument   */
/*  Network Interface) is like MIDI, but  */
/*  allows for floating point control     */
/*  changes, note numbers, etc. Example:  */
/*  noteOn 60.01 111.132 plays a sharp    */      
/*  middle C with a velocity of 111.132   */
/*  See SKINI.txt for more information    */
/*  Warning:  Obey columns in the text    */
/*  file if you try to edit it or create  */
/*  your own files.                       */
/******************************************/

#include "MIDIText.h"

MIDIText :: MIDIText(char *fileName)
{
    myFile = fopen(fileName,"r");
    this->nextMessage();
}

MIDIText :: ~MIDIText()
{
}

/*    MIDI File Codes 
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
    else	{   //  these are all nybbles of status bytes //
	if (byte == 8)	printf("NoteOff");
	if (byte == 9)	printf("NoteOn");
	if (byte == 10)	printf("Poly Pressure");
	if (byte == 11)	printf("Control Change");
	if (byte == 12)	printf("Program Change");
	if (byte == 13)	printf("Channel Pressure");
	if (byte == 14)	printf("Pitch Wheel");
    }
*/

int MIDIText ::  nextMessage()
{
    int notDone = 1,point;
    char inputString[1024];
    char tempString[32];

    while (notDone)	{
	notDone = 0;
	if (fgets(inputString,1000,myFile))	{
            sscanf(inputString,"%s %lf %i %f %f",tempString,&deltaTime,&channel,&byteTwo,&byteThree);
	    point = 5;
	    if (tempString[0]=='%') point = 1;
	    if (tempString[point] == 'n')	{	// NoteO'n'
	        messageType = 9;
            }
	    else if (inputString[point] == 'f')   {  // NoteO'f'f
	        messageType = 8;
	    }	    
	    else if (inputString[point] == 'B')   {  // Pitch'B'end
	        byteThree = byteTwo;
	        messageType = 14;
	    }	    
	    else if (inputString[point] == 'a')   {  // Progr'a'mChange
	        messageType = 12;
	    }	    
	    else if (inputString[point] == 'T')   {  // After'T'ouch
	        messageType = 11;
	    }	    
	    else if (inputString[point] == 'o')   {  // Contr'o'lChange
	        messageType = 11;
	    }
	    else if (inputString[0] == '/' || inputString[0] == ' ')	{
		notDone = 1;
	    }
	}
	else {
 	    messageType = -1;
	}	    
    }
    return messageType;
}

int MIDIText ::  getType()
{
    return messageType;
}

int MIDIText ::  getChannel()
{
    return channel;
}

MY_FLOAT MIDIText :: getDelta()
{
    return deltaTime;
}

MY_FLOAT MIDIText :: getByteTwo()
{
    return byteTwo;
}

MY_FLOAT MIDIText :: getByteThree()
{
    return byteThree;
}

/************   Test Main Program   *****************/
/*
void main(int argc,char *argv[])
{
    MIDIText testFile(argv[1]);
    
    while(testFile.nextMessage() > 0)	;

}
*/

