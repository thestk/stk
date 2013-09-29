/***************************************************/
/*! \class Sphere
    \brief STK sphere class.

    This class implements a spherical ball with
    radius, mass, position, and velocity parameters.

    by Perry R. Cook, 1995 - 2002.
*/
/***************************************************/

#include "Sphere.h"
#include <stdio.h>
#include <math.h>

Sphere::Sphere(double initRadius)
{
  myRadius = initRadius;
  myMass = 1.0;
  myPosition = new Vector3D(0, 0, 0);
  myVelocity = new Vector3D(0, 0, 0);
};

Sphere::~Sphere()
{
  delete myPosition;
  delete myVelocity;
}

void Sphere::setPosition(double anX, double aY, double aZ)
{
  myPosition->setXYZ(anX, aY, aZ);
};

void Sphere::setVelocity(double anX, double aY, double aZ)
{
  myVelocity->setXYZ(anX, aY, aZ);
};

void Sphere::setRadius(double aRadius)
{
  myRadius = aRadius;
};

void Sphere::setMass(double aMass)
{
  myMass = aMass;
};

Vector3D* Sphere::getPosition()
{
  return myPosition;
};

Vector3D* Sphere::getRelativePosition(Vector3D* aPosition)
{
  workingVector.setXYZ(aPosition->getX() - myPosition->getX(),
                       aPosition->getY() - myPosition->getY(),  
                       aPosition->getZ() - myPosition->getZ());
  return &workingVector;
};

double Sphere::getVelocity(Vector3D* aVelocity)
{
  aVelocity->setXYZ(myVelocity->getX(), myVelocity->getY(), myVelocity->getZ());
  return myVelocity->getLength();
};

double Sphere::isInside(Vector3D *aPosition)
{
  // Return directed distance from aPosition to spherical boundary ( <
  // 0 if inside).
  double distance;
  Vector3D *tempVector;

  tempVector = this->getRelativePosition(aPosition);
  distance = tempVector->getLength();
  return distance - myRadius;
};

double Sphere::getRadius()
{
  return myRadius;
};

double Sphere::getMass()
{
  return myMass;
};

void Sphere::addVelocity(double anX, double aY, double aZ)
{
  myVelocity->setX(myVelocity->getX() + anX);
  myVelocity->setY(myVelocity->getY() + aY);
  myVelocity->setZ(myVelocity->getZ() + aZ);
}

void Sphere::tick(double timeIncrement)
{
  myPosition->setX(myPosition->getX() + (timeIncrement * myVelocity->getX()));
  myPosition->setY(myPosition->getY() + (timeIncrement * myVelocity->getY()));
  myPosition->setZ(myPosition->getZ() + (timeIncrement * myVelocity->getZ()));
};

