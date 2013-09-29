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

namespace stk {

Mesh2D :: Mesh2D( short nX, short nY )
{
  this->setNX(nX);
  this->setNY(nY);

  StkFloat pole = 0.05;

  short i;
  for (i=0; i<NYMAX; i++) {
    filterY_[i].setPole( pole );
    filterY_[i].setGain( 0.99 );
  }

  for (i=0; i<NXMAX; i++) {
    filterX_[i].setPole( pole );
    filterX_[i].setGain( 0.99 );
  }

  this->clearMesh();

  counter_=0;
  xInput_ = 0;
  yInput_ = 0;
}

Mesh2D :: ~Mesh2D( void )
{
}

void Mesh2D :: clear( void )
{
  this->clearMesh();

  short i;
  for (i=0; i<NY_; i++)
    filterY_[i].clear();

  for (i=0; i<NX_; i++)
    filterX_[i].clear();

  counter_=0;
}

void Mesh2D :: clearMesh( void )
{
  int x, y;
  for (x=0; x<NXMAX-1; x++) {
    for (y=0; y<NYMAX-1; y++) {
      v_[x][y] = 0;
    }
  }
  for (x=0; x<NXMAX; x++) {
    for (y=0; y<NYMAX; y++) {

      vxp_[x][y] = 0;
      vxm_[x][y] = 0;
      vyp_[x][y] = 0;
      vym_[x][y] = 0;

      vxp1_[x][y] = 0;
      vxm1_[x][y] = 0;
      vyp1_[x][y] = 0;
      vym1_[x][y] = 0;
    }
  }
}

StkFloat Mesh2D :: energy( void )
{
  // Return total energy contained in wave variables Note that some
  // energy is also contained in any filter delay elements.

  int x, y;
  StkFloat t;
  StkFloat e = 0;
  if ( counter_ & 1 ) { // Ready for Mesh2D::tick1() to be called.
    for (x=0; x<NX_; x++) {
      for (y=0; y<NY_; y++) {
        t = vxp1_[x][y];
        e += t*t;
        t = vxm1_[x][y];
        e += t*t;
        t = vyp1_[x][y];
        e += t*t;
        t = vym1_[x][y];
        e += t*t;
      }
    }
  }
  else { // Ready for Mesh2D::tick0() to be called.
    for (x=0; x<NX_; x++) {
      for (y=0; y<NY_; y++) {
        t = vxp_[x][y];
        e += t*t;
        t = vxm_[x][y];
        e += t*t;
        t = vyp_[x][y];
        e += t*t;
        t = vym_[x][y];
        e += t*t;
      }
    }
  }

  return(e);
}

void Mesh2D :: setNX( short lenX )
{
  NX_ = lenX;
  if ( lenX < 2 ) {
    errorString_ << "Mesh2D::setNX(" << lenX << "): Minimum length is 2!";
    handleError( StkError::WARNING );
    NX_ = 2;
  }
  else if ( lenX > NXMAX ) {
    errorString_ << "Mesh2D::setNX(" << lenX << "): Maximum length is " << NXMAX << '!';;
    handleError( StkError::WARNING );
    NX_ = NXMAX;
  }
}

void Mesh2D :: setNY( short lenY )
{
  NY_ = lenY;
  if ( lenY < 2 ) {
    errorString_ << "Mesh2D::setNY(" << lenY << "): Minimum length is 2!";
    handleError( StkError::WARNING );
    NY_ = 2;
  }
  else if ( lenY > NYMAX ) {
    errorString_ << "Mesh2D::setNY(" << lenY << "): Maximum length is " << NXMAX << '!';;
    handleError( StkError::WARNING );
    NY_ = NYMAX;
  }
}

void Mesh2D :: setDecay( StkFloat decayFactor )
{
  StkFloat gain = decayFactor;
  if ( decayFactor < 0.0 ) {
    errorString_ << "Mesh2D::setDecay: decayFactor value is less than 0.0!";
    handleError( StkError::WARNING );
    gain = 0.0;
  }
  else if ( decayFactor > 1.0 ) {
    errorString_ << "Mesh2D::setDecay decayFactor value is greater than 1.0!";
    handleError( StkError::WARNING );
    gain = 1.0;
  }

  int i;
  for (i=0; i<NYMAX; i++)
    filterY_[i].setGain( gain );

  for (i=0; i<NXMAX; i++)
    filterX_[i].setGain( gain );
}

void Mesh2D :: setInputPosition( StkFloat xFactor, StkFloat yFactor )
{
  if ( xFactor < 0.0 ) {
    errorString_ << "Mesh2D::setInputPosition xFactor value is less than 0.0!";
    handleError( StkError::WARNING );
    xInput_ = 0;
  }
  else if ( xFactor > 1.0 ) {
    errorString_ << "Mesh2D::setInputPosition xFactor value is greater than 1.0!";
    handleError( StkError::WARNING );
    xInput_ = NX_ - 1;
  }
  else
    xInput_ = (short) (xFactor * (NX_ - 1));

  if ( yFactor < 0.0 ) {
    errorString_ << "Mesh2D::setInputPosition yFactor value is less than 0.0!";
    handleError( StkError::WARNING );
    yInput_ = 0;
  }
  else if ( yFactor > 1.0 ) {
    errorString_ << "Mesh2D::setInputPosition yFactor value is greater than 1.0!";
    handleError( StkError::WARNING );
    yInput_ = NY_ - 1;
  }
  else
    yInput_ = (short) (yFactor * (NY_ - 1));
}

void Mesh2D :: noteOn( StkFloat frequency, StkFloat amplitude )
{
  // Input at corner.
  if ( counter_ & 1 ) {
    vxp1_[xInput_][yInput_] += amplitude;
    vyp1_[xInput_][yInput_] += amplitude;
  }
  else {
    vxp_[xInput_][yInput_] += amplitude;
    vyp_[xInput_][yInput_] += amplitude;
  }

#if defined(_STK_DEBUG_)
  errorString_ << "Mesh2D::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Mesh2D :: noteOff( StkFloat amplitude )
{
#if defined(_STK_DEBUG_)
  errorString_ << "Mesh2D::NoteOff: amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

StkFloat Mesh2D :: inputTick( StkFloat input )
{
  if ( counter_ & 1 ) {
    vxp1_[xInput_][yInput_] += input;
    vyp1_[xInput_][yInput_] += input;
    lastFrame_[0] = tick1();
  }
  else {
    vxp_[xInput_][yInput_] += input;
    vyp_[xInput_][yInput_] += input;
    lastFrame_[0] = tick0();
  }

  counter_++;
  return lastFrame_[0];
}

StkFloat Mesh2D :: tick( unsigned int )
{
  lastFrame_[0] = ((counter_ & 1) ? this->tick1() : this->tick0());
  counter_++;
  return lastFrame_[0];
}

const StkFloat VSCALE = 0.5;

StkFloat Mesh2D :: tick0( void )
{
  int x, y;
  StkFloat outsamp = 0;

  // Update junction velocities.
  for (x=0; x<NX_-1; x++) {
    for (y=0; y<NY_-1; y++) {
      v_[x][y] = ( vxp_[x][y] + vxm_[x+1][y] + 
		  vyp_[x][y] + vym_[x][y+1] ) * VSCALE;
    }
  }    

  // Update junction outgoing waves, using alternate wave-variable buffers.
  for (x=0; x<NX_-1; x++) {
    for (y=0; y<NY_-1; y++) {
      StkFloat vxy = v_[x][y];
      // Update positive-going waves.
      vxp1_[x+1][y] = vxy - vxm_[x+1][y];
      vyp1_[x][y+1] = vxy - vym_[x][y+1];
      // Update minus-going waves.
      vxm1_[x][y] = vxy - vxp_[x][y];
      vym1_[x][y] = vxy - vyp_[x][y];
    }
  }    

  // Loop over velocity-junction boundary faces, update edge
  // reflections, with filtering.  We're only filtering on one x and y
  // edge here and even this could be made much sparser.
  for (y=0; y<NY_-1; y++) {
    vxp1_[0][y] = filterY_[y].tick(vxm_[0][y]);
    vxm1_[NX_-1][y] = vxp_[NX_-1][y];
  }
  for (x=0; x<NX_-1; x++) {
    vyp1_[x][0] = filterX_[x].tick(vym_[x][0]);
    vym1_[x][NY_-1] = vyp_[x][NY_-1];
  }

  // Output = sum of outgoing waves at far corner.  Note that the last
  // index in each coordinate direction is used only with the other
  // coordinate indices at their next-to-last values.  This is because
  // the "unit strings" attached to each velocity node to terminate
  // the mesh are not themselves connected together.
  outsamp = vxp_[NX_-1][NY_-2] + vyp_[NX_-2][NY_-1];

  return outsamp;
}

StkFloat Mesh2D :: tick1( void )
{
  int x, y;
  StkFloat outsamp = 0;

  // Update junction velocities.
  for (x=0; x<NX_-1; x++) {
    for (y=0; y<NY_-1; y++) {
      v_[x][y] = ( vxp1_[x][y] + vxm1_[x+1][y] + 
		  vyp1_[x][y] + vym1_[x][y+1] ) * VSCALE;
    }
  }

  // Update junction outgoing waves, 
  // using alternate wave-variable buffers.
  for (x=0; x<NX_-1; x++) {
    for (y=0; y<NY_-1; y++) {
      StkFloat vxy = v_[x][y];

      // Update positive-going waves.
      vxp_[x+1][y] = vxy - vxm1_[x+1][y];
      vyp_[x][y+1] = vxy - vym1_[x][y+1];

      // Update minus-going waves.
      vxm_[x][y] = vxy - vxp1_[x][y];
      vym_[x][y] = vxy - vyp1_[x][y];
    }
  }

  // Loop over velocity-junction boundary faces, update edge
  // reflections, with filtering.  We're only filtering on one x and y
  // edge here and even this could be made much sparser.
  for (y=0; y<NY_-1; y++) {
    vxp_[0][y] = filterY_[y].tick(vxm1_[0][y]);
    vxm_[NX_-1][y] = vxp1_[NX_-1][y];
  }
  for (x=0; x<NX_-1; x++) {
    vyp_[x][0] = filterX_[x].tick(vym1_[x][0]);
    vym_[x][NY_-1] = vyp1_[x][NY_-1];
  }

  // Output = sum of outgoing waves at far corner.
  outsamp = vxp1_[NX_-1][NY_-2] + vyp1_[NX_-2][NY_-1];

  return outsamp;
}

void Mesh2D :: controlChange( int number, StkFloat value )
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "Mesh2D::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "Mesh2D::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

  if (number == 2) // 2
    this->setNX( (short) (norm * (NXMAX-2) + 2) );
  else if (number == 4) // 4
    this->setNY( (short) (norm * (NYMAX-2) + 2) );
  else if (number == 11) // 11
    this->setDecay( 0.9 + (norm * 0.1) );
  else if (number == __SK_ModWheel_) // 1
    this->setInputPosition( norm, norm );
  else {
    errorString_ << "Mesh2D::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "Mesh2D::controlChange: number = " << number << ", value = " << value << ".";
    handleError( StkError::DEBUG_WARNING );
#endif
}

} // stk namespace
