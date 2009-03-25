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

#include "Mesh2D.h"
#include "SKINI.msg"
#include <stdlib.h>

Mesh2D :: Mesh2D(short nX, short nY)
{
  this->setNX(nX);
  this->setNY(nY);

  MY_FLOAT pole = 0.05;

  short i;
  for (i=0; i<NYMAX; i++) {
    filterY[i] = new OnePole(pole);
    filterY[i]->setGain(0.99);
  }

  for (i=0; i<NXMAX; i++) {
    filterX[i] = new OnePole(pole);
    filterX[i]->setGain(0.99);
  }

  this->clearMesh();

  counter=0;
  xInput = 0;
  yInput = 0;
}

Mesh2D :: ~Mesh2D()
{
  short i;
  for (i=0; i<NYMAX; i++)
    delete filterY[i];

  for (i=0; i<NXMAX; i++)
    delete filterX[i];
}

void Mesh2D :: clear()
{
  this->clearMesh();

  short i;
  for (i=0; i<NY; i++)
    filterY[i]->clear();

  for (i=0; i<NX; i++)
    filterX[i]->clear();

  counter=0;
}

void Mesh2D :: clearMesh()
{
  int x, y;
  for (x=0; x<NXMAX-1; x++) {
    for (y=0; y<NYMAX-1; y++) {
      v[x][y] = 0;
    }
  }
  for (x=0; x<NXMAX; x++) {
    for (y=0; y<NYMAX; y++) {

      vxp[x][y] = 0;
      vxm[x][y] = 0;
      vyp[x][y] = 0;
      vym[x][y] = 0;

      vxp1[x][y] = 0;
      vxm1[x][y] = 0;
      vyp1[x][y] = 0;
      vym1[x][y] = 0;
    }
  }
}

MY_FLOAT Mesh2D :: energy()
{
  // Return total energy contained in wave variables Note that some
  // energy is also contained in any filter delay elements.

  int x, y;
  MY_FLOAT t;
  MY_FLOAT e = 0;
  if ( counter & 1 ) { // Ready for Mesh2D::tick1() to be called.
    for (x=0; x<NX; x++) {
      for (y=0; y<NY; y++) {
        t = vxp1[x][y];
        e += t*t;
        t = vxm1[x][y];
        e += t*t;
        t = vyp1[x][y];
        e += t*t;
        t = vym1[x][y];
        e += t*t;
      }
    }
  }
  else { // Ready for Mesh2D::tick0() to be called.
    for (x=0; x<NX; x++) {
      for (y=0; y<NY; y++) {
        t = vxp[x][y];
        e += t*t;
        t = vxm[x][y];
        e += t*t;
        t = vyp[x][y];
        e += t*t;
        t = vym[x][y];
        e += t*t;
      }
    }
  }

  return(e);
}

void Mesh2D :: setNX(short lenX)
{
  NX = lenX;
  if ( lenX < 2 ) {
    std::cerr << "Mesh2D::setNX(" << lenX << "): Minimum length is 2!" << std::endl;
    NX = 2;
  }
  else if ( lenX > NXMAX ) {
    std::cerr << "Mesh2D::setNX(" << lenX << "): Maximum length is " << NXMAX << "!" << std::endl;
    NX = NXMAX;
  }
}

void Mesh2D :: setNY(short lenY)
{
  NY = lenY;
  if ( lenY < 2 ) {
    std::cerr << "Mesh2D::setNY(" << lenY << "): Minimum length is 2!" << std::endl;
    NY = 2;
  }
  else if ( lenY > NYMAX ) {
    std::cerr << "Mesh2D::setNY(" << lenY << "): Maximum length is " << NYMAX << "!" << std::endl;
    NY = NYMAX;
  }
}

void Mesh2D :: setDecay(MY_FLOAT decayFactor)
{
  MY_FLOAT gain = decayFactor;
  if ( decayFactor < 0.0 ) {
    std::cerr << "Mesh2D::setDecay decayFactor value is less than 0.0!" << std::endl;
    gain = 0.0;
  }
  else if ( decayFactor > 1.0 ) {
    std::cerr << "Mesh2D::setDecay decayFactor value is greater than 1.0!" << std::endl;
    gain = 1.0;
  }

  int i;
  for (i=0; i<NYMAX; i++)
    filterY[i]->setGain(gain);

  for (i=0; i<NXMAX; i++)
    filterX[i]->setGain(gain);
}

void Mesh2D :: setInputPosition(MY_FLOAT xFactor, MY_FLOAT yFactor)
{
  if ( xFactor < 0.0 ) {
    std::cerr << "Mesh2D::setInputPosition xFactor value is less than 0.0!" << std::endl;
    xInput = 0;
  }
  else if ( xFactor > 1.0 ) {
    std::cerr << "Mesh2D::setInputPosition xFactor value is greater than 1.0!" << std::endl;
    xInput = NX - 1;
  }
  else
    xInput = (short) (xFactor * (NX - 1));

  if ( yFactor < 0.0 ) {
    std::cerr << "Mesh2D::setInputPosition yFactor value is less than 0.0!" << std::endl;
    yInput = 0;
  }
  else if ( yFactor > 1.0 ) {
    std::cerr << "Mesh2D::setInputPosition yFactor value is greater than 1.0!" << std::endl;
    yInput = NY - 1;
  }
  else
    yInput = (short) (yFactor * (NY - 1));
}

void Mesh2D :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  // Input at corner.
  if ( counter & 1 ) {
    vxp1[xInput][yInput] += amplitude;
    vyp1[xInput][yInput] += amplitude;
  }
  else {
    vxp[xInput][yInput] += amplitude;
    vyp[xInput][yInput] += amplitude;
  }

#if defined(_STK_DEBUG_)
  std::cerr << "Mesh2D: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Mesh2D :: noteOff(MY_FLOAT amplitude)
{
#if defined(_STK_DEBUG_)
  std::cerr << "Mesh2D: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT Mesh2D :: tick(MY_FLOAT input)
{
  if ( counter & 1 ) {
    vxp1[xInput][yInput] += input;
    vyp1[xInput][yInput] += input;
    lastOutput = tick1();
  }
  else {
    vxp[xInput][yInput] += input;
    vyp[xInput][yInput] += input;
    lastOutput = tick0();
  }

  counter++;
  return lastOutput;
}

MY_FLOAT Mesh2D :: tick()
{
  lastOutput = ((counter & 1) ? this->tick1() : this->tick0());
  counter++;
  return lastOutput;
}

#define VSCALE ((MY_FLOAT) (0.5))

MY_FLOAT Mesh2D :: tick0()
{
  int x, y;
  MY_FLOAT outsamp = 0;

  // Update junction velocities.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      v[x][y] = ( vxp[x][y] + vxm[x+1][y] + 
		  vyp[x][y] + vym[x][y+1] ) * VSCALE;
    }
  }    

  // Update junction outgoing waves, using alternate wave-variable buffers.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      MY_FLOAT vxy = v[x][y];
      // Update positive-going waves.
      vxp1[x+1][y] = vxy - vxm[x+1][y];
      vyp1[x][y+1] = vxy - vym[x][y+1];
      // Update minus-going waves.
      vxm1[x][y] = vxy - vxp[x][y];
      vym1[x][y] = vxy - vyp[x][y];
    }
  }    

  // Loop over velocity-junction boundary faces, update edge
  // reflections, with filtering.  We're only filtering on one x and y
  // edge here and even this could be made much sparser.
  for (y=0; y<NY-1; y++) {
    vxp1[0][y] = filterY[y]->tick(vxm[0][y]);
    vxm1[NX-1][y] = vxp[NX-1][y];
  }
  for (x=0; x<NX-1; x++) {
    vyp1[x][0] = filterX[x]->tick(vym[x][0]);
    vym1[x][NY-1] = vyp[x][NY-1];
  }

  // Output = sum of outgoing waves at far corner.  Note that the last
  // index in each coordinate direction is used only with the other
  // coordinate indices at their next-to-last values.  This is because
  // the "unit strings" attached to each velocity node to terminate
  // the mesh are not themselves connected together.
  outsamp = vxp[NX-1][NY-2] + vyp[NX-2][NY-1];

  return outsamp;
}

MY_FLOAT Mesh2D :: tick1()
{
  int x, y;
  MY_FLOAT outsamp = 0;

  // Update junction velocities.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      v[x][y] = ( vxp1[x][y] + vxm1[x+1][y] + 
		  vyp1[x][y] + vym1[x][y+1] ) * VSCALE;
    }
  }

  // Update junction outgoing waves, 
  // using alternate wave-variable buffers.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      MY_FLOAT vxy = v[x][y];

      // Update positive-going waves.
      vxp[x+1][y] = vxy - vxm1[x+1][y];
      vyp[x][y+1] = vxy - vym1[x][y+1];

      // Update minus-going waves.
      vxm[x][y] = vxy - vxp1[x][y];
      vym[x][y] = vxy - vyp1[x][y];
    }
  }

  // Loop over velocity-junction boundary faces, update edge
  // reflections, with filtering.  We're only filtering on one x and y
  // edge here and even this could be made much sparser.
  for (y=0; y<NY-1; y++) {
    vxp[0][y] = filterY[y]->tick(vxm1[0][y]);
    vxm[NX-1][y] = vxp1[NX-1][y];
  }
  for (x=0; x<NX-1; x++) {
    vyp[x][0] = filterX[x]->tick(vym1[x][0]);
    vym[x][NY-1] = vyp1[x][NY-1];
  }

  // Output = sum of outgoing waves at far corner.
  outsamp = vxp1[NX-1][NY-2] + vyp1[NX-2][NY-1];

  return outsamp;
}

void Mesh2D :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "Mesh2D: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "Mesh2D: Control value greater than 128.0!" << std::endl;
  }

  if (number == 2) // 2
    setNX( (short) (norm * (NXMAX-2) + 2) );
  else if (number == 4) // 4
    setNY( (short) (norm * (NYMAX-2) + 2) );
  else if (number == 11) // 11
    setDecay( 0.9 + (norm * 0.1) );
  else if (number == __SK_ModWheel_) // 1
    setInputPosition(norm, norm);
  else if (number == __SK_AfterTouch_Cont_) // 128
    ;
  else
    std::cerr << "Mesh2D: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "Mesh2D: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
