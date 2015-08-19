#ifndef ANIMAL_GEOMETRY_QUATERNION_H
#define ANIMAL_GEOMETRY_QUATERNION_H

#include <animal/geometry/vec3.h>



namespace animal { namespace geometry {

template <class RealT> class Quaternion_Traits;

// -----------------------------------------------------------
//  
//  Quaternion class.
/** Unit quaternion used to model a rotation in 3D.
    
    Declaration/Definition file: animal/geometry/quaternion.h
    (creation date: January 16, 2000).
    
    @author François Faure
    
    iMAGIS/GRAVIR, 2000 */
//  
// -----------------------------------------------------------

template <
  class RealT      = double,
  class NumTraitsT = animal::traits::Numerics<RealT>,
  class TraitsT    = Quaternion_Traits<RealT> >

class Quaternion
{

public:
  
  // Traits typedefs
  typedef RealT                 Real;
  typedef TraitsT               Traits;
  typedef NumTraitsT            NumTraits;
  typedef typename Traits::Vec3 Vec3;
  
  
  /** @name Constructors/Destructor */
  //@{
  /// Default constructor: does nothing
  Quaternion();
  
  /// Components w, x, y, z
  Quaternion(const Real w, const Real x, const Real y, const Real z);
  
  /// Axis and angle of rotation
  Quaternion(const Vec3& axis, const Real angle);
  //@}
  
  
  /** @name Get */
  //@{
  /// Get axis, angle -biased!- for display only
  void getAxisAngle(Real& x, Real& y, Real& z, Real& angle) const;
  
  /// Get axis, angle -biased!- for display only
  void getAxisAngle(Vec3& axis, Real& angle) const ;
  
  /// Get w component in w, x, y, z
  const Real& w() const;
  
  /// Get x component in w, x, y, z
  const Real& x() const;
  
  /// Get y component in w, x, y, z
  const Real& y() const;
  
  /// Get z component in w, x, y, z
  const Real& z() const;
  //@}
  
  
  /** @name Set */
  //@{
  /** Set axis, angle.
      Axis represented by 3 Real.
      Axis needs not to be normalized. */
  void setAxisAngle(const Real x, const Real y, const Real z, const Real angle);
  
  /** Set axis, angle.
      Axis is represented by a vector.
      Axis needs not to be normalized. */
  void setAxisAngle(const Vec3& axis, const Real angle);
  
  /// Set w value
  Quaternion& setw(const Real );
  
  /// Set x value
  Quaternion& setx(const Real );
  
  /// Set y value
  Quaternion& sety(const Real );
  
  /// Set z value
  Quaternion& setz(const Real );
  
  /// Set wxyz value
  Quaternion& setwxyz(const Real w, const Real x, const Real y, const Real z);
  
  
  /** @name Set quaternion using Euler angles (not tested!) */
  //@{
  ///
  Quaternion& setEulerXYZ(const Real x, const Real y, const Real z);
  
  ///
  Quaternion& setEulerXYX(const Real x, const Real y, const Real xx);
  
  ///
  Quaternion& setEulerXZY(const Real x, const Real z, const Real y);
  
  ///
  Quaternion& setEulerXZX(const Real x, const Real z, const Real xx);
  
  ///
  Quaternion& setEulerYZX(const Real y, const Real z, const Real x);
  
  ///
  Quaternion& setEulerYZY(const Real y, const Real z, const Real yy);
  
  ///
  Quaternion& setEulerYXY(const Real y, const Real x, const Real yy);
  
  ///
  Quaternion& setEulerYXZ(const Real y, const Real x, const Real z);
  
  ///
  Quaternion& setEulerZXY(const Real z, const Real x, const Real y);
  
  ///
  Quaternion& setEulerZXZ(const Real z, const Real x, const Real zz);
  
  ///
  Quaternion& setEulerZYX(const Real z, const Real y, const Real x);
  
  ///
  Quaternion& setEulerZYZ(const Real z, const Real y, const Real zz);
  //@}
  
  //@}
  
  
  /** @name Operations on Quaternions (rotations composition) */
  //@{
  /// In-place product
  Quaternion& operator*=(const Quaternion& );
  
  /// In-place product with the inverse of a quaternion
  Quaternion& operator/=(const Quaternion& );
  
  /// Product with a quaternion
  Quaternion operator*(const Quaternion& ) const;
  
  /// Product with the inverse of a quaternion
  Quaternion operator/(const Quaternion& ) const ;
  
  /// Inverse rotation
  Quaternion inv() const ;
  //@}
  
  
  /** @name Operations on vectors */
  //@{
  /// Apply rotation to a vector
  Vec3 operator*(const Vec3& ) const;
  
  /// Apply inverse rotation to a vector
  Vec3 operator/(const Vec3& ) const;
  //@}
  
  
  /** @name Input/Output */
  //@{
  /// Text output: axis angle
  template <class R, class NT, class T>
  friend std::ostream&
  operator<<(std::ostream& , const Quaternion<R,NT,T>& );
  
  /// Text input: axis angle
  template <class R, class NT, class T>
  friend std::istream&
  operator>>(std::istream& , Quaternion<R,NT,T>& );
  //@}
  
  
  /** @name Norm */
  //@{
  /// Norm
  Real norm() const;
  
  /// Normalize
  Quaternion& normalize();
  //@}
  
  
  /** @name Constants */
  //@{
  /// Identity (rotation with null angle)
  static const Quaternion id();
  //@}
  
  
  
private:
  
  Real v[4]; // The Quaternion is modeled using (w, x, y, z)
               // i.e. cos(t/2), ax.sin(t/2), ay.sin(t/2), az.sin(t/2)
               // where {ax,ay,az} is a Quaternion axis with norm=1
               // and t is the Quaternion angle
  
}; // class Quaternion



// -----------------------------------------------------------
//  
/** @name Quaternion class-related methods
    
    Declaration/Definition file: animal/geometry/quaternion.h
    (creation date: January 16, 2000).
    
    @author François Faure
    
    iMAGIS/GRAVIR, 2000
    
    @see Quaternion */
//  
// -----------------------------------------------------------
//@{

/** @name Operations on vectors */
//@{
/// Apply rotation to a vector
template <class R, class NT, class T>
typename Quaternion<R,NT,T>::Vec3
operator*(const typename Quaternion<R,NT,T>::Vec3& ,
          const Quaternion<R,NT,T>& );

/// Apply inverse rotation to a vector
template <class R, class NT, class T>
typename Quaternion<R,NT,T>::Vec3
operator/(const typename Quaternion<R,NT,T>::Vec3& ,
          const Quaternion<R,NT,T>& );
//@}


/** @name Quaternion to rotation matrix */
//@{
/// Write rotation matrix m corresponding to quaternion q
template <class RealT, class NumTraitsT, class TraitsT>
void writeRotMatrix(const Quaternion<RealT,NumTraitsT,TraitsT>& q, RealT m[3][3]);

/** Write rotation matrix m corresponding to quaternion q, in OpenGL format.
    For normalized quaternion only! */
template <class RealT, class NumTraitsT, class TraitsT>
void writeOpenGLRotMatrix(const Quaternion<RealT,NumTraitsT,TraitsT>& q, RealT m[4][4]);
//@}

//@} // Quaternion class-related methods



// -----------------------------------------------------------
//  
//  Quaternion_Traits class.
/** Provides access to vector data and methods.
    Defines also some numerical constants.
    Create or derive your own version to customize
    your Quaternion class.
    
    Declaration/Definition file: animal/geometry/quaternion.h
    (creation date: January 16, 2000).
    
    @author François Faure */
//  
// -----------------------------------------------------------

template <class RealT> class Quaternion_Traits
{

public:
  
  // Traits typedefs
  typedef RealT Real;
  typedef animal::geometry::Vec3<Real> Vec3;
  
  
  
  /** @name Access to vector */
  //@{
  /// Get x coordinate
  static const Real& x(const Vec3& v) { return v.x(); }
  
  /// Get y coordinate
  static const Real& y(const Vec3& v) { return v.y(); }
  
  /// Get z coordinate
  static const Real& z(const Vec3& v) { return v.z(); }
  //@}
  
  
  /// Normalize vector
  static Vec3& normalize(Vec3& v) { return v.normalize(); }
  
}; // class Quaternion_Traits










// -------------------------------------------------------------
// 
//   D E F I N I T I O N   O F   I N L I N E D   M E T H O D S 
// 
// -------------------------------------------------------------





template<class RealT, class NumTraitsT, class TraitsT>
inline
Quaternion<RealT,NumTraitsT,TraitsT>::
Quaternion()
{}

template<class RealT, class NumTraitsT, class TraitsT>
inline
Quaternion<RealT,NumTraitsT,TraitsT>::
Quaternion(const Real w, const Real x, const Real y, const Real z)
{
  v[0]=w; v[1]=x; v[2]=y; v[3]=z;
}

template<class RealT, class NumTraitsT, class TraitsT>
inline
Quaternion<RealT,NumTraitsT,TraitsT>::
Quaternion(const Vec3& axis, const Real angle)
{
  setAxisAngle(axis, angle); 
}





template<class RealT, class NumTraitsT, class TraitsT>
inline void 
Quaternion<RealT,NumTraitsT,TraitsT>::
getAxisAngle(Real& x, Real& y, Real& z, Real& angle) const 
{
  angle = 2.0*NumTraitsT::acosine( v[0]>1.0 ? 1.0 : v[0]<-1.0 ? -1.0 : v[0] );
  
  if ( angle > NumTraitsT::numthreshold() )
    {
      Real tmp = 1.0/NumTraitsT::sine(angle/2);
      x=v[1]*tmp;
      y=v[2]*tmp;
      z=v[3]*tmp;
    }
  else
    {
      x = 1.0; // Arbitrary!
      y = 0.0;
      z = 0.0;
    }
}

template<class RealT, class NumTraitsT, class TraitsT>
inline void 
Quaternion<RealT,NumTraitsT,TraitsT>::
getAxisAngle(Vec3& axis, Real& angle) const 
{
  angle = 2.0*NumTraitsT::acosine( v[0]>1.0 ? 1.0 : v[0]<-1.0 ? -1.0 : v[0] );
  
  if ( angle > NumTraitsT::numthreshold() )
    {
      Real tmp = 1.0/NumTraitsT::sine(angle/2);
      axis = Vec3(v[1]*tmp, v[2]*tmp, v[3]*tmp);
    }
  else
    {
      axis = Vec3(1.0, 0.0, 0.0); // Arbitrary!
    }
}

template<class RealT, class NumTraitsT, class TraitsT>
inline const RealT&
Quaternion<RealT,NumTraitsT,TraitsT>::w() const
{
  return v[0];
}

template<class RealT, class NumTraitsT, class TraitsT>
inline const RealT&
Quaternion<RealT,NumTraitsT,TraitsT>::x() const
{
  return v[1];
}

template<class RealT, class NumTraitsT, class TraitsT>
inline const RealT&
Quaternion<RealT,NumTraitsT,TraitsT>::y() const
{
  return v[2];
}

template<class RealT, class NumTraitsT, class TraitsT>
inline const RealT&
Quaternion<RealT,NumTraitsT,TraitsT>::z() const
{
  return v[3];
}





template<class RealT, class NumTraitsT, class TraitsT>
inline void 
Quaternion<RealT,NumTraitsT,TraitsT>::
setAxisAngle(const Real x, const Real y, const Real z, const Real angle)
{
  v[0] = NumTraitsT::cosine(angle/2);
  
  Vec3 axisn(x, y, z);
  TraitsT::normalize(axisn);
  Real tmp = NumTraitsT::sine(angle/2);
  
  v[1] = TraitsT::x(axisn)*tmp;
  v[2] = TraitsT::y(axisn)*tmp;
  v[3] = TraitsT::z(axisn)*tmp;
}

template<class RealT, class NumTraitsT, class TraitsT>
inline void 
Quaternion<RealT,NumTraitsT,TraitsT>::
setAxisAngle(const Vec3& axis, const Real angle)
{
  v[0] = NumTraitsT::cosine(angle/2);
  
  Vec3 axisn = axis;
  TraitsT::normalize(axisn);
  Real tmp = NumTraitsT::sine(angle/2);
  
  v[1] = TraitsT::x(axisn)*tmp;
  v[2] = TraitsT::y(axisn)*tmp;
  v[3] = TraitsT::z(axisn)*tmp;
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setw(const Real w)
{
  v[0]=w;
  return *this;
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setx(const Real x)
{
  v[1]=x;
  return *this;
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
sety(const Real y)
{
  v[2]=y;
  return *this;
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setz(const Real z)
{
  v[3]=z;
  return *this;
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setwxyz(const Real w, const Real x, const Real y, const Real z)
{
  v[0]=w; v[1]=x; v[2]=y; v[3]=z;
  return *this;
}





template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerXYZ(const Real x, const Real y, const Real z)
{
  Quaternion qx; qx.setAxisAngle(1,0,0, x);
  Quaternion qy; qy.setAxisAngle(0,1,0, y);
  Quaternion qz; qz.setAxisAngle(0,0,1, z);
  
  return (*this = qx*qy*qz);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerXYX(const Real x, const Real y, const Real xx)
{
  Quaternion qx;  qx.setAxisAngle (1,0,0, x );
  Quaternion qy;  qy.setAxisAngle (0,1,0, y );
  Quaternion qxx; qxx.setAxisAngle(1,0,0, xx);
  
  return (*this = qx*qy*qxx);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerXZY(const Real x, const Real z, const Real y)
{
  Quaternion qx; qx.setAxisAngle(1,0,0, x);
  Quaternion qz; qz.setAxisAngle(0,0,1, z);
  Quaternion qy; qy.setAxisAngle(0,1,0, y);
  
  return (*this = qx*qz*qy);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerXZX(const Real x, const Real z, const Real xx)
{
  Quaternion qx;  qx.setAxisAngle (1,0,0, x );
  Quaternion qz;  qz.setAxisAngle (0,0,1, z );
  Quaternion qxx; qxx.setAxisAngle(1,0,0, xx);
  
  return (*this = qx*qz*qxx);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerYZX(const Real y, const Real z, const Real x)
{
  Quaternion qy; qy.setAxisAngle(0,1,0, y);
  Quaternion qz; qz.setAxisAngle(0,0,1, z);
  Quaternion qx; qx.setAxisAngle(1,0,0, x);
  
  return (*this = qy*qz*qx);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerYZY(const Real y, const Real z, const Real yy)
{
  Quaternion qy;  qy.setAxisAngle (0,1,0, y );
  Quaternion qz;  qz.setAxisAngle (0,0,1, z );
  Quaternion qyy; qyy.setAxisAngle(0,1,0, yy);
  
  return (*this = qy*qz*qyy);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerYXY(const Real y, const Real x, const Real yy)
{
  Quaternion qy;  qy.setAxisAngle (0,1,0, y );
  Quaternion qx;  qx.setAxisAngle (1,0,0, x );
  Quaternion qyy; qyy.setAxisAngle(0,1,0, yy);
  
  return (*this = qy*qx*qyy);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerYXZ(const Real y, const Real x, const Real z)
{
  Quaternion qy;  qy.setAxisAngle(0,1,0, y);
  Quaternion qx;  qx.setAxisAngle(1,0,0, x);
  Quaternion qz;  qz.setAxisAngle(0,0,1, z);
  
  return (*this = qy*qx*qz);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerZXY(const Real z, const Real x, const Real y)
{
  Quaternion qz; qz.setAxisAngle(0,0,1, z);
  Quaternion qx; qx.setAxisAngle(1,0,0, x);
  Quaternion qy; qy.setAxisAngle(0,1,0, y);
  
  return (*this = qz*qx*qy);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerZXZ(const Real z, const Real x, const Real zz)
{
  Quaternion qz;  qz.setAxisAngle (0,0,1, z );
  Quaternion qx;  qx.setAxisAngle (1,0,0, x );
  Quaternion qzz; qzz.setAxisAngle(0,0,1, zz);
  
  return (*this = qz*qx*qzz);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerZYX(const Real z, const Real y, const Real x)
{
  Quaternion qz; qz.setAxisAngle(0,0,1, z);
  Quaternion qy; qy.setAxisAngle(0,1,0, y);
  Quaternion qx; qx.setAxisAngle(1,0,0, x);
  
  return (*this = qz*qy*qx);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::
setEulerZYZ(const Real z, const Real y, const Real zz)
{
  Quaternion qz;  qz.setAxisAngle (0,0,1, z );
  Quaternion qy;  qy.setAxisAngle (0,1,0, y );
  Quaternion qzz; qzz.setAxisAngle(0,0,1, zz);
  
  return (*this = qz*qy*qzz);
}





template<class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>& 
Quaternion<RealT,NumTraitsT,TraitsT>::
operator*=(const Quaternion& q)
{
  Real w = q.v[0]*v[0] - q.v[1]*v[1] - q.v[2]*v[2] - q.v[3]*v[3];
  Real x = q.v[0]*v[1] + q.v[1]*v[0] + q.v[2]*v[3] - q.v[3]*v[2];
  Real y = q.v[0]*v[2] - q.v[1]*v[3] + q.v[2]*v[0] + q.v[3]*v[1];
  Real z = q.v[0]*v[3] + q.v[1]*v[2] - q.v[2]*v[1] + q.v[3]*v[0];
  v[0] = w; v[1] = x; v[2] = y; v[3] = z;
  
  return *this;
}

template<class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>& 
Quaternion<RealT,NumTraitsT,TraitsT>::
operator/=(const Quaternion& q)
{
  Real w = q.v[0]*v[0] + q.v[1]*v[1] + q.v[2]*v[2] + q.v[3]*v[3];
  Real x = q.v[0]*v[1] - q.v[1]*v[0] - q.v[2]*v[3] + q.v[3]*v[2];
  Real y = q.v[0]*v[2] + q.v[1]*v[3] - q.v[2]*v[0] - q.v[3]*v[1];
  Real z = q.v[0]*v[3] - q.v[1]*v[2] + q.v[2]*v[1] - q.v[3]*v[0];
  v[0] = w; v[1] = x; v[2] = y; v[3] = z;
  
  return *this;
}

template<class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>
Quaternion<RealT,NumTraitsT,TraitsT>::
operator*(const Quaternion& q) const
{
  return Quaternion
   (
     q.v[0]*v[0] - q.v[1]*v[1] - q.v[2]*v[2] - q.v[3]*v[3],
     q.v[0]*v[1] + q.v[1]*v[0] + q.v[2]*v[3] - q.v[3]*v[2],
     q.v[0]*v[2] - q.v[1]*v[3] + q.v[2]*v[0] + q.v[3]*v[1],
     q.v[0]*v[3] + q.v[1]*v[2] - q.v[2]*v[1] + q.v[3]*v[0]
   );	
}

template<class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>
Quaternion<RealT,NumTraitsT,TraitsT>::
operator/(const Quaternion& q) const
{
  return Quaternion
   (
     q.v[0]*v[0] + q.v[1]*v[1] + q.v[2]*v[2] + q.v[3]*v[3],
     q.v[0]*v[1] - q.v[1]*v[0] - q.v[2]*v[3] + q.v[3]*v[2],
     q.v[0]*v[2] + q.v[1]*v[3] - q.v[2]*v[0] - q.v[3]*v[1],
     q.v[0]*v[3] - q.v[1]*v[2] + q.v[2]*v[1] - q.v[3]*v[0]
   );	
}

template<class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>
Quaternion<RealT,NumTraitsT,TraitsT>::inv() const
{
  return Quaternion(v[0], -v[1], -v[2], -v[3]);
}





template <class RealT, class NumTraitsT, class TraitsT>
inline typename Quaternion<RealT,NumTraitsT,TraitsT>::Vec3
Quaternion<RealT,NumTraitsT,TraitsT>::
operator*(const typename Quaternion<RealT,NumTraitsT,TraitsT>::Vec3& p) const 
{
  Real r[4];
  r[0] =  TraitsT::x(p)*v[1] + TraitsT::y(p)*v[2] + TraitsT::z(p)*v[3];
  r[1] =  TraitsT::x(p)*v[0] - TraitsT::y(p)*v[3] + TraitsT::z(p)*v[2];
  r[2] =  TraitsT::x(p)*v[3] + TraitsT::y(p)*v[0] - TraitsT::z(p)*v[1];
  r[3] = -TraitsT::x(p)*v[2] + TraitsT::y(p)*v[1] + TraitsT::z(p)*v[0];
  
  return Vec3
   (
     v[0]*r[1] + v[1]*r[0] + v[2]*r[3] - v[3]*r[2],
     v[0]*r[2] - v[1]*r[3] + v[2]*r[0] + v[3]*r[1],
     v[0]*r[3] + v[1]*r[2] - v[2]*r[1] + v[3]*r[0]
   );

}

template <class RealT, class NumTraitsT, class TraitsT>
inline typename Quaternion<RealT,NumTraitsT,TraitsT>::Vec3
operator*(const typename Quaternion<RealT,NumTraitsT,TraitsT>::Vec3& p,
          const Quaternion<RealT,NumTraitsT,TraitsT>& q)
{
  return q*p;
}

template <class RealT, class NumTraitsT, class TraitsT>
inline typename Quaternion<RealT,NumTraitsT,TraitsT>::Vec3
Quaternion<RealT,NumTraitsT,TraitsT>::
operator/(const typename Quaternion<RealT,NumTraitsT,TraitsT>::Vec3& p) const
{
  Real r[4];
  r[0] = -TraitsT::x(p)*v[1] - TraitsT::y(p)*v[2] - TraitsT::z(p)*v[3];
  r[1] =  TraitsT::x(p)*v[0] + TraitsT::y(p)*v[3] - TraitsT::z(p)*v[2];
  r[2] = -TraitsT::x(p)*v[3] + TraitsT::y(p)*v[0] + TraitsT::z(p)*v[1];
  r[3] =  TraitsT::x(p)*v[2] - TraitsT::y(p)*v[1] + TraitsT::z(p)*v[0];
  
  return Vec3
   (
     v[0]*r[1] - v[1]*r[0] - v[2]*r[3] + v[3]*r[2],
     v[0]*r[2] + v[1]*r[3] - v[2]*r[0] - v[3]*r[1],
     v[0]*r[3] - v[1]*r[2] + v[2]*r[1] - v[3]*r[0]
   );
}

template <class RealT, class NumTraitsT, class TraitsT>
inline typename Quaternion<RealT,NumTraitsT,TraitsT>::Vec3
operator/(const typename Quaternion<RealT,NumTraitsT,TraitsT>::Vec3& p,
          const Quaternion<RealT,NumTraitsT,TraitsT>& q)
{
  return q/p;
}





template <class RealT, class NumTraitsT, class TraitsT>
inline std::ostream& 
operator<<(std::ostream& stream, const Quaternion<RealT,NumTraitsT,TraitsT>& q)
{
  RealT x, y, z, angle;
  q.getAxisAngle(x,y,z,angle);
  stream << x << " " << y << " " << z << " " << angle;
  
  return stream;
}

template <class RealT, class NumTraitsT, class TraitsT>
inline std::istream& 
operator>>(std::istream& stream, Quaternion<RealT,NumTraitsT,TraitsT>& q)
{
  RealT x, y, z, angle;
  stream >> x >> y >> z >> angle;
  q.setAxisAngle(x,y,z,angle);
  
  return stream;
}





template<class RealT, class NumTraitsT, class TraitsT>
inline RealT
Quaternion<RealT,NumTraitsT,TraitsT>::norm() const
{
  return NumTraitsT::sqroot( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3] );
}

template<class RealT, class NumTraitsT, class TraitsT>
inline Quaternion<RealT,NumTraitsT,TraitsT>&
Quaternion<RealT,NumTraitsT,TraitsT>::normalize()
{
  Real tmp = 1.0/norm();
  v[0]*=tmp; v[1]*=tmp; v[2]*=tmp; v[3]*=tmp;
  return *this;
}





template<class RealT, class NumTraitsT, class TraitsT>
inline const Quaternion<RealT,NumTraitsT,TraitsT>
Quaternion<RealT,NumTraitsT,TraitsT>::id()
{
  return Quaternion(Real(1.0), Real(0.0), Real(0.0), Real(0.0));
}





template <class RealT, class NumTraitsT, class TraitsT>
inline void
writeRotMatrix(const Quaternion<RealT,NumTraitsT,TraitsT>& q, RealT m[3][3])
{
  RealT qw = q.w();
  RealT qx = q.x();
  RealT qy = q.y();
  RealT qz = q.z();
  
  RealT qww = qw*qw;
  RealT qwx = qw*qx;
  RealT qwy = qw*qy;
  RealT qwz = qw*qz;
  
  RealT qxx = qx*qx;
  RealT qxy = qx*qy;
  RealT qxz = qx*qz;
  
  RealT qyy = qy*qy;
  RealT qyz = qy*qz;
  
  RealT qzz = qz*qz;
  
  /* Standard C convention:
     the element m[i][j] is in row i and column j.
  */
  m[0][0] = qww + qxx - (qyy + qzz);
  m[1][0] =         2.0*(qxy - qwz);
  m[2][0] =         2.0*(qxz + qwy);
  
  m[0][1] =         2.0*(qxy + qwz);
  m[1][1] = qww + qyy - (qzz + qxx);
  m[2][1] =         2.0*(qyz - qwx);
  
  m[0][2] =         2.0*(qxz - qwy);
  m[1][2] =         2.0*(qyz + qwx);
  m[2][2] = qww + qzz - (qyy + qxx);
}

template <class RealT, class NumTraitsT, class TraitsT>
inline void
writeOpenGLRotMatrix(const Quaternion<RealT,NumTraitsT,TraitsT>& q, RealT m[4][4])
{
  RealT qw = q.w();
  RealT qx = q.x();
  RealT qy = q.y();
  RealT qz = q.z();
  
  RealT qwx = qw*qx;
  RealT qwy = qw*qy;
  RealT qwz = qw*qz;
  
  RealT qxx = qx*qx;
  RealT qxy = qx*qy;
  RealT qxz = qx*qz;
  
  RealT qyy = qy*qy;
  RealT qyz = qy*qz;
  
  RealT qzz = qz*qz;
  
  /* OpenGL convention:
     the element m[i][j] is in the ith column and jth row
     of the OpenGL transformation matrix.
     This is the reverse of the standard C convention.
  */
  m[0][0] = 1.0 - 2.0*(qyy + qzz);
  m[0][1] =       2.0*(qxy - qwz);
  m[0][2] =       2.0*(qxz + qwy);
  m[0][3] = 0.0;
  
  m[1][0] =       2.0*(qxy + qwz);
  m[1][1] = 1.0 - 2.0*(qzz + qxx);
  m[1][2] =       2.0*(qyz - qwx);
  m[1][3] = 0.0;
  
  m[2][0] =       2.0*(qxz - qwy);
  m[2][1] =       2.0*(qyz + qwx);
  m[2][2] = 1.0 - 2.0*(qyy + qxx);
  m[2][3] = 0.0;
  
  m[3][0] = 0.0;
  m[3][1] = 0.0;
  m[3][2] = 0.0;
  m[3][3] = 1.0;
}

} } // namespace animal { namespace geometry {



#endif // ANIMAL_GEOMETRY_QUATERNION_H
