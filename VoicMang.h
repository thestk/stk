/******************************************/
/*  Simple Voice Manager (Mangler)        */
/*  for ToolKit96, 1996 Perry R. Cook     */
/*  Princeton University                  */
/*                                        */
/*  Make one of these by telling it the   */
/*  maximum number of voices you'll want, */
/*  and also what flavor instrument       */
/*  group it will be mangling.  Pipe TSIDI*/
/*  messages into it and it will return   */
/*  the mixed channel signal each tick.   */
/*  For multi-channel (multi-timbral),    */
/*  make one for each channel and mix     */
/*  their outputs.                        */
/*                                        */
/*  Each note on returns a unique tag,    */
/*  (credits to the NeXT MusicKit here),  */
/*  so you can send control changes to    */
/*  unique instances of instruments       */
/*  within an ensemble.                   */
/*                                        */
/*  TSIDI (ToolKit Synthesis Instrument   */
/*  Digital Interfaceis like MIDI, but    */
/*  allows for floating point control     */
/*  changes, note numbers, etc. Example:  */
/*  noteOn(1,60.01,111.132) plays a sharp */      
/*  middle C with a velocity of 111.132   */
/*                                        */
/******************************************/

#include "Object.h"

#if !defined(__VoicMang_h)
#define __VoicMang_h

#include "Instrmnt.h"

#define __VMang_MAX_ 8

class VoicMang : public Object
{
  protected:  
    char InstrumentType[16];              // Instrument Flavor
    int  max_voices;                                
    long newTag;                          // Unique NoteOn tag counter
    Instrmnt *instrument[__VMang_MAX_];   // The actual Instruments
    long voicesOn[__VMang_MAX_];          // Tags of Sounding Notes
    int  notesOn[__VMang_MAX_];           // Note Numbers On
    long mute_time;                       // Instrument time to shut up
    MY_FLOAT freqBases[__VMang_MAX_];     // Indiv. Pitch Bend Multipliers
    MY_FLOAT frequencies[__VMang_MAX_];   // Indiv. Sounding Frequencies
    MY_FLOAT pitch_bend;                  // Channel Pitch Bend Mult.
  public:
    VoicMang(int maxVoices, char *instrType);
    ~VoicMang();
    long noteOn(MY_FLOAT noteNum, MY_FLOAT amp);
    long noteOnF(MY_FLOAT freq, MY_FLOAT amp);
    void noteOff(long tag, MY_FLOAT amp);
    int noteOffN(int note_num, MY_FLOAT amp);
    long oldestVoice();
    void kill(long tag);
    void controlChange(int number, MY_FLOAT value);
    void controlChange(long tag, int number, MY_FLOAT value);
    void pitchBend(MY_FLOAT value);
    void pitchBend(long tag, MY_FLOAT value);
    MY_FLOAT tick();
};

#endif
