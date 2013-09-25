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

#if !defined(__MODALBAR_H)
#define __MODALBAR_H

#include "Modal.h"

class ModalBar : public Modal
{
public:
  //! Class constructor.
  ModalBar();

  //! Class destructor.
  ~ModalBar();

  //! Set stick hardness (0.0 - 1.0).
  void setStickHardness(MY_FLOAT hardness);

  //! Set stick position (0.0 - 1.0).
  void setStrikePosition(MY_FLOAT position);

  //! Select a bar preset (currently modulo 9).
  void setPreset(int preset);

  //! Set the modulation (vibrato) depth.
  void setModulationDepth(MY_FLOAT mDepth);

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);
};

#endif
