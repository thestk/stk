/***************************************************/
/*! \class Table
    \brief STK table lookup class.

    This class loads a table of floating-point
    doubles, which are assumed to be in big-endian
    format.  Linear interpolation is performed for
    fractional lookup indexes.

    An StkError will be thrown if the table file
    is not found.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Table.h"
#include <fstream> 
#include <sys/stat.h>

Table :: Table( std::string fileName)
{
  // Use the system call "stat" to determine the file length
  struct stat filestat;
  if ( stat( fileName.c_str(), &filestat ) == -1 ) {
    errorString_ << "Table: Couldn't stat or find file (" << fileName << ").";
    handleError( StkError::FILE_NOT_FOUND );
  }
  length_ = (long) filestat.st_size / 8;  // length in 8-byte samples

  // Open the file and read samples into data[]
  FILE *fd;
  fd = fopen(fileName.c_str(),"rb");
  if (!fd) {
    errorString_ << "Table::Table: unable to open or find file (" << fileName << ")";
    handleError( StkError::FILE_NOT_FOUND );
  }

  data_.resize( length_, 0.0 );

  // Read samples into data
  long i = 0;
  double temp;
  while ( fread( &temp, 8, 1, fd ) ) {
#ifdef __LITTLE_ENDIAN__
    swap64( (unsigned char *)&temp );
#endif
    data_[i++] = (StkFloat) temp;
  }
  fclose(fd);

  lastOutput_ = 0.0;
}

Table :: ~Table()
{
}

long Table :: getLength() const
{
  return length_;
}

StkFloat Table :: tick(StkFloat index)
{
  StkFloat alpha;
  long temp;

  if ( index > length_-1 ) {
    errorString_ << "Table: Index (" << index << ") exceeds table length ... sticking at end!\n";
    handleError( StkError::WARNING );
    index = length_-1;
  }
  else if (index < 0.0) {
    errorString_ << "Table: Index (" << index << ") is less than zero ... setting to zero!\n";
    handleError( StkError::WARNING );
    index = 0.0;
  }

  // Index in range 0 to length-1
  temp = (long) index;                  // Integer part of index
  alpha = index - (StkFloat) temp;      // Fractional part of index
  if (alpha > 0.0) {                    // Do linear interpolation
    lastOutput_ = data_[temp];
    lastOutput_ += (alpha*(data_[temp+1] - lastOutput_));
  }
  else lastOutput_ = data_[temp];

  return lastOutput_;
}

StkFloat *Table :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Function::tick( vector, vectorSize );
}

StkFrames& Table :: tick( StkFrames& frames, unsigned int channel )
{
  return Function::tick( frames, channel );
}
