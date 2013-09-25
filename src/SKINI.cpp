/***************************************************/
/*! \class SKINI
    \brief STK SKINI parsing class

    This class parses SKINI formatted text
    messages.  It can be used to parse individual
    messages or it can be passed an entire file.
    The file specification is Perry's and his
    alone, but it's all text so it shouldn't be to
    hard to figure out.

    SKINI (Synthesis toolKit Instrument Network
    Interface) is like MIDI, but allows for
    floating-point control changes, note numbers,
    etc.  The following example causes a sharp
    middle C to be played with a velocity of 111.132:

    noteOn  60.01  111.13

    See also SKINI.txt.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "SKINI.h"
#include <string.h>
#include <stdlib.h>

// Constructor for use when parsing SKINI strings (coming over socket
// for example.  Use parseThis() method with string pointer.
SKINI :: SKINI()
{
}

//  Constructor for reading SKINI files ... use nextMessage() method.
SKINI :: SKINI(char *fileName)
{
  char msg[256];

  myFile = fopen(fileName,"r");
  if ((int) myFile < 0) {
    sprintf(msg, "SKINI: Could not open or find file (%s).", fileName);
    handleError(msg, StkError::FILE_NOT_FOUND);
  }

  this->nextMessage();
}

SKINI :: ~SKINI()
{
}

/*****************  SOME HANDY ROUTINES   *******************/

#include "SKINI.tbl"

#define __SK_MAX_FIELDS_ 5
#define __SK_MAX_SIZE_ 32

short ignore(char aChar)
{
  short ignoreIt = 0;
  if (aChar == 0)   ignoreIt = 1;        //  Null String Termination
  if (aChar == '\n')  ignoreIt = 1;      //  Carraige Return???
  if (aChar == '/') ignoreIt = 2;        //  Comment Line
  return ignoreIt;
}

short delimit(char aChar)
{
  if (aChar == ' ' ||           // Space
      aChar == ','  ||          // Or Comma
      aChar == '\t')            // Or Tab
    return 1;
  else
    return 0;
}

short nextChar(char* aString)
{
  int i;

  for (i=0;i<__SK_MAX_SIZE_;i++)  {
    if (        aString[i] != ' ' &&             // Space
                aString[i] != ','  &&            // Or Comma
                aString[i] != '\t'     )         // Or Tab
	    return i;
  }
  return 1024;
}

int subStrings(char *aString, 
	   char someStrings[__SK_MAX_FIELDS_][__SK_MAX_SIZE_], 
	   int  somePointrs[__SK_MAX_FIELDS_],
	   char *remainderString)
{
  int notDone,howMany,point,temp;
  notDone = 1;
  howMany = 0;
  point = 0;
  temp = nextChar(aString);
  if (temp >= __SK_MAX_SIZE_) {
    notDone = 0;
    printf("Confusion here: Ignoring this line\n");
    printf("%s\n",aString);
    return howMany;
  }
  point = temp;
  somePointrs[howMany] = point;
  temp = 0;
  while (notDone)    {
    if (aString[point] == '\n') {
      notDone = 0;
    }
    else        {
      someStrings[howMany][temp++] = aString[point++];
      if (temp >= __SK_MAX_SIZE_)      {
        howMany = 0;
        return howMany;
      }
      if (delimit(aString[point]) || aString[point] == '\n') {
        someStrings[howMany][temp] = 0;
        howMany += 1;
        if (howMany < __SK_MAX_FIELDS_)       {
          temp = nextChar(&aString[point]);
          point += temp;
          somePointrs[howMany-1] = point;
          temp = 0;
        }
        else   {
          temp = 0;
          somePointrs[howMany-1] = point;
          while(aString[point] != '\n')
            remainderString[temp++] = aString[point++];
          remainderString[temp] = aString[point];
        }
      }
    }   
  }
  //     printf("Got: %i Strings:\n",howMany);
  //     for (temp=0;temp<howMany;temp++) 
  //         printf("%s\n",someStrings[temp]);
  return howMany;
	 
}

/****************  THE ENCHILLADA !!!!  **********************/
/***   This function parses a single string (if it can)   ****/
/***   of SKINI message, setting the appropriate variables ***/
/*************************************************************/

long SKINI :: parseThis(char* aString)
{
  int which,aField;
  int temp,temp2;
  char someStrings[__SK_MAX_FIELDS_][__SK_MAX_SIZE_];
  int  somePointrs[__SK_MAX_FIELDS_];
     
  temp = nextChar(aString);
  if ((which = ignore(aString[temp]))) {
    if (which == 2) printf("// CommentLine: %s\n",aString);
    messageType = 0;
    return messageType;
  }
  else        {
    temp = subStrings(aString,someStrings,somePointrs,remainderString);
    if (temp > 0)    
      which = 0;
    aField = 0;
    strcpy(msgTypeString,someStrings[aField]);
    while ((which < __SK_MaxMsgTypes_) && 
           (strcmp(msgTypeString,
                   skini_msgs[which].messageString)))  {
	    which += 1;  
    }
    if (which >= __SK_MaxMsgTypes_)  {
	    messageType = 0;
	    printf("Couldn't parse this message field: =%s\n %s\n",
             msgTypeString,aString);
	    return messageType;
    }
    else  {
	    messageType = skini_msgs[which].type;
      // printf("Message Token = %s type = %i\n", msgTypeString,messageType);
    }
    aField += 1;

    if (someStrings[aField][0] == '=') {
	    deltaTime = (MY_FLOAT) atof(&someStrings[aField][1]);
	    deltaTime = -deltaTime;
    }
    else {
	    deltaTime = (MY_FLOAT) atof(someStrings[aField]);
    }
    // printf("DeltaTime = %f\n",deltaTime);
    aField += 1;
	
    channel = atoi(someStrings[aField]);    
    // printf("Channel = %i\n",channel);
    aField += 1;
	
    if (skini_msgs[which].data2 != NOPE)    {
	    if (skini_msgs[which].data2 == SK_INT)       {
        byteTwoInt = atoi(someStrings[aField]);    
        byteTwo = (MY_FLOAT) byteTwoInt;
	    }
	    else if (skini_msgs[which].data2 == SK_DBL)       {
        byteTwo = (MY_FLOAT) atof(someStrings[aField]);    
        byteTwoInt = (long) byteTwo;
	    }
	    else if (skini_msgs[which].data2 == SK_STR)       {
        temp = somePointrs[aField-1];    /*  Hack Danger Here, Why -1??? */
        temp2 = 0;
        while (aString[temp] != '\n')   { 
          remainderString[temp2++] = aString[temp++];
        }
        remainderString[temp2] = 0;
      }
	    else {
        byteTwoInt = skini_msgs[which].data2;
        byteTwo = (MY_FLOAT) byteTwoInt;
        aField -= 1;
	    }
	    
	    aField += 1;
	    if (skini_msgs[which].data3 != NOPE)    {
        if (skini_msgs[which].data3 == SK_INT)        {
          byteThreeInt = atoi(someStrings[aField]);    
          byteThree = (MY_FLOAT) byteThreeInt;
        }
        else if (skini_msgs[which].data3 == SK_DBL)   {
          byteThree = (MY_FLOAT) atof(someStrings[aField]);    
          byteThreeInt = (long) byteThree;
        }
        else if (skini_msgs[which].data3 == SK_STR)   {
          temp = somePointrs[aField-1];	/*  Hack Danger Here, Why -1??? */
          temp2 = 0;
          while (aString[temp] != '\n')   { 
            remainderString[temp2++] = aString[temp++];
          }
          remainderString[temp2] = 0;
        }
        else {
          byteThreeInt = skini_msgs[which].data3;
          byteThree = (MY_FLOAT) byteThreeInt;
        }
	    }
	    else {
        byteThreeInt = byteTwoInt;
        byteThree = byteTwo;
	    }
    }
  }
  return messageType;
}

long SKINI ::  nextMessage()
{
  int notDone;
  char inputString[1024];

  notDone = 1;
  while (notDone)     {
    notDone = 0;
    if (!fgets(inputString,1024,myFile)) {    
	    printf("// End of Score. Thanks for using SKINI!!\n");
	    messageType = -1;
	    return messageType;
    }
    else if (parseThis(inputString) == 0)   {
	    notDone = 1;
    }
  }
  return messageType;
}

long SKINI ::  getType() const
{
  return messageType;
}
 
long SKINI ::  getChannel() const
{
  return channel;
}

MY_FLOAT SKINI :: getDelta() const
{
  return deltaTime;
}

MY_FLOAT SKINI :: getByteTwo() const
{
  return byteTwo;
}

long SKINI :: getByteTwoInt() const
{
  return byteTwoInt;
}

MY_FLOAT SKINI :: getByteThree() const
{
  return byteThree;
}

long SKINI :: getByteThreeInt() const
{
  return byteThreeInt;
}

const char* SKINI :: getRemainderString()
{
  return remainderString;
}

const char* SKINI :: getMessageTypeString()
{
  return msgTypeString;
}

const char* SKINI :: whatsThisType(long type)
{
  int i = 0;
  whatString[0] = 0;
  for ( i=0; i<__SK_MaxMsgTypes_; i++ ) {
    if ( type == skini_msgs[i].type ) {
	    strcat(whatString, skini_msgs[i].messageString);
	    strcat(whatString, ",");
    }
  }
  return whatString;            
}

const char* SKINI :: whatsThisController(long contNum)
{
  int i = 0;
  whatString[0] = 0;
  for ( i=0; i<__SK_MaxMsgTypes_; i++) {
    if ( skini_msgs[i].type == __SK_ControlChange_
        && contNum == skini_msgs[i].data2) {
	    strcat(whatString, skini_msgs[i].messageString);
	    strcat(whatString, ",");
    }
  }
  return whatString;
}


