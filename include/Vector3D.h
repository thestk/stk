/***************************************************/
/*! \class Vector3D
    \brief STK 3D vector class.

    This class implements a three-dimensional vector.

    by Perry R. Cook, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_VECTOR3D_H
#define STK_VECTOR3D_H

#include "Stk.h"

class Vector3D : public Stk
{

public:
  //! Default constructor taking optional initial X, Y, and Z values.
  Vector3D(StkFloat initX=0.0, StkFloat initY=0.0, StkFloat initZ=0.0);

  //! Class destructor.
  ~Vector3D();

  //! Get the current X value.
  StkFloat getX();

  //! Get the current Y value.
  StkFloat getY();

  //! Get the current Z value.
  StkFloat getZ();

  //! Calculate the vector length.
  StkFloat getLength();

  //! Set the X, Y, and Z values simultaniously.
  void setXYZ(StkFloat x, StkFloat y, StkFloat z);

  //! Set the X value.
  void setX(StkFloat x);

  //! Set the Y value.
  void setY(StkFloat y);

  //! Set the Z value.
  void setZ(StkFloat z);

protected:
  StkFloat myX_;
  StkFloat myY_;
  StkFloat myZ_;
};

#endif
