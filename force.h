#ifndef FORCE_H
#define FORCE_H

#include <animal/geometry/vec3.h>

template <class TraitsT>
struct Force_Function
{
  typedef typename TraitsT::Real_t       Real_t;
  typedef typename TraitsT::Model_t      Model_t;
  typedef typename TraitsT::State_t      State_t;
  typedef typename TraitsT::Derivative_t Derivative_t;
  typedef typename TraitsT::Numerics_t   Numerics_t;
  
  typedef animal::geometry::Vec3<Real_t> Vec3_t;
};

#endif // FORCE_H
