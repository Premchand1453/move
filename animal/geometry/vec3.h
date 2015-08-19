#ifndef ANIMAL_GEOMETRY_VEC3_H
#define ANIMAL_GEOMETRY_VEC3_H

#include <iostream>
#include <animal/traits/numerics.h>



namespace animal { namespace geometry {

// -----------------------------------------------------
//  
//  Vec3 class.
/** Vector (or point) in 3D space.
    
    Declaration/Definition file: animal/geometry/vec3.h
    (creation date: December 3, 1999).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 1999 */
//  
// -----------------------------------------------------

template <
  class RealT      = double,
  class NumTraitsT = animal::traits::Numerics<RealT> >

class Vec3
{

public:
  
  // Traits typedef
  typedef RealT Real;
  
  
  /** @name Constructors/Destructor */
  //@{
  /// Default constructor: does nothing
  Vec3();
  
  /// Constructor: create a Vec3 with 3 Real values
  Vec3(const Real , const Real , const Real );
  //@}
  
  
  /** @name Get */
  //@{
  /// Get x value
  const Real& x() const;
  
  /// Get y value
  const Real& y() const;
  
  /// Get z value
  const Real& z() const;
  
  /// Operator Vec3[i]
  const Real& operator[](int i) const;
  //@}
  
  
  /** @name Set */
  //@{
  /// Set x value
  Vec3& setx(const Real );
  
  /// Set y value
  Vec3& sety(const Real );
  
  /// Set z value
  Vec3& setz(const Real );
  
  /// Operator Vec3[i]
  Real& operator[](int i);
  //@}
  
  
  /** @name Assignment operators */
  //@{
  /// Operator Vec3 += Vec3
  Vec3& operator+=(const Vec3& );
  
  /// Operator Vec3 -= Vec3
  Vec3& operator-=(const Vec3& );
  
  /// Operator Vec3 *= Real
  Vec3& operator*=(const Real );
  
  /// Operator Vec3 /= Real
  Vec3& operator/=(const Real );
  //@}
  
  
  /** @name Arithmetic operators */
  //@{
  /// Operator Vec3+Vec3
  Vec3 operator+(const Vec3& ) const;
  
  /// Operator Vec3-Vec3
  Vec3 operator-(const Vec3& ) const;
  
  /// Operator Vec3*Real
  Vec3 operator*(const Real ) const;
  
  /// Operator Real*Vec3
  template <class R, class NT>
  friend Vec3<R,NT>
  operator*(const R , const Vec3<R,NT>& );
  
  /// Operator Vec3/Real
  Vec3 operator/(const Real ) const;
  
  /// Operator -Vec3
  Vec3 operator-();
  //@}
  
  
  /** @name Equality and relational operators */
  //@{
  /// Operator Vec3 == Vec3
  bool operator==(const Vec3& ) const;
  
  /// Operator Vec3 != Vec3
  bool operator!=(const Vec3& ) const;
  //@}
  
  
  /** @name I/O stream operators */
  //@{
  /// Operator <<
  template <class R, class NT>
  friend std::ostream&
  operator<<(std::ostream& , const Vec3<R,NT>& );
  
  /// Operator >>
  template <class R, class NT>
  friend std::istream&
  operator>>(std::istream& , Vec3<R,NT>& );
  //@}
  
  
  /** @name Norms */
  //@{
  /// Norm (aka l-2 norm, euclidean norm)
  Real norm() const;
  
  /// Square norm
  Real sqnorm() const;
  
  /// Infinite norm (aka l-infinity norm)
  Real inftNorm() const;
  
  /// Normalization
  Vec3& normalize();
  //@}
  
  
  /** @name Constants */
  //@{
  /// Null Vec3
  static const Vec3 null();
  //@}
  
  
  
private:
  
  Real a[3];
  
}; // class Vec3



// -----------------------------------------------------
//  
/** @name Vec3 class-related methods
    
    Declaration/Definition file: animal/geometry/vec3.h
    (creation date: December 3, 1999).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 1999
    
    @see Vec3 */
//  
// -----------------------------------------------------
//@{

/** @name Dot and cross products */
//@{
/// Dot product
template <class RealT, class NumTraitsT>
RealT dot(const Vec3<RealT,NumTraitsT>& , const Vec3<RealT,NumTraitsT>& );

/// Cross product
template <class RealT, class NumTraitsT>
Vec3<RealT,NumTraitsT> cross(const Vec3<RealT,NumTraitsT>& , const Vec3<RealT,NumTraitsT>& );
//@}

/// Distance between p1 and p2
template <class RealT, class NumTraitsT>
RealT dist(const Vec3<RealT,NumTraitsT>& p1, const Vec3<RealT,NumTraitsT>& p2);

/// Square distance between p1 and p2
template <class RealT, class NumTraitsT>
RealT sqdist(const Vec3<RealT,NumTraitsT>& p1, const Vec3<RealT,NumTraitsT>& p2);

/// Cosinus of the (v1,v2) angle
template <class RealT, class NumTraitsT>
RealT cosAng(const Vec3<RealT,NumTraitsT>& v1, const Vec3<RealT,NumTraitsT>& v2);

//@} // Vec3 class-related methods










// -------------------------------------------------------------
// 
//   D E F I N I T I O N   O F   I N L I N E D   M E T H O D S 
// 
// -------------------------------------------------------------

template <class RealT, class NumTraitsT>
inline
Vec3<RealT,NumTraitsT>::
Vec3()
{}

template <class RealT, class NumTraitsT>
inline
Vec3<RealT,NumTraitsT>::
Vec3(const Real x0, const Real y0, const Real z0)
{
  a[0] = x0; a[1] = y0; a[2] = z0;
}





template <class RealT, class NumTraitsT>
inline const RealT&
Vec3<RealT,NumTraitsT>::
x() const 
{
  return a[0];
}

template <class RealT, class NumTraitsT>
inline const RealT&
Vec3<RealT,NumTraitsT>::
y() const 
{
  return a[1];
}

template <class RealT, class NumTraitsT>
inline const RealT&
Vec3<RealT,NumTraitsT>::
z() const 
{
  return a[2];
}

template <class RealT, class NumTraitsT>
inline const RealT&
Vec3<RealT,NumTraitsT>::
operator[](int i) const
{
  return a[i];
}





template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>&
Vec3<RealT,NumTraitsT>::
setx(const Real x0)
{
  a[0] = x0;
  return *this;
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>&
Vec3<RealT,NumTraitsT>::
sety(const Real y0)
{
  a[1] = y0;
  return *this;
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>&
Vec3<RealT,NumTraitsT>::
setz(const Real z0)
{
  a[2] = z0;
  return *this;
}

template <class RealT, class NumTraitsT>
inline RealT&
Vec3<RealT,NumTraitsT>::
operator[](int i)
{
  return a[i];
}





template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>&
Vec3<RealT,NumTraitsT>::
operator+=(const Vec3& v)
{
  a[0] += v.x();
  a[1] += v.y();
  a[2] += v.z();
  return *this;
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>&
Vec3<RealT,NumTraitsT>::
operator-=(const Vec3& v)
{
  a[0] -= v.x();
  a[1] -= v.y();
  a[2] -= v.z();
  return *this;
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>&
Vec3<RealT,NumTraitsT>::
operator*=(const Real k)
{
  a[0] *= k;
  a[1] *= k;
  a[2] *= k;
  return *this;
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>&
Vec3<RealT,NumTraitsT>::
operator/=(const Real k)
{
  (*this) *= 1.0/k;
  return *this;
}





template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>
Vec3<RealT,NumTraitsT>::
operator+(const Vec3& v) const
{
  return Vec3( a[0]+v.x(), a[1]+v.y(), a[2]+v.z() );
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>
Vec3<RealT,NumTraitsT>::
operator-(const Vec3& v) const
{
  return Vec3( a[0]-v.x(), a[1]-v.y(), a[2]-v.z() );
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>
Vec3<RealT,NumTraitsT>::
operator*(const Real k) const
{
  return Vec3(a[0]*k, a[1]*k, a[2]*k);
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>
operator*(const RealT k, const Vec3<RealT,NumTraitsT>& v)
{
  return v*k;
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>
Vec3<RealT,NumTraitsT>::
operator/(const Real k) const
{
  return (*this)*(1.0/k);
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>
Vec3<RealT,NumTraitsT>::
operator-()
{
  return Vec3( -x(), -y(), -z() );
}





template <class RealT, class NumTraitsT>
inline bool
Vec3<RealT,NumTraitsT>::
operator==(const Vec3& v) const
{
  return ( (v.x() == a[0]) && (v.y() == a[1]) && (v.z() == a[2]) );
}

template <class RealT, class NumTraitsT>
inline bool
Vec3<RealT,NumTraitsT>::
operator!=(const Vec3& v) const
{
  return !operator==(v);
}





template <class RealT, class NumTraitsT>
inline std::ostream&
operator<<(std::ostream& s, const Vec3<RealT,NumTraitsT>& v)
{
  return s << v.x() << " " << v.y() << " " << v.z();
}

template <class RealT, class NumTraitsT>
inline std::istream&
operator>>(std::istream& s, Vec3<RealT,NumTraitsT>& v)
{
  RealT x0 = 0.0;
  RealT y0 = 0.0;
  RealT z0 = 0.0;
  
  s >> x0 >> y0 >> z0;
  v = Vec3<RealT,NumTraitsT>(x0, y0, z0);
  return s;
}





template <class RealT, class NumTraitsT>
inline RealT
Vec3<RealT,NumTraitsT>::
norm() const 
{
  return NumTraitsT::sqroot( a[0]*a[0] + a[1]*a[1] + a[2]*a[2] );
}

template <class RealT, class NumTraitsT>
inline RealT
Vec3<RealT,NumTraitsT>::
sqnorm() const
{
  return ( a[0]*a[0] + a[1]*a[1] + a[2]*a[2] );
}

template <class RealT, class NumTraitsT>
inline RealT
Vec3<RealT,NumTraitsT>::
inftNorm() const
{
  Real x0 = a[0];
  Real y0 = a[1];
  Real z0 = a[2];
  
  if ( x0 < 0.0 ) x0 = -x0;
  if ( y0 < 0.0 ) y0 = -y0;
  if ( z0 < 0.0 ) z0 = -z0;
  
  Real max = x0;
  
  if ( max < y0 ) max = y0;
  if ( max < z0 ) max = z0;
  return max;
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>&
Vec3<RealT,NumTraitsT>::
normalize()
{
  (*this) *= 1.0/norm();
  return *this;
}





template <class RealT, class NumTraitsT>
inline const Vec3<RealT,NumTraitsT>
Vec3<RealT,NumTraitsT>::
null()
{
  return Vec3( Real(0.0), Real(0.0), Real(0.0) );
}





template <class RealT, class NumTraitsT>
inline RealT
dot(const Vec3<RealT,NumTraitsT>& v1, const Vec3<RealT,NumTraitsT>& v2)
{
  return ( v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z() );
}

template <class RealT, class NumTraitsT>
inline Vec3<RealT,NumTraitsT>
cross(const Vec3<RealT,NumTraitsT>& v1, const Vec3<RealT,NumTraitsT>& v2)
{
  return Vec3<RealT,NumTraitsT>( v1.y()*v2.z() - v1.z()*v2.y(),
				 v1.z()*v2.x() - v1.x()*v2.z(),
				 v1.x()*v2.y() - v1.y()*v2.x() );
}





template <class RealT, class NumTraitsT>
inline RealT
dist(const Vec3<RealT,NumTraitsT>& p1, const Vec3<RealT,NumTraitsT>& p2)
{
  return (p1 - p2).norm();
}

template <class RealT, class NumTraitsT>
inline RealT
sqdist(const Vec3<RealT,NumTraitsT>& p1, const Vec3<RealT,NumTraitsT>& p2)
{
  return (p1 - p2).sqnorm();
}

template <class RealT, class NumTraitsT>
inline RealT
cosAng(const Vec3<RealT,NumTraitsT>& v1, const Vec3<RealT,NumTraitsT>& v2)
{
  return dot(v1,v2)/NumTraitsT::sqroot( v1.sqnorm()*v2.sqnorm() );
}

} } // namespace animal { namespace geometry {



#endif // ANIMAL_GEOMETRY_VEC3_H
