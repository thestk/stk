// Miscellaneous parsing and error functions for use with syntmono.
//
// Gary P. Scavone, 1999.

#include "../STK/Instrmnt.h"

int newInstByNum(int instNum);

int newInstByName(char *instName);

void usage(char *func);

int checkArgs(int numArgs, char *args[]);

int parseArgs(int numArgs, char *args[]);
