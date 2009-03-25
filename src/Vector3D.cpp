/***************************************************/
/*! \class Vector3D
    \brief STK 3D vector class.

    This class implements a three-dimensional vector.

    by Perry R. Cook, 1995 - 2004.
*/
/***************************************************/

#include "Vector3D.h"
#include <math.h>

Vector3D :: Vector3D(StkFloat initX, StkFloat initY, StkFloat initZ)
{
  myX_ = initX;
  myY_ = initY;
  myZ_ = initZ;
}

Vector3D :: ~Vector3D()
{
}

StkFloat Vector3D :: getX()
{
  return myX_;
}

StkFloat Vector3D :: getY()
{
  return myY_;
}

StkFloat Vector3D :: getZ()
{
  return myZ_;
}

StkFloat Vector3D :: getLength()
{
  StkFloat temp;
  temp = myX_ * myX_;
  temp += myY_ * myY_;
  temp += myZ_ * myZ_;
  temp = sqrt(temp);
  return temp;
}

void Vector3D :: setXYZ(StkFloat x, StkFloat y, StkFloat z)
{
  myX_ = x;
  myY_ = y;
  myZ_ = z;
};

void Vector3D :: setX(StkFloat x)
{
  myX_ = x;
}

void Vector3D :: setY(StkFloat y)
{
  myY_ = y;
}

void Vector3D :: setZ(StkFloat z)
{
  myZ_ = z;
}


