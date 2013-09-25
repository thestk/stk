/*************************************************/
/*
  STK Error Handling Class
  by Gary P. Scavone, 2000.
  
  This is a fairly abstract exception handling
  class.  There could be sub-classes to take
  care of more specific error conditions ... or not.
*/
/*************************************************/

#if !defined(__StkError_h)
#define __StkError_h

#include "Object.h"

class StkError : public Object
{
public:
  enum TYPE { UNSPECIFIED,
              FUNCTION_SYNTAX,
              FILE_NOT_FOUND,
              FILE_ERROR,
              PROCESS_THREAD,
              PROCESS_SOCKET,
              PROCESS_SOCKET_IPADDR,
              SOUNDCARD_NOT_FOUND,
              SOUNDCARD_CAPS,
              SOUNDCARD_CONTROL,
              MIDICARD_NOT_FOUND,
              MIDICARD_CAPS,
              MIDICARD_CONTROL
  };

protected:
  char errormsg[256];
  TYPE type;

public:
	StkError(const char *p, TYPE tipe = StkError::UNSPECIFIED);
  virtual ~StkError(void);
  virtual void printMessage(void);
  virtual const TYPE& getType(void) { return type; }
};

#endif
