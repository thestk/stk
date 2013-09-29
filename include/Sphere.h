/***************************************************/
/*! \class Sphere
    \brief STK sphere class.

    This class implements a spherical ball with
    radius, mass, position, and velocity parameters.

    by Perry R. Cook, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SPHERE_H)
#define __SPHERE_H

#include "Vector3D.h"

class Sphere
{
public:
  //! Constructor taking an initial radius value.
  Sphere(double initRadius);

  //! Class destructor.
  ~Sphere();

  //! Set the 3D center position of the sphere.
  void setPosition(double anX, double aY, double aZ);

  //! Set the 3D velocity of the sphere.
  void setVelocity(double anX, double aY, double aZ);

  //! Set the radius of the sphere.
  void setRadius(double aRadius);

  //! Set the mass of the sphere.
  void setMass(double aMass);

  //! Get the current position of the sphere as a 3D vector.
  Vector3D* getPosition();

  //! Get the relative position of the given point to the sphere as a 3D vector.
  Vector3D* getRelativePosition(Vector3D *aPosition);

  //! Set the velcoity of the sphere as a 3D vector.
  double getVelocity(Vector3D* aVelocity);

  //! Returns the distance from the sphere boundary to the given position (< 0 if inside).
  double isInside(Vector3D *aPosition);

  //! Get the current sphere radius.
  double getRadius();

  //! Get the current sphere mass.
  double getMass();

  //! Increase the current sphere velocity by the given 3D components.
  void addVelocity(double anX, double aY, double aZ);

  //! Move the sphere for the given time increment.
  void tick(double timeIncrement);
   
private:
  Vector3D *myPosition;
  Vector3D *myVelocity;
  Vector3D workingVector;
  double myRadius;
  double myMass;
};

#endif
