#ifndef ANIMAL_SUPPORT_TRACKBALL_H
#define ANIMAL_SUPPORT_TRACKBALL_H

#include <animal/geometry/quaternion.h>



namespace animal { namespace support {

// ----------------------------------------------------------
//  
//  Trackball class.
/** Trackball in 3D space.
    
    Rewritten for animal purposes from trackball class
    by Pierre-Olivier Agliati (1999).
    See SIGGRAPH '96 Advanced OpenGL course notes.
    
    Declaration/Definition file: animal/support/trackball.h
    (creation date: January 17, 2000).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 2000 */
//  
// ----------------------------------------------------------

template <
  class RealT = double,
  class NumTraitsT = animal::traits::Numerics<RealT>,
  class IntT = int >

class Trackball
{

public:
  
  // Traits typedef
  typedef RealT Real_t;
  typedef IntT Int_t;
  typedef animal::geometry::Vec3<Real_t> Vec3_t;
  typedef animal::geometry::Quaternion<Real_t> Quat_t;
  
  
  /** Constructor: create a Trackball, specifying
      initial rotation and translation values,
      translation sensitivity and size */
  Trackball(Quat_t r_i = Quat_t::id(), Vec3_t t_i = Vec3_t::null(),
	    Real_t t_s = 10.0, Real_t s = 1.0);
  
  
  /** @name Set */
  //@{
  /// Set to a given transformation
  void setTransf(const Quat_t& , const Vec3_t& );
  
  /// Set initial transformation
  void setInitTransf(const Quat_t& , const Vec3_t& );
  
  /// Reset to initial transformation
  void reinitializeTransf();
  
  /// Set translation sensitivity
  void setTranslSenst(Real_t );
  
  /// Set size
  void setSize(Real_t );
  
  /** Set active range of the mouse (width, height)
      Call it when trackball is created or window viewport changes */
  void reshape(Int_t w, Int_t h);
  //@}
  
  
  /** @name Motion operations */
  //@{
  /// Write current transformation matrix, in OpenGL format
  void writeOpenGLTransfMatrix(Real_t m[4][4]) const;
  
  /// Start movement
  void startRotation(Int_t x, Int_t y);
  void startTranslationXY(Int_t x, Int_t y);
  void startTranslationZ(Int_t x, Int_t y);
  
  /// Movement
  void move(Int_t x, Int_t y);
  
  /// Stop movement
  void stopRotation();
  void stopTranslationXY();
  void stopTranslationZ();
  //@}
  
  
private:
  
  // Projection to deformed ball
  Real_t projectionToDeformedBall(Real_t x, Real_t y) const;
  
  // Build new quaternion when rotation is performed
  void buildQuaternion(Quat_t &q,
		       Real_t p1x, Real_t p1y,
		       Real_t p2x, Real_t p2y) const;
  
  // Compute transformation according to motion performed
  void moveRotation(Int_t x, Int_t y);
  void moveTranslationXY(Int_t x, Int_t y);
  void moveTranslationZ(Int_t x, Int_t y);
  
  
  // A trackball is defined using a quaternion and
  // a translation vector
  Quat_t rot, rot_init;
  Vec3_t transl, transl_init;
  
  // Translation sensitivity, size
  Real_t transl_senst, size;
  
  // Active range for x & y coordinates
  Int_t width, height;
  
  // Last defined coordinates
  Int_t last_x, last_y;
  
  // Current state
  bool is_rotd, is_transldXY, is_transldZ;
  
  // Square size, size limit (to handle size problems)
  Real_t sqsize, size_limit;
  
}; // class Trackball










// -------------------------------------------------------------
// 
//   D E F I N I T I O N   O F   I N L I N E D   M E T H O D S 
// 
// -------------------------------------------------------------





template <class RealT, class NumTraitsT, class IntT>
inline
Trackball<RealT,NumTraitsT,IntT>::
Trackball(Quat_t r_i, Vec3_t t_i, Real_t t_s, Real_t s)
: rot(r_i), rot_init(r_i),
  transl(t_i), transl_init(t_i),
  transl_senst(t_s),
  size(s), sqsize(size*size), size_limit(sqsize*0.5),
  width(), height(),
  last_x(), last_y(),
  is_rotd(false), is_transldXY(false), is_transldZ(false)
{}





template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
setTransf(const Quat_t& q, const Vec3_t& t)
{
  rot = q;
  transl = t;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
setInitTransf(const Quat_t& q, const Vec3_t& t)
{
  rot_init = q;
  transl_init = t;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
reinitializeTransf()
{
  setTransf(rot_init, transl_init);
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
setTranslSenst(Real_t t_s)
{
  transl_senst = t_s;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
setSize(Real_t s)
{
  size = s;
  sqsize = size*size;
  size_limit = sqsize*0.5;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
reshape(Int_t w, Int_t h)
{
  width = w; height = h;
}





template <class RealT, class NumTraitsT, class IntT>
inline RealT
Trackball<RealT,NumTraitsT,IntT>::
projectionToDeformedBall(Real_t x, Real_t y) const
{
  Real_t d = x*x + y*y;
  
  // Return z coordinate (?), i.e.
  // distance to ball center for small distances, with sqsize = d + z*z
  // else a decreasing value to zero
  if ( d < size_limit )
    return NumTraitsT::sqroot(sqsize - d);
  else
    return ( size_limit / NumTraitsT::sqroot(d) );
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
buildQuaternion(Quat_t& q,
		Real_t p1x, Real_t p1y, Real_t p2x, Real_t p2y) const
{
  // Case of null rotation
  if ( (p1x == p2x) && (p1y == p2y) )
    {
      q = Quat_t::id();
      return;
    }
  
  // Points coordinates are reported in the new 3D frame: origin at ball center,
  // x (respectively y) unit vector length equal to half screen width (respectively height),
  // z unit vector length approximately equal to distance from screen plane to ball center (?).
  Vec3_t p1( p1x, p1y, projectionToDeformedBall(p1x,p1y) );
  Vec3_t p2( p2x, p2y, projectionToDeformedBall(p2x,p2y) );
  
  // Axis of rotation
  Vec3_t axis = animal::geometry::cross(p2,p1);
  axis.normalize();
  
  // Approximation of rotation half-angle sinus
  Real_t sin_ha = (p1-p2).norm()/(2.0*size);
  
  // Clamp "out-of-control" values
  if ( sin_ha > 1.0 ) sin_ha = 1.0;
  else if ( sin_ha < -1.0 ) sin_ha = -1.0;
  
  Real_t cos_ha = NumTraitsT::sqroot(1.0 - sin_ha*sin_ha);
  
  // Update quaternion
  q.setw( cos_ha          );
  q.setx( axis.x()*sin_ha );
  q.sety( axis.y()*sin_ha );
  q.setz( axis.z()*sin_ha );
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
moveRotation(Int_t x, Int_t y)
{
  // Compute transformation quaternion
  Quat_t q;
  buildQuaternion( q,
		  static_cast<Real_t>( (2*last_x - width)  ) / static_cast<Real_t>(width ),
		  static_cast<Real_t>( (height - 2*last_y) ) / static_cast<Real_t>(height),
		  static_cast<Real_t>( (2*x - width)       ) / static_cast<Real_t>(width ),
		  static_cast<Real_t>( (height - 2*y)      ) / static_cast<Real_t>(height) );
  // Points coordinates are reported in the new 2D frame: origin at screen center,
  // x (respectively y) unit vector length equal to half screen width (respectively height).
  
  // Add to the global quaternion
  rot = q*rot;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
moveTranslationXY(Int_t x, Int_t y)
{
  transl[0] += (x - last_x)/transl_senst;
  transl[1] += (last_y - y)/transl_senst;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
moveTranslationZ(Int_t x, Int_t y)
{
  transl[2] += (y - last_y)/transl_senst;
}





template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
writeOpenGLTransfMatrix(Real_t m[4][4]) const
{
  /* We fill the transformation matrix */
  // Rotation & Scaling
  animal::geometry::writeOpenGLRotMatrix(rot,m);
  
  // Translation
  m[3][0] = transl[0];
  m[3][1] = transl[1];
  m[3][2] = transl[2];
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
startRotation(Int_t x, Int_t y)
{
  is_rotd = true;
  
  // Save current parameters
  last_x = x; last_y = y;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
startTranslationXY(Int_t x, Int_t y)
{
  is_transldXY = true;
  
  // Save current parameters
  last_x = x; last_y = y;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
startTranslationZ(Int_t x, Int_t y)
{
  is_transldZ = true;
  
  // Save current parameters
  last_x = x; last_y = y;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
move(Int_t x, Int_t y)
{
  // Handle every enabled transformation
  if ( is_rotd )
    moveRotation(x,y);
  
  if ( is_transldXY )
    moveTranslationXY(x,y);
  
  if ( is_transldZ )
    moveTranslationZ(x,y);
  
  // Save current parameters
  last_x = x; last_y = y;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
stopRotation()
{
  is_rotd = false;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
stopTranslationXY()
{
  is_transldXY = false;
}

template <class RealT, class NumTraitsT, class IntT>
inline void
Trackball<RealT,NumTraitsT,IntT>::
stopTranslationZ()
{
  is_transldZ = false;
}

} } // namespace animal { namespace support {



#endif // ANIMAL_SUPPORT_TRACKBALL_H
