/***************************************************/
/*! \class Vector3D
    \brief STK 3D vector class.

    This class implements a three-dimensional vector.

    by Perry R. Cook, 1995 - 2002.
*/
/***************************************************/

#if !defined(__VECTOR3D_H)
#define __VECTOR3D_H

class Vector3D {

public:
  //! Default constructor taking optional initial X, Y, and Z values.
  Vector3D(double initX=0.0, double initY=0.0, double initZ=0.0);

  //! Class destructor.
  ~Vector3D();

  //! Get the current X value.
  double getX();

  //! Get the current Y value.
  double getY();

  //! Get the current Z value.
  double getZ();

  //! Calculate the vector length.
  double getLength();

  //! Set the X, Y, and Z values simultaniously.
  void setXYZ(double anX, double aY, double aZ);

  //! Set the X value.
  void setX(double aval);

  //! Set the Y value.
  void setY(double aval);

  //! Set the Z value.
  void setZ(double aval);

protected:
  double myX;
  double myY;
  double myZ;
};

#endif
