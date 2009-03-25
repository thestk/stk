/***************************************************/
/*! \class Vector3D
    \brief STK 3D vector class.

    This class implements a three-dimensional vector.

    by Perry R. Cook, 1995 - 2002.
*/
/***************************************************/

#include "Vector3D.h"
#include <math.h>

Vector3D :: Vector3D(double initX, double initY, double initZ)
{
  myX = initX;
  myY = initY;
  myZ = initZ;
}

Vector3D :: ~Vector3D()
{
}

double Vector3D :: getX()
{
  return myX;
}

double Vector3D :: getY()
{
  return myY;
}

double Vector3D :: getZ()
{
  return myZ;
}

double Vector3D :: getLength()
{
  double temp;
  temp = myX * myX;
  temp += myY * myY;
  temp += myZ * myZ;
  temp = sqrt(temp);
  return temp;
}

void Vector3D :: setXYZ(double anX, double aY, double aZ)
{
  myX = anX;
  myY = aY;
  myZ = aZ;
};

void Vector3D :: setX(double aval)
{
  myX = aval;
}

void Vector3D :: setY(double aval)
{
  myY = aval;
}

void Vector3D :: setZ(double aval)
{
  myZ = aval;
}


