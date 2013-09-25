/*******************************************/
/*  Matlab MAT-file Output Class,          */
/*  by Gary P. Scavone, 1999.              */
/*  This object creates a Matlab MAT-file  */
/*  structure with a numeric array         */
/*  subelement and fills it with buffers   */
/*  of samples (doubles).                  */
/*                                         */
/*  When originally created, the Matlab    */
/*  MAT-file format was not public and I   */
/*  had to reverse-engineer the format.    */
/*  Matlab finally released the format in  */
/*  the Spring of 1999, and this class was */
/*  updated to more closely adhere to      */
/*  specifications.                        */
/*******************************************/

#include "MatWvOut.h"

/********  Matlab Matfile Header Struct   *******/
struct matheaderform {
  char heading[124]; // Header text field
  INT16 hff[2];      // Header flag fields
  INT32 adf[11];     // Array data format fields
  // There's more, but it's of variable length
};

FILE *openMatFile(int chans,char *fileName)	{
  struct matheaderform hdr;
  FILE *fd;
  char tempName[128];
  char arrayName[128];
  char msg[256];
  int i;
  INT32 tmp, headsize, namelen;

  strcpy(hdr.heading,"MATLAB 5.0 MAT-file, Generated using the Synthesis ToolKit (STK). By Gary P. Scavone, CCRMA, Stanford University, 1999.");

  for (i=strlen(hdr.heading);i<124;i++) hdr.heading[i] = ' ';

  // Header Flag Fields
  hdr.hff[0] = (INT16) 0x0100;   // Version field
  hdr.hff[1] = (INT16) 'M';      // Endian indicator field ("MI")
  hdr.hff[1] <<= 8;
  hdr.hff[1] += 'I';

  hdr.adf[0] = (INT32) 14;       // Matlab array data type value
  hdr.adf[1] = (INT32) 0;        // Size of file after this point to end (in bytes)
                                 // Don't know size yet.

  // Numeric Array Subelements (4):
  // 1. Array Flags
  hdr.adf[2] = (INT32) 6;        // Matlab 32-bit unsigned integer data type value
  hdr.adf[3] = (INT32) 8;        // 8 bytes of data to follow
  hdr.adf[4] = (INT32) 6;        // Double-precision array, no array flags set
  hdr.adf[5] = (INT32) 0;        // 4 bytes undefined
  // 2. Array Dimensions
  hdr.adf[6] = (INT32) 5;        // Matlab 32-bit signed integer data type value
  hdr.adf[7] = (INT32) 8;        // 8 bytes of data to follow (2D array)
  hdr.adf[8] = (INT32) chans;    // This is the number of rows
  hdr.adf[9] = (INT32) 0;        // This is the number of columns

  // 3. Array Name
  // We'll use fileName for the matlab array name (as well as the file name).
  // If fileName is 4 characters or less, we have to use a compressed data element
  // format for the array name data element.  Otherwise, the array name must
  // be formatted in 8-byte increments (up to 31 characters + NULL).
  namelen = strlen(fileName);
  if (strstr(fileName,".mat")) namelen -= 4;
  if (namelen > 31) { // Check length of array name
    namelen = 31;
    printf("File name too long for MATLAB array name.\n");
    printf("Using first 31 characters only.\n");
  }
  
  strncpy(arrayName,fileName,namelen);
  if (namelen > 4) {
    hdr.adf[10] = 1;        // Matlab 8-bit signed integer data type value
  }
  else { // Compressed data element format
    hdr.adf[10] = namelen;
    hdr.adf[10] <<=16;
    hdr.adf[10] += 1;
  }
  headsize = 40;            // Number of bytes in data element so far

  // Format file name
  strcpy(tempName,fileName);
  if (strstr(tempName,".mat")==NULL) strcat(tempName,".mat");

  // Open file and write fixed header information
  fd = fopen(tempName,"w+b");
  if (!fd)   {
    sprintf(msg, "MatWvOut: Could not create soundfile: %s\n", tempName);
    throw StkError(msg, StkError::FILE_ERROR);
  }

  arrayName[namelen] = '\0';
  printf("\nCreating MAT-file (%s) with MATLAB array: %s\n", tempName, arrayName);
  fwrite(&hdr,sizeof(char),172,fd); // Write the fixed portion of the header

  // Write MATLAB array name
  if (namelen > 4) {
    fwrite(&namelen,sizeof(INT32),1,fd); // Size of array name in bytes (chars)
    fwrite(arrayName,sizeof(char),namelen,fd);
    tmp = (INT32) ceil((float)namelen/8);
    fseek(fd,tmp*8-namelen,SEEK_CUR);
    headsize += tmp*8;
  }
  else { // Compressed data element format
    fwrite(arrayName,sizeof(char),namelen,fd);
    tmp = 4-namelen;
    fseek(fd,tmp,SEEK_CUR);
  }

  // Finish writing known header information
  tmp = 9;        // Matlab IEEE 754 double data type
  fwrite(&tmp,sizeof(INT32),1,fd);
  tmp = 0;        // Size of real part subelement in bytes (8 per sample)
  fwrite(&tmp,sizeof(INT32),1,fd);
  headsize += 8;  // Total number of bytes in data element so far

  fseek(fd,132,SEEK_SET);
  fwrite(&headsize,sizeof(INT32),1,fd); // Write header size ... will update at end
  fseek(fd,0,SEEK_END);

  return fd;
}

MatWvOut :: MatWvOut(char *fileName, int chans)
{
  char msg[256];
  if (chans < 1) {
    sprintf(msg, "MatWvOut: number of channels = %d not supported!\n", chans);
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }
  channels = chans;
  fd = openMatFile(channels,fileName);
  data_length = FILE_BUFFER_SIZE*channels;
  matdata = (double *) new double[data_length];
}

MatWvOut :: ~MatWvOut()
{
  double temp;
  INT32 headsize, temp1;

  fwrite(matdata,sizeof(double),counter,fd);
  temp = (double) totalCount * ONE_OVER_SRATE;
  printf("%f Seconds Computed\n\n", temp);

  fseek(fd,164,SEEK_SET); // jump to number of columns
  fwrite(&totalCount,sizeof(INT32),1,fd);

  fseek(fd,132,SEEK_SET);  // jump to header size
  fread(&headsize,sizeof(INT32),1,fd);
  temp1 = headsize;
  headsize += (INT32) (totalCount * 8 * channels);
  fseek(fd,132,SEEK_SET);
  // Write file size (minus some header info)
  fwrite(&headsize,sizeof(INT32),1,fd);

  fseek(fd,temp1+132,SEEK_SET); // jumpt to data size (in bytes)
  temp1 = totalCount * 8 * channels;
  fwrite(&temp1,sizeof(INT32),1,fd);
  fclose(fd);

  if (matdata) {
    delete [ ] matdata;
    matdata = 0;
  }
}

void MatWvOut :: tick(MY_FLOAT sample)
{
  for (int i=0;i<channels;i++)
    matdata[counter++] = (double) (sample);

  totalCount++;
  if (counter == data_length) {
    fwrite(matdata,sizeof(double),data_length,fd);
    counter = 0;
  }
}

void MatWvOut :: mtick(MY_MULTI samples)
{
  for (int i=0;i<channels;i++)
    matdata[counter++] = (double) *samples++;

  totalCount++;
  if (counter == data_length) {
    fwrite(matdata,sizeof(double),data_length,fd);
    counter = 0;
  }
}
