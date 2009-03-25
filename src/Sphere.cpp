/***************************************************/
/*! \class Sphere
    \brief STK sphere class.

    This class implements a spherical ball with
    radius, mass, position, and velocity parameters.

    by Perry R. Cook, 1995 - 2004.
*/
/***************************************************/

#include "Sphere.h"
#include <math.h>

Sphere::Sphere(StkFloat radius)
{
  radius_ = radius;
  mass_ = 1.0;
};

Sphere::~Sphere()
{
}

void Sphere::setPosition(StkFloat x, StkFloat y, StkFloat z)
{
  position_.setXYZ(x, y, z);
};

void Sphere::setVelocity(StkFloat x, StkFloat y, StkFloat z)
{
  velocity_.setXYZ(x, y, z);
};

void Sphere::setRadius(StkFloat radius)
{
  radius_ = radius;
};

void Sphere::setMass(StkFloat mass)
{
  mass_ = mass;
};

Vector3D* Sphere::getPosition()
{
  return &position_;
};

Vector3D* Sphere::getRelativePosition(Vector3D* position)
{
  workingVector_.setXYZ(position->getX() - position_.getX(),
                        position->getY() - position_.getY(),  
                        position->getZ() - position_.getZ());
  return &workingVector_;
};

StkFloat Sphere::getVelocity(Vector3D* velocity)
{
  velocity->setXYZ( velocity_.getX(), velocity_.getY(), velocity_.getZ() );
  return velocity_.getLength();
};

StkFloat Sphere::isInside(Vector3D *position)
{
  // Return directed distance from aPosition to spherical boundary ( <
  // 0 if inside).
  StkFloat distance;
  Vector3D *tempVector;

  tempVector = this->getRelativePosition( position );
  distance = tempVector->getLength();
  return distance - radius_;
};

StkFloat Sphere::getRadius()
{
  return radius_;
};

StkFloat Sphere::getMass()
{
  return mass_;
};

void Sphere::addVelocity(StkFloat x, StkFloat y, StkFloat z)
{
  velocity_.setX(velocity_.getX() + x);
  velocity_.setY(velocity_.getY() + y);
  velocity_.setZ(velocity_.getZ() + z);
}

void Sphere::tick(StkFloat timeIncrement)
{
  position_.setX(position_.getX() + (timeIncrement * velocity_.getX()));
  position_.setY(position_.getY() + (timeIncrement * velocity_.getY()));
  position_.setZ(position_.getZ() + (timeIncrement * velocity_.getZ()));
};

