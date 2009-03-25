/***************************************************/
/*! \class Sphere
    \brief STK sphere class.

    This class implements a spherical ball with
    radius, mass, position, and velocity parameters.

    by Perry R. Cook, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_SPHERE_H
#define STK_SPHERE_H

#include "Stk.h"
#include "Vector3D.h"

class Sphere : public Stk
{
public:
  //! Constructor taking an initial radius value.
  Sphere(StkFloat radius = 1.0 );

  //! Class destructor.
  ~Sphere();

  //! Set the 3D center position of the sphere.
  void setPosition(StkFloat x, StkFloat y, StkFloat z);

  //! Set the 3D velocity of the sphere.
  void setVelocity(StkFloat x, StkFloat y, StkFloat z);

  //! Set the radius of the sphere.
  void setRadius(StkFloat radius);

  //! Set the mass of the sphere.
  void setMass(StkFloat mass);

  //! Get the current position of the sphere as a 3D vector.
  Vector3D* getPosition();

  //! Get the relative position of the given point to the sphere as a 3D vector.
  Vector3D* getRelativePosition(Vector3D *position);

  //! Set the velcoity of the sphere as a 3D vector.
  StkFloat getVelocity(Vector3D* velocity);

  //! Returns the distance from the sphere boundary to the given position (< 0 if inside).
  StkFloat isInside(Vector3D *position);

  //! Get the current sphere radius.
  StkFloat getRadius();

  //! Get the current sphere mass.
  StkFloat getMass();

  //! Increase the current sphere velocity by the given 3D components.
  void addVelocity(StkFloat x, StkFloat y, StkFloat z);

  //! Move the sphere for the given time increment.
  void tick(StkFloat timeIncrement);
   
private:
  Vector3D position_;
  Vector3D velocity_;
  Vector3D workingVector_;
  StkFloat radius_;
  StkFloat mass_;
};

#endif
