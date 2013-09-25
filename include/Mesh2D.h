/***************************************************/
/*! \class Mesh2D
    \brief Two-dimensional rectilinear waveguide mesh class.

    This class implements a rectilinear,
    two-dimensional digital waveguide mesh
    structure.  For details, see Van Duyne and
    Smith, "Physical Modeling with the 2-D Digital
    Waveguide Mesh", Proceedings of the 1993
    International Computer Music Conference.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - X Dimension = 2
       - Y Dimension = 4
       - Mesh Decay = 11
       - X-Y Input Position = 1

    by Julius Smith, 2000 - 2002.
    Revised by Gary Scavone for STK, 2002.
*/
/***************************************************/

#if !defined(__MESH2D_H)
#define __MESH2D_H

#include "Instrmnt.h"
#include "OnePole.h"

#define NXMAX ((short)(12))
#define NYMAX ((short)(12))

class Mesh2D : public Instrmnt
{
 public:
  //! Class constructor, taking the x and y dimensions in samples.
  Mesh2D(short nX, short nY);

  //! Class destructor.
  ~Mesh2D();

  //! Reset and clear all internal state.
  void clear(); 

  //! Set the x dimension size in samples.
  void setNX(short lenX);

  //! Set the y dimension size in samples.
  void setNY(short lenY);

  //! Set the x, y input position on a 0.0 - 1.0 scale.
  void setInputPosition(MY_FLOAT xFactor, MY_FLOAT yFactor);

  //! Set the loss filters gains (0.0 - 1.0).
  void setDecay(MY_FLOAT decayFactor);

  //! Impulse the mesh with the given amplitude (frequency ignored).
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay) ... currently ignored.
  void noteOff(MY_FLOAT amplitude);

  //! Calculate and return the signal energy stored in the mesh.
  MY_FLOAT energy();

  //! Compute one output sample, without adding energy to the mesh.
  MY_FLOAT tick();

  //! Input a sample to the mesh and compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 protected:

  MY_FLOAT tick0();
  MY_FLOAT tick1();
  void clearMesh();

  short NX, NY;
  short xInput, yInput;
  OnePole *filterX[NXMAX];
  OnePole *filterY[NYMAX];
  MY_FLOAT v[NXMAX-1][NYMAX-1]; // junction velocities
  MY_FLOAT vxp[NXMAX][NYMAX]; // positive-x velocity wave
  MY_FLOAT vxm[NXMAX][NYMAX]; // negative-x velocity wave
  MY_FLOAT vyp[NXMAX][NYMAX]; // positive-y velocity wave
  MY_FLOAT vym[NXMAX][NYMAX]; // negative-y velocity wave

  // Alternate buffers
  MY_FLOAT vxp1[NXMAX][NYMAX]; // positive-x velocity wave
  MY_FLOAT vxm1[NXMAX][NYMAX]; // negative-x velocity wave
  MY_FLOAT vyp1[NXMAX][NYMAX]; // positive-y velocity wave
  MY_FLOAT vym1[NXMAX][NYMAX]; // negative-y velocity wave

  int counter; // time in samples


};

#endif
