#ifndef ANIMAL_GEOMETRY_VEC2_H
#define ANIMAL_GEOMETRY_VEC2_H

#include <iostream>
#include <animal/traits/numerics.h>



namespace animal { namespace geometry {

// -----------------------------------------------------
//  
//  Vec2 class.
/** Vector (or point) in 2D space.
    
    Declaration/Definition file: animal/geometry/vec2.h
    (creation date: December 3, 1999).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 1999 */
//  
// -----------------------------------------------------

template <
  class RealT      = double,
  class NumTraitsT = animal::traits::Numerics<RealT> >

class Vec2
{

public:
  
  // Traits typedef
  typedef RealT Real;
  
  
  /** @name Constructors/Destructor */
  //@{
  /// Default constructor: does nothing
  Vec2();
  
  /// Constructor: create a Vec2 with 2 Real values
  Vec2(const Real , const Real );
  //@}
  
  
  /** @name Get */
  //@{
  /// Get x value
  const Real& x() const;
  
  /// Get y value
  const Real& y() const;
  
  /// Operator Vec2[i]
  const Real& operator[](int i) const;
  //@}
  
  
  /** @name Set */
  //@{
  /// Set x value
  Vec2& setx(const Real );
  
  /// Set y value
  Vec2& sety(const Real );
  
  /// Operator Vec2[i]
  Real& operator[](int i);
  //@}
  
  
  /** @name Assignment operators */
  //@{
  /// Operator Vec2 += Vec2
  Vec2& operator+=(const Vec2& );
  
  /// Operator Vec2 -= Vec2
  Vec2& operator-=(const Vec2& );
  
  /// Operator Vec2 *= Real
  Vec2& operator*=(const Real );
  
  /// Operator Vec2 /= Real
  Vec2& operator/=(const Real );
  //@}
  
  
  /** @name Arithmetic operators */
  //@{
  /// Operator Vec2+Vec2
  Vec2 operator+(const Vec2& ) const;
  
  /// Operator Vec2-Vec2
  Vec2 operator-(const Vec2& ) const;
  
  /// Operator Vec2*Real
  Vec2 operator*(const Real ) const;
  
  /// Operator Real*Vec2
  template <class R, class NT>
  friend Vec2<R,NT>
  operator*(const R , const Vec2<R,NT>& );
  
  /// Operator Vec2/Real
  Vec2 operator/(const Real ) const;
  
  /// Operator -Vec2
  Vec2 operator-();
  //@}
  
  
  /** @name Equality and relational operators */
  //@{
  /// Operator Vec2 == Vec2
  bool operator==(const Vec2& ) const;
  
  /// Operator Vec2 != Vec2
  bool operator!=(const Vec2& ) const;
  //@}
  
  
  /** @name I/O stream operators */
  //@{
  /// Operator <<
  template <class R, class NT>
  friend std::ostream&
  operator<<(std::ostream& , const Vec2<R,NT>& );
  
  /// Operator >>
  template <class R, class NT>
  friend std::istream&
  operator>>(std::istream& , Vec2<R,NT>& );
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
  Vec2& normalize();
  //@}
  
  
  /** @name Constants */
  //@{
  /// Null Vec2
  static const Vec2 null();
  //@}
  
  
  
private:
  
  Real a[2];
  
}; // class Vec2



// -----------------------------------------------------
//  
/** @name Vec2 class-related methods
    
    Declaration/Definition file: animal/geometry/vec2.h
    (creation date: December 3, 1999).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 1999
    
    @see Vec2 */
//  
// -----------------------------------------------------
//@{

/** @name Dot and cross products */
//@{
/// Dot product
template <class RealT, class NumTraitsT>
RealT dot(const Vec2<RealT,NumTraitsT>& , const Vec2<RealT,NumTraitsT>& );

/// "Cross product"
template <class RealT, class NumTraitsT>
RealT cross(const Vec2<RealT,NumTraitsT>& , const Vec2<RealT,NumTraitsT>& );
//@}

/// Distance between p1 and p2
template <class RealT, class NumTraitsT>
RealT dist(const Vec2<RealT,NumTraitsT>& p1, const Vec2<RealT,NumTraitsT>& p2);

/// Square distance between p1 and p2
template <class RealT, class NumTraitsT>
RealT sqdist(const Vec2<RealT,NumTraitsT>& p1, const Vec2<RealT,NumTraitsT>& p2);

/// Cosinus of the (v1,v2) angle
template <class RealT, class NumTraitsT>
RealT cosAng(const Vec2<RealT,NumTraitsT>& v1, const Vec2<RealT,NumTraitsT>& v2);

//@} // Vec2 class-related methods










// -------------------------------------------------------------
// 
//   D E F I N I T I O N   O F   I N L I N E D   M E T H O D S 
// 
// -------------------------------------------------------------

template <class RealT, class NumTraitsT>
inline
Vec2<RealT,NumTraitsT>::
Vec2()
{}

template <class RealT, class NumTraitsT>
inline
Vec2<RealT,NumTraitsT>::
Vec2(const Real x0, const Real y0)
{
  a[0] = x0; a[1] = y0;
}





template <class RealT, class NumTraitsT>
inline const RealT&
Vec2<RealT,NumTraitsT>::
x() const 
{
  return a[0];
}

template <class RealT, class NumTraitsT>
inline const RealT&
Vec2<RealT,NumTraitsT>::
y() const 
{
  return a[1];
}

template <class RealT, class NumTraitsT>
inline const RealT&
Vec2<RealT,NumTraitsT>::
operator[](int i) const
{
  return a[i];
}





template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>&
Vec2<RealT,NumTraitsT>::
setx(const Real x0)
{
  a[0] = x0;
  return *this;
}

template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>&
Vec2<RealT,NumTraitsT>::
sety(const Real y0)
{
  a[1] = y0;
  return *this;
}

template <class RealT, class NumTraitsT>
inline RealT&
Vec2<RealT,NumTraitsT>::
operator[](int i)
{
  return a[i];
}





template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>&
Vec2<RealT,NumTraitsT>::
operator+=(const Vec2& v)
{
  a[0] += v.x();
  a[1] += v.y();
  return *this;
}

template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>&
Vec2<RealT,NumTraitsT>::
operator-=(const Vec2& v)
{
  a[0] -= v.x();
  a[1] -= v.y();
  return *this;
}

template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>&
Vec2<RealT,NumTraitsT>::
operator*=(const Real k)
{
  a[0] *= k;
  a[1] *= k;
  return *this;
}

template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>&
Vec2<RealT,NumTraitsT>::
operator/=(const Real k)
{
  (*this) *= 1.0/k;
  return *this;
}





template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>
Vec2<RealT,NumTraitsT>::
operator+(const Vec2& v) const
{
  return Vec2( a[0]+v.x(), a[1]+v.y() );
}

template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>
Vec2<RealT,NumTraitsT>::
operator-(const Vec2& v) const
{
  return Vec2( a[0]-v.x(), a[1]-v.y() );
}

template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>
Vec2<RealT,NumTraitsT>::
operator*(const Real k) const
{
  return Vec2(a[0]*k, a[1]*k);
}

template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>
operator*(const RealT k, const Vec2<RealT,NumTraitsT>& v)
{
  return v*k;
}

template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>
Vec2<RealT,NumTraitsT>::
operator/(const Real k) const
{
  return (*this)*(1.0/k);
}

template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>
Vec2<RealT,NumTraitsT>::
operator-()
{
  return Vec2( -x(), -y() );
}





template <class RealT, class NumTraitsT>
inline bool
Vec2<RealT,NumTraitsT>::
operator==(const Vec2& v) const
{
  return ( (v.x() == a[0]) && (v.y() == a[1]) );
}

template <class RealT, class NumTraitsT>
inline bool
Vec2<RealT,NumTraitsT>::
operator!=(const Vec2& v) const
{
  return !operator==(v);
}





template <class RealT, class NumTraitsT>
inline std::ostream&
operator<<(std::ostream& s, const Vec2<RealT,NumTraitsT>& v)
{
  return s << v.x() << " " << v.y();
}

template <class RealT, class NumTraitsT>
inline std::istream&
operator>>(std::istream& s, Vec2<RealT,NumTraitsT>& v)
{
  RealT x0 = 0.0;
  RealT y0 = 0.0;
  
  s >> x0 >> y0;
  v = Vec2<RealT,NumTraitsT>(x0, y0);
  return s;
}





template <class RealT, class NumTraitsT>
inline RealT
Vec2<RealT,NumTraitsT>::
norm() const 
{
  return NumTraitsT::sqroot( a[0]*a[0] + a[1]*a[1] );
}

template <class RealT, class NumTraitsT>
inline RealT
Vec2<RealT,NumTraitsT>::
sqnorm() const
{
  return ( a[0]*a[0] + a[1]*a[1] );
}

template <class RealT, class NumTraitsT>
inline RealT
Vec2<RealT,NumTraitsT>::
inftNorm() const
{
  Real x0 = a[0];
  Real y0 = a[1];
  
  if ( x0 < 0.0 ) x0 = -x0;
  if ( y0 < 0.0 ) y0 = -y0;
  
  if ( x0 < y0 )
    return y0;
  else
    return x0;
}

template <class RealT, class NumTraitsT>
inline Vec2<RealT,NumTraitsT>&
Vec2<RealT,NumTraitsT>::
normalize()
{
  (*this) *= 1.0/norm();
  return *this;
}





template <class RealT, class NumTraitsT>
inline const Vec2<RealT,NumTraitsT>
Vec2<RealT,NumTraitsT>::
null()
{
  return Vec2( Real(0.0), Real(0.0) );
}





template <class RealT, class NumTraitsT>
inline RealT
dot(const Vec2<RealT,NumTraitsT>& v1, const Vec2<RealT,NumTraitsT>& v2)
{
  return ( v1.x()*v2.x() + v1.y()*v2.y() );
}

template <class RealT, class NumTraitsT>
inline RealT
cross(const Vec2<RealT,NumTraitsT>& v1, const Vec2<RealT,NumTraitsT>& v2)
{
  return ( v1.x()*v2.y() - v1.y()*v2.x() );
}





template <class RealT, class NumTraitsT>
inline RealT
dist(const Vec2<RealT,NumTraitsT>& p1, const Vec2<RealT,NumTraitsT>& p2)
{
  return (p1 - p2).norm();
}

template <class RealT, class NumTraitsT>
inline RealT
sqdist(const Vec2<RealT,NumTraitsT>& p1, const Vec2<RealT,NumTraitsT>& p2)
{
  return (p1 - p2).sqnorm();
}

template <class RealT, class NumTraitsT>
inline RealT
cosAng(const Vec2<RealT,NumTraitsT>& v1, const Vec2<RealT,NumTraitsT>& v2)
{
  return dot(v1,v2)/NumTraitsT::sqroot( v1.sqnorm()*v2.sqnorm() );
}

} } // namespace animal { namespace geometry {



#endif // ANIMAL_GEOMETRY_VEC2_H
