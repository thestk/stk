/***************************************************/
/*! \class ModalBar
    \brief STK resonant bar instrument class.

    This class implements a number of different
    struck bar instruments.  It inherits from the
    Modal class.

    Control Change Numbers: 
       - Stick Hardness = 2
       - Stick Position = 4
       - Vibrato Gain = 11
       - Vibrato Frequency = 7
       - Direct Stick Mix = 1
       - Volume = 128
       - Modal Presets = 16
         - Marimba = 0
         - Vibraphone = 1
         - Agogo = 2
         - Wood1 = 3
         - Reso = 4
         - Wood2 = 5
         - Beats = 6
         - Two Fixed = 7
         - Clump = 8

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "ModalBar.h"
#include "SKINI.msg"
#include <string.h>
#include <math.h>

ModalBar :: ModalBar()
  : Modal()
{
  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  wave = new WvIn( strcat(file,"rawwaves/marmstk1.raw"), TRUE );
  wave->setRate((MY_FLOAT) 0.5 * 22050.0 / Stk::sampleRate() );

  // Set the resonances for preset 0 (marimba).
  setPreset( 0 );
}

ModalBar :: ~ModalBar()
{
  delete wave;
}

void ModalBar :: setStickHardness(MY_FLOAT hardness)
{
  stickHardness = hardness;
  if ( hardness < 0.0 ) {
    cerr << "ModalBar: setStickHardness parameter is less than zero!" << endl;
    stickHardness = 0.0;
  }
  else if ( hardness > 1.0 ) {
    cerr << "ModalBar: setStickHarness parameter is greater than 1.0!" << endl;
    stickHardness = 1.0;
  }

  wave->setRate( (0.25 * (MY_FLOAT) pow(4.0, stickHardness)) );
  masterGain = 0.1 + (1.8 * stickHardness);
}

void ModalBar :: setStrikePosition(MY_FLOAT position)
{
  strikePosition = position;
  if ( position < 0.0 ) {
    cerr << "ModalBar: setStrikePositions parameter is less than zero!" << endl;
    strikePosition = 0.0;
  }
  else if ( position > 1.0 ) {
    cerr << "ModalBar: setStrikePosition parameter is greater than 1.0!" << endl;
    strikePosition = 1.0;
  }

  // Hack only first three modes.
  MY_FLOAT temp2 = position * PI;
  MY_FLOAT temp = sin(temp2);                                       
  this->setModeGain(0, 0.12 * temp);

  temp = sin(0.05 + (3.9 * temp2));
  this->setModeGain(1,(MY_FLOAT) -0.03 * temp);

  temp = (MY_FLOAT)  sin(-0.05 + (11 * temp2));
  this->setModeGain(2,(MY_FLOAT) 0.11 * temp);
}

void ModalBar :: setPreset(int preset)
{
  // Presets:
  //     First line:  relative modal frequencies (negative number is
  //                  a fixed mode that doesn't scale with frequency
  //     Second line: resonances of the modes
  //     Third line:  mode volumes
  //     Fourth line: stickHardness, strikePosition, and direct stick
  //                  gain (mixed directly into the output
  static MY_FLOAT presets[9][4][4] = { 
    {{1.0, 3.99, 10.65, -2443},		// Marimba
     {0.9996, 0.9994, 0.9994, 0.999},
     {0.04, 0.01, 0.01, 0.008},
     {0.429688, 0.445312, 0.093750}},
    {{1.0, 2.01, 3.9, 14.37}, 		// Vibraphone
     {0.99995, 0.99991, 0.99992, 0.9999},	
     {0.025, 0.015, 0.015, 0.015 },
     {0.390625,0.570312,0.078125}},
    {{1.0, 4.08, 6.669, -3725.0},		// Agogo 
     {0.999, 0.999, 0.999, 0.999},	
     {0.06, 0.05, 0.03, 0.02},
     {0.609375,0.359375,0.140625}},
    {{1.0, 2.777, 7.378, 15.377},		// Wood1
     {0.996, 0.994, 0.994, 0.99},	
     {0.04, 0.01, 0.01, 0.008},
     {0.460938,0.375000,0.046875}},
    {{1.0, 2.777, 7.378, 15.377},		// Reso
     {0.99996, 0.99994, 0.99994, 0.9999},	
     {0.02, 0.005, 0.005, 0.004},
     {0.453125,0.250000,0.101562}},
    {{1.0, 1.777, 2.378, 3.377},		// Wood2
     {0.996, 0.994, 0.994, 0.99},	
     {0.04, 0.01, 0.01, 0.008},
     {0.312500,0.445312,0.109375}},
    {{1.0, 1.004, 1.013, 2.377},		// Beats
     {0.9999, 0.9999, 0.9999, 0.999},	
     {0.02, 0.005, 0.005, 0.004},
     {0.398438,0.296875,0.070312}},
    {{1.0, 4.0, -1320.0, -3960.0},		// 2Fix
     {0.9996, 0.999, 0.9994, 0.999},	
     {0.04, 0.01, 0.01, 0.008},
     {0.453125,0.453125,0.070312}},
    {{1.0, 1.217, 1.475, 1.729},		// Clump
     {0.999, 0.999, 0.999, 0.999},	
     {0.03, 0.03, 0.03, 0.03 },
     {0.390625,0.570312,0.078125}},
  };

  int temp = (preset % 9);
  for (int i=0; i<nModes; i++) {
    this->setRatioAndRadius(i, presets[temp][0][i], presets[temp][1][i]);
    this->setModeGain(i, presets[temp][2][i]);
  }

  this->setStickHardness(presets[temp][3][0]);
  this->setStrikePosition(presets[temp][3][1]);
  directGain = presets[temp][3][2];

  if (temp == 1) // vibraphone
    vibratoGain = 0.2;
  else
    vibratoGain = 0.0;
}

void ModalBar :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "ModalBar: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "ModalBar: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_StickHardness_) // 2
    this->setStickHardness( norm );
  else if (number == __SK_StrikePosition_) // 4
    this->setStrikePosition( norm );
  else if (number == __SK_ProphesyRibbon_) // 16
		this->setPreset((int) value);
  else if (number == __SK_ModWheel_) // 1
    directGain = norm;
  else if (number == 11) // 11
    vibratoGain = norm * 0.3;
  else if (number == __SK_ModFrequency_) // 7
    vibrato->setFrequency( norm * 12.0 );
  else if (number == __SK_AfterTouch_Cont_)	// 128
    envelope->setTarget( norm );
  else
    cerr << "ModalBar: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "ModalBar: controlChange number = " << number << ", value = " << value << endl;
#endif
}
