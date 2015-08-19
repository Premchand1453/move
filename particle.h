#ifndef PARTICLE_H
#define PARTICLE_H

#include <iostream>
#include <GL/gl.h>
#include <animal/geometry/vec3.h>

typedef GLdouble Real;
typedef animal::geometry::Vec3<Real> Vec3;

struct Particle_State
{
  Vec3 vel; // velocity (in m.s-1)
  Vec3 pos; // position (in m)
  
  enum constraint_mode {NO_CONSTRAINT = 0,
                        FIXED         = 1,
                        PUSHED        = 2,
                        OBSERVED      = 3};
  // should be put into model...
  constraint_mode constraint;
  
  Particle_State() : vel(), pos(), constraint()
    {}
  Particle_State(const Vec3& v, const Vec3& p, const constraint_mode c)
    : vel(v), pos(p), constraint(c)
    {}
  Particle_State operator+(const Particle_State& ps) const
    {
      return Particle_State(vel + ps.vel, pos + ps.pos, constraint);
    }
  Particle_State operator*(const Real k) const
    {
      return Particle_State(vel*k, pos*k, constraint);
    }
  friend std::ostream& operator<<(std::ostream& s, const Particle_State& ps)
    {
      return s << ps.vel << " " << ps.pos << " " << ps.constraint;
    }
};

struct Particle_State_Derivative
{
  Vec3 acc; // acceleration (in m.s-2)
  Vec3 vel; // velocity (in m.s-1)
  
  Particle_State_Derivative() : acc(), vel()
    {}
  Particle_State_Derivative(const Vec3& a, const Vec3& v)
    : acc(a), vel(v)
    {}
  Particle_State_Derivative operator+(const Particle_State_Derivative& psd) const
    {
      return Particle_State_Derivative(acc + psd.acc, vel + psd.vel);
    }
  Particle_State_Derivative operator*(const Real k) const
    {
      return Particle_State_Derivative(acc*k, vel*k);
    }
  friend std::ostream& operator<<(std::ostream& s, const Particle_State_Derivative& psd)
    {
      return s << psd.acc << " " << psd.vel;
    }
};

struct Particle_Model
{
  Real m;  // particle mass (in kg)
  Vec3 f;  // particle force (in N)
  
  Particle_Model() : m(), f()
    {}
  Particle_Model(Real mass, Vec3 force)
    : m(mass), f(force)
    {}
};

#endif // PARTICLE_H
