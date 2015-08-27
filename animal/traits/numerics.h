#ifndef ANIMAL_TRAITS_NUMERICS_H
#define ANIMAL_TRAITS_NUMERICS_H

#include <math.h>
#include <float.h>



namespace animal { namespace traits {

// -------------------------------------------------------
//  
//  Numerics class.
/** Numerics general template.
    Defines numerical functions and constants.
    Create or derive your own version to customize
    your Numerics class.
    
    Declaration/Definition file: animal/traits/numerics.h
    (creation date: February 7, 2000).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 2000 */
//  
// -------------------------------------------------------

template <class RealT> class Numerics
{};



/// Numerics specialization for C++ float type
template <> class Numerics<float>
{

public:
  
  // Traits typedefs
  typedef int   Int;
  typedef float Real;
  
  
  /// Absolute value (floating point number)
  static Real fpabs(Real k) { return fabsf(k); }
  
  /// Square root
  static Real sqroot(Real k) { return sqrtf(k); }
  
  /// Sine
  static Real sine(Real k) { return sinf(k); }
  
  /// Cosine
  static Real cosine(Real k) { return cosf(k); }
  
  /// Arc Cosine
  static Real acosine(Real k) { return acosf(k); }
  
  /// Detection of nan/inf values
  //static Int isfinite(Real k) { return finite( static_cast<double>(k) ); } // DEPRECATED
  
  /// Numerical error threshold
  static Real numthreshold() { return FLT_EPSILON; }
  
}; // class Numerics<float>



/// Numerics specialization for C++ double type
template <> class Numerics<double>
{

public:
  
  // Traits typedefs
  typedef int    Int;
  typedef double Real;
  
  
  /// Absolute value (floating point number)
  static Real fpabs(Real k) { return fabs(k); }
  
  /// Square root
  static Real sqroot(Real k) { return sqrt(k); }
  
  /// Sine
  static Real sine(Real k) { return sin(k); }
  
  /// Cosine
  static Real cosine(Real k) { return cos(k); }
  
  /// Arc Cosine
  static Real acosine(Real k) { return acos(k); }
  
  /// Detection of nan/inf values
  //static Int isfinite(Real k) { return finite(k); } // DEPRECATED
  
  /// Numerical error threshold
  static Real numthreshold() { return DBL_EPSILON; }
  
}; // class Numerics<double>

} } // namespace animal { namespace traits {



#endif // ANIMAL_TRAITS_NUMERICS_H
