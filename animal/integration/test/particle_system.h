#ifndef ANIMAL_INTEGRATION_TEST_PARTICLE_SYSTEM_H
#define ANIMAL_INTEGRATION_TEST_PARTICLE_SYSTEM_H

#include <iostream>



/** Definition of types used by
    solver, stepper and driver example classes
*/

struct Particle_State
{
  double vel; // velocity (in m.s-1)
  double pos; // position (in m)
  
  Particle_State() : vel(), pos()
    {}
  Particle_State(double v, double p) : vel(v), pos(p)
    {}
  Particle_State operator+(const Particle_State& ps) const
    {
      return Particle_State(vel + ps.vel, pos + ps.pos);
    }
  Particle_State operator*(const double k) const
    {
      return Particle_State(vel*k, pos*k);
    }
  friend std::ostream& operator<<(std::ostream& s, const Particle_State& ps)
    {
      return s << ps.vel << " " << ps.pos;
    }
};

struct Particle_State_Derivative
{
  double acc; // acceleration (in m.s-2)
  double vel; // velocity (in m.s-1)
  
  Particle_State_Derivative() : acc(), vel()
    {}
  Particle_State_Derivative(double a, double v) : acc(a), vel(v)
    {}
  Particle_State_Derivative operator+(const Particle_State_Derivative& psd) const
    {
      return Particle_State_Derivative(acc + psd.acc, vel + psd.vel);
    }
  Particle_State_Derivative operator*(const double k) const
    {
      return Particle_State_Derivative(acc*k, vel*k);
    }
  friend std::ostream& operator<<(std::ostream& s, const Particle_State_Derivative& psd)
    {
      return s << psd.acc << " " << psd.vel;
    }
};

struct Particle_System_Model
{
  double m;  // particle mass (in kg)
  double k;  // spring stiffness (in N.m-1)
  double L0; // spring rest length (in m)
  
  Particle_System_Model() : m(), k(), L0()
    {}
  Particle_System_Model(double mass, double stiffness, double rest_length)
    : m(mass), k(stiffness), L0(rest_length)
    {}
};



#endif // ANIMAL_INTEGRATION_TEST_PARTICLE_SYSTEM_H
