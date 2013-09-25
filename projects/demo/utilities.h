// Miscellaneous parsing and error functions for use with STK demo program.
//
// Gary P. Scavone, 1999.

#include "Instrmnt.h"
#include "WvOut.h"

int voiceByNumber(int number, Instrmnt **instrument);

int voiceByName(char *name, Instrmnt **instrument);

void usage(char *function);

int checkArgs(int numArgs, char *args[]);

int parseArgs(int numArgs, char *args[], WvOut **output);
