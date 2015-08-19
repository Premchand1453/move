#ifndef SCHEME_H
#define SCHEME_H

#include <vector>
#include <animal/integration/explicit_solver.h>
#include "force.h"
#include "particle.h"

struct Particle_Traits :
  public animal::integration::
    Solver_Traits< double,
                   std::vector<Particle_State>,
		   std::vector<Particle_State_Derivative>,
		   std::vector<Particle_Model> >
{};

// Notice : avoid putting restrictions like "const" in function signatures...
// No one knows what is really useful!

template <class ForceF_Container> // could be put into model
struct Stoermer_Derivative :
  public animal::integration::Derivative_Function<Particle_Traits>
{
  ForceF_Container F;
  
  Stoermer_Derivative() : F()
    {}
  Stoermer_Derivative(const ForceF_Container& ffc) : F(ffc)
    {}
  
  void operator()(Model_t& M,
		  const State_t& S,
		  Derivative_t& D,
		  const Real_t t)
    {
      typename ForceF_Container::iterator first_F = F.begin();
      typename ForceF_Container::iterator last_F  = F.end();
      
      for ( ;
	    first_F != last_F;
	    ++first_F
	  )
	{
	  (*first_F)(M, S);
	}
      
      const Real kd = 5.0e-03;      // coefficient of drag
      const Vec3 g(0.0, -9.8, 0.0); // gravitational constant
      const Vec3 p(0.0, -1.0, 0.0); // push force -1.0 for m-s systems, -1.5 elsewhere
      
      Model_t::iterator       first_M = M.begin();
      State_t::const_iterator first_S = S.begin();
      State_t::const_iterator last_S  = S.end();
      Derivative_t::iterator  first_D = D.begin();
      
      for ( ;
	    first_S != last_S;
	    ++first_M, ++first_S, ++first_D
	  )
	{
	  Particle_State::constraint_mode cst = (*first_S).constraint;
	  
	  if ( cst == Particle_State::NO_CONSTRAINT
#if MEASURE
	       || cst == Particle_State::OBSERVED
#endif
	     )
	    {
#if MEASURE
	      if ( cst == Particle_State::OBSERVED )
	        {
		  cout << (*first_S).pos << endl;
	        }
#endif
	      Vec3 force = (*first_M).f;
	      Real mass  = (*first_M).m;
	      
	      force += - kd * (*first_S).vel; // viscous drag
	      force += mass * g;              // gravitational force
	      
	      (*first_D).acc = force/mass;
	      (*first_M).f = Vec3::null();    // clear force
	    }
	  else if ( cst == Particle_State::PUSHED )
	    {
	      Vec3 force = (*first_M).f;
	      Real mass  = (*first_M).m;
	      
	      force += - kd * (*first_S).vel; // viscous drag
	      force += mass * g;              // gravitational force
	      force += p;                     // push
	      
	      (*first_D).acc = force/mass;
	      (*first_M).f = Vec3::null();    // clear force
	    }
	}
    }
};

struct Stoermer_Step :
  public animal::integration::Step_Function<Particle_Traits>
{
  void operator()(const State_t& initial_S,
		  State_t& final_S,
		  const Derivative_t& D,
		  const Real_t h) const // should contain Model_t too
    {
      Real_t sqh = h*h;
      
      State_t::const_iterator      first_iS = initial_S.begin();
      State_t::const_iterator      last_iS  = initial_S.end();
      State_t::iterator            first_fS = final_S.begin();
      Derivative_t::const_iterator first_D  = D.begin();
      
      for ( ;
	    first_iS != last_iS;
	    ++first_iS, ++first_fS, ++first_D
	  )
	{
	  Particle_State::constraint_mode cst = (*first_iS).constraint;
	  
	  if ( cst == Particle_State::NO_CONSTRAINT
	       || cst == Particle_State::PUSHED
#if MEASURE
	       || cst == Particle_State::OBSERVED
#endif
	     )
	    {
	      (*first_fS).vel = (*first_iS).vel + sqh*(*first_D).acc;
	      (*first_fS).pos = (*first_iS).pos + (*first_fS).vel;
	    }
	}
    }
};

struct Spring : public Force_Function<Particle_Traits>
{
  State_t::size_type p0, p1; // indices
  
  Real_t ks; // stiffness constant
  Real_t L0; // rest length
  
#if DAMPED
  Real_t kd; // damping constant
#endif
  
  Spring()
    {}
  Spring(const State_t::size_type i0, const State_t::size_type i1,
	 const Real_t stiffness, const Real_t damping,
	 const Real_t rest_length)
    {
      p0 = i0;
      p1 = i1;
      ks = stiffness;
      L0 = rest_length;
#if DAMPED
      kd = damping;
#endif
    }
  
  void operator()(Model_t& M, const State_t& S) const
    {
      Vec3_t l = S[p0].pos - S[p1].pos;
      
      Real_t L = l.norm();
      Vec3_t nl = l/L;
      
#if DAMPED
      Vec3_t v = S[p0].vel - S[p1].vel;
      Vec3_t F = - ( ks*(L - L0) + kd*animal::geometry::dot(v,nl) ) * nl;
#else
      Vec3_t F = - ( ks*(L - L0) ) * nl;
#endif
      
      M[p0].f +=   F;
      M[p1].f += - F;
    }
};

struct TetraSpring : public Force_Function<Particle_Traits>
{
  State_t::size_type p0, p1, p2, p3; // particles indices
  Vec3_t f1, ff1, f2, ff2, f3, ff3;  // intersection points
  
  int vi[6][3];    // vertices indices
  Real_t cf[6][3]; // interpolation coefs
  
  Real_t ks1, ks2, ks3, ks4, ks5, ks6; // stiffness constant
  Real_t L01, L02, L03;                // rest length
  
#if DAMPED
  Real_t kd1, kd2, kd3;                // damping constant
#endif
  
#if CONSTVOL
  Real_t ks; // volume stiffness
  Real_t L0; // volume rest length
#endif
  
  TetraSpring()
    {}
  TetraSpring(const State_t::size_type i0, const State_t::size_type i1,
	      const State_t::size_type i2, const State_t::size_type i3,
	      const int tabv[6][3], const Real_t tabc[6][3],
	      const Real_t s1, const Real_t s2, const Real_t s3,
	      const Real_t s4, const Real_t s5, const Real_t s6,
	      const Real_t d1, const Real_t d2, const Real_t d3,
	      const Real_t s, const Real_t rl,
	      const Vec3_t ip1, const Vec3_t ip2, const Vec3_t ip3,
	      const Vec3_t ip4, const Vec3_t ip5, const Vec3_t ip6)
    {
      p0 = i0; p1 = i1; p2 = i2; p3 = i3;
      
      f1  = ip1; ff1 = ip2; f2  = ip3;
      ff2 = ip4; f3  = ip5; ff3 = ip6;
      
      for (int i = 0; i < 6; ++i)
	for (int j = 0; j < 3; ++j)
	  {
	    vi[i][j] = tabv[i][j];
	    cf[i][j] = tabc[i][j];
	  }
      
      ks1 = s1; ks2 = s2; ks3 = s3;
      ks4 = s4; ks5 = s5; ks6 = s6;
      
#if DAMPED
      kd1 = d1; kd2 = d2; kd3 = d3;
#endif
      
      Vec3_t l1 = f1 - ff1;
      Vec3_t l2 = f2 - ff2;
      Vec3_t l3 = f3 - ff3;
      
      L01 = l1.norm();
      L02 = l2.norm();
      L03 = l3.norm();
      
#if CONSTVOL
      ks = s;
      L0 = rl;
#endif
    }
  
  void operator()(Model_t& M, const State_t& S)
    {
      Vec3_t pos[4] = { S[p0].pos, S[p1].pos, S[p2].pos, S[p3].pos };
      
      f1  = cf[0][0] * pos[vi[0][0]] + cf[0][1] * pos[vi[0][1]] + cf[0][2] * pos[vi[0][2]];
      ff1 = cf[1][0] * pos[vi[1][0]] + cf[1][1] * pos[vi[1][1]] + cf[1][2] * pos[vi[1][2]];
      f2  = cf[2][0] * pos[vi[2][0]] + cf[2][1] * pos[vi[2][1]] + cf[2][2] * pos[vi[2][2]];
      ff2 = cf[3][0] * pos[vi[3][0]] + cf[3][1] * pos[vi[3][1]] + cf[3][2] * pos[vi[3][2]];
      f3  = cf[4][0] * pos[vi[4][0]] + cf[4][1] * pos[vi[4][1]] + cf[4][2] * pos[vi[4][2]];
      ff3 = cf[5][0] * pos[vi[5][0]] + cf[5][1] * pos[vi[5][1]] + cf[5][2] * pos[vi[5][2]];
      
      Vec3_t l1 = f1 - ff1;
      Vec3_t l2 = f2 - ff2;
      Vec3_t l3 = f3 - ff3;
      
      Real_t L1 = l1.norm();
      Real_t L2 = l2.norm();
      Real_t L3 = l3.norm();
      
      Real_t L1_inv = 1.0/L1;
      Real_t L2_inv = 1.0/L2;
      Real_t L3_inv = 1.0/L3;
      
#if DAMPED
      Vec3_t vel[4] = { S[p0].vel, S[p1].vel, S[p2].vel, S[p3].vel };
      
      Vec3_t vf1  = cf[0][0] * vel[vi[0][0]] + cf[0][1] * vel[vi[0][1]] + cf[0][2] * vel[vi[0][2]];
      Vec3_t vff1 = cf[1][0] * vel[vi[1][0]] + cf[1][1] * vel[vi[1][1]] + cf[1][2] * vel[vi[1][2]];
      Vec3_t vf2  = cf[2][0] * vel[vi[2][0]] + cf[2][1] * vel[vi[2][1]] + cf[2][2] * vel[vi[2][2]];
      Vec3_t vff2 = cf[3][0] * vel[vi[3][0]] + cf[3][1] * vel[vi[3][1]] + cf[3][2] * vel[vi[3][2]];
      Vec3_t vf3  = cf[4][0] * vel[vi[4][0]] + cf[4][1] * vel[vi[4][1]] + cf[4][2] * vel[vi[4][2]];
      Vec3_t vff3 = cf[5][0] * vel[vi[5][0]] + cf[5][1] * vel[vi[5][1]] + cf[5][2] * vel[vi[5][2]];
      
      Vec3_t vl1 = vf1 - vff1;
      Vec3_t vl2 = vf2 - vff2;
      Vec3_t vl3 = vf3 - vff3;
#endif
      
      Real_t cosang12 = animal::geometry::dot(l1,l2)*L1_inv*L2_inv;
      Real_t cosang13 = animal::geometry::dot(l1,l3)*L1_inv*L3_inv;
      Real_t cosang23 = animal::geometry::dot(l2,l3)*L2_inv*L3_inv;
      
#if ALTERN // Faster!
      Vec3_t nl1 = l1*L1_inv;
      Vec3_t nl2 = l2*L2_inv;
      Vec3_t nl3 = l3*L3_inv;
      
#if DAMPED
      Vec3_t F1 = - ( ks1*(L1 - L01) + kd1*animal::geometry::dot(vl1,nl1) ) * nl1;
      Vec3_t F2 = - ( ks2*(L2 - L02) + kd2*animal::geometry::dot(vl2,nl2) ) * nl2;
      Vec3_t F3 = - ( ks3*(L3 - L03) + kd3*animal::geometry::dot(vl3,nl3) ) * nl3;
#else
      Vec3_t F1 = - ( ks1*(L1 - L01) ) * nl1;
      Vec3_t F2 = - ( ks2*(L2 - L02) ) * nl2;
      Vec3_t F3 = - ( ks3*(L3 - L03) ) * nl3;
#endif
      
      Real K12  = - ( ks4*cosang12 );
      Real K13  = - ( ks5*cosang13 );
      Real K23  = - ( ks6*cosang23 );
      
      Vec3_t Ff1  =   F1 + K12*nl2 + K13*nl3;
      Vec3_t Fff1 = - Ff1;
      
      Vec3_t Ff2  =   F2 + K12*nl1 + K23*nl3;
      Vec3_t Fff2 = - Ff2;
      
      Vec3_t Ff3  =   F3 + K13*nl1 + K23*nl2;
      Vec3_t Fff3 = - Ff3;
#else
      Vec3_t normal12  = animal::geometry::cross(l1, l2);
      Vec3_t n112 = animal::geometry::cross(l1, normal12).normalize();
      Vec3_t n212 = animal::geometry::cross(l2, normal12).normalize();
      
      Vec3_t normal13  = animal::geometry::cross(l1, l3);
      Vec3_t n113 = animal::geometry::cross(l1, normal13).normalize();
      Vec3_t n313 = animal::geometry::cross(l3, normal13).normalize();
      
      Vec3_t normal23  = animal::geometry::cross(l2, l3);
      Vec3_t n223 = animal::geometry::cross(l2, normal23).normalize();
      Vec3_t n323 = animal::geometry::cross(l3, normal23).normalize();
      
#if DAMPED
      Vec3_t nl1 = l1*L1_inv;
      Vec3_t nl2 = l2*L2_inv;
      Vec3_t nl3 = l3*L3_inv;
      
      Vec3_t F1 = - ( ks1*(L1 - L01) + kd1*animal::geometry::dot(vl1,nl1) ) * nl1;
      Vec3_t F2 = - ( ks2*(L2 - L02) + kd2*animal::geometry::dot(vl2,nl2) ) * nl2;
      Vec3_t F3 = - ( ks3*(L3 - L03) + kd3*animal::geometry::dot(vl3,nl3) ) * nl3;
#else
      Vec3_t F1 = - ( ks1*(L1 - L01) ) * (l1*L1_inv);
      Vec3_t F2 = - ( ks2*(L2 - L02) ) * (l2*L2_inv);
      Vec3_t F3 = - ( ks3*(L3 - L03) ) * (l3*L3_inv);
#endif
      
      Real K12 = ( ks4*cosang12 );
      Real K13 = ( ks5*cosang13 );
      Real K23 = ( ks6*cosang23 );
      
      Vec3_t Ff1  =   F1 + K12*n112 + K13*n113;
      Vec3_t Fff1 = - Ff1;
      
      Vec3_t Ff2  =   F2 - K12*n212 + K23*n223;
      Vec3_t Fff2 = - Ff2;
      
      Vec3_t Ff3  =   F3 - K13*n313 - K23*n323;
      Vec3_t Fff3 = - Ff3;
#endif
      
      Vec3_t frc[4] = { Vec3_t::null(), Vec3_t::null(), Vec3_t::null(), Vec3_t::null() };
      
      frc[vi[0][0]] += cf[0][0] * Ff1;
      frc[vi[0][1]] += cf[0][1] * Ff1;
      frc[vi[0][2]] += cf[0][2] * Ff1;
      
      frc[vi[1][0]] += cf[1][0] * Fff1;
      frc[vi[1][1]] += cf[1][1] * Fff1;
      frc[vi[1][2]] += cf[1][2] * Fff1;
      
      frc[vi[2][0]] += cf[2][0] * Ff2;
      frc[vi[2][1]] += cf[2][1] * Ff2;
      frc[vi[2][2]] += cf[2][2] * Ff2;
      
      frc[vi[3][0]] += cf[3][0] * Fff2;
      frc[vi[3][1]] += cf[3][1] * Fff2;
      frc[vi[3][2]] += cf[3][2] * Fff2;
      
      frc[vi[4][0]] += cf[4][0] * Ff3;
      frc[vi[4][1]] += cf[4][1] * Ff3;
      frc[vi[4][2]] += cf[4][2] * Ff3;
      
      frc[vi[5][0]] += cf[5][0] * Fff3;
      frc[vi[5][1]] += cf[5][1] * Fff3;
      frc[vi[5][2]] += cf[5][2] * Fff3;
      
#if CONSTVOL
      Vec3_t g_pos = 0.25*(pos[0] + pos[1] + pos[2] + pos[3]);
      
      Vec3_t d0 = pos[0] - g_pos;
      Vec3_t d1 = pos[1] - g_pos;
      Vec3_t d2 = pos[2] - g_pos;
      Vec3_t d3 = pos[3] - g_pos;
      
      Real_t D0 = d0.norm();
      Real_t D1 = d1.norm();
      Real_t D2 = d2.norm();
      Real_t D3 = d3.norm();
      
      Real_t D = D0 + D1 + D2 + D3;
      
      Real_t K = - ( ks*(D - L0) );
      
      M[p0].f += frc[0] + K * (d0/D0);
      M[p1].f += frc[1] + K * (d1/D1);
      M[p2].f += frc[2] + K * (d2/D2);
      M[p3].f += frc[3] + K * (d3/D3);
#else
      M[p0].f += frc[0];
      M[p1].f += frc[1];
      M[p2].f += frc[2];
      M[p3].f += frc[3];
#endif
    }
};

struct HexaSpring : public Force_Function<Particle_Traits>
{
  State_t::size_type p0, p1, p2, p3, p4, p5, p6, p7; // particles indices
  Vec3_t f1, ff1, f2, ff2, f3, ff3;                  // intersection points
  
  int vi[6][4];    // vertices indices
  Real_t cf[6][4]; // interpolation coefs
  
  Real_t ks1, ks2, ks3, ks4, ks5, ks6; // stiffness constant
  Real_t L01, L02, L03;                // rest length
  
#if DAMPED
  Real_t kd1, kd2, kd3;                // damping constant
#endif
  
#if CONSTVOL
  Real_t ks; // volume stiffness
#if ALTERN
  Real_t D00, D01, D02, D03, D04, D05, D06, D07;
#if DAMPED
  Real_t kd;
#endif
#else
  Real_t L0; // volume rest length
#endif
#endif
  
  HexaSpring()
    {}
  HexaSpring(const State_t::size_type i0, const State_t::size_type i1,
	     const State_t::size_type i2, const State_t::size_type i3,
	     const State_t::size_type i4, const State_t::size_type i5,
	     const State_t::size_type i6, const State_t::size_type i7,
	     const int tabv[6][4], const Real_t tabc[6][4],
	     const Real_t s1, const Real_t s2, const Real_t s3,
	     const Real_t s4, const Real_t s5, const Real_t s6,
	     const Real_t d1, const Real_t d2, const Real_t d3,
	     const Real_t s, const Real_t d, const Real_t rl,
	     const Real_t rl0, const Real_t rl1, const Real_t rl2, const Real_t rl3,
	     const Real_t rl4, const Real_t rl5, const Real_t rl6, const Real_t rl7,
	     const Vec3_t ip1, const Vec3_t ip2, const Vec3_t ip3,
	     const Vec3_t ip4, const Vec3_t ip5, const Vec3_t ip6)
    {
      p0 = i0; p1 = i1; p2 = i2; p3 = i3;
      p4 = i4; p5 = i5; p6 = i6; p7 = i7;
      
      f1  = ip1; ff1 = ip2; f2  = ip3;
      ff2 = ip4; f3  = ip5; ff3 = ip6;
      
      for (int i = 0; i < 6; ++i)
	for (int j = 0; j < 4; ++j)
	  {
	    vi[i][j] = tabv[i][j];
	    cf[i][j] = tabc[i][j];
	  }
      
      ks1 = s1; ks2 = s2; ks3 = s3;
      ks4 = s4; ks5 = s5; ks6 = s6;
      
#if DAMPED      
      kd1 = d1; kd2 = d2; kd3 = d3;
#endif
      
      Vec3_t l1 = f1 - ff1;
      Vec3_t l2 = f2 - ff2;
      Vec3_t l3 = f3 - ff3;
      
      L01 = l1.norm();
      L02 = l2.norm();
      L03 = l3.norm();
      
#if CONSTVOL
      ks = s;
#if ALTERN
      D00 = rl0; D01 = rl1; D02 = rl2; D03 = rl3;
      D04 = rl4; D05 = rl5; D06 = rl6; D07 = rl7;
#if DAMPED
      kd = d;
#endif
#else
      L0 = rl;
#endif
#endif
    }
  
  void operator()(Model_t& M, const State_t& S)
    {
      Vec3_t pos[8] =
        {
	  S[p0].pos, S[p1].pos, S[p2].pos, S[p3].pos,
	  S[p4].pos, S[p5].pos, S[p6].pos, S[p7].pos
        };
      
      f1  = cf[0][0] * cf[0][1] * pos[vi[0][0]] +
	    cf[0][2] * cf[0][1] * pos[vi[0][1]] +
	    cf[0][2] * cf[0][3] * pos[vi[0][2]] +
	    cf[0][0] * cf[0][3] * pos[vi[0][3]];
      
      ff1 = cf[1][0] * cf[1][1] * pos[vi[1][0]] +
	    cf[1][2] * cf[1][1] * pos[vi[1][1]] +
	    cf[1][2] * cf[1][3] * pos[vi[1][2]] +
	    cf[1][0] * cf[1][3] * pos[vi[1][3]];
      
      f2  = cf[2][0] * cf[2][1] * pos[vi[2][0]] +
	    cf[2][2] * cf[2][1] * pos[vi[2][1]] +
	    cf[2][2] * cf[2][3] * pos[vi[2][2]] +
	    cf[2][0] * cf[2][3] * pos[vi[2][3]];
      
      ff2 = cf[3][0] * cf[3][1] * pos[vi[3][0]] +
	    cf[3][2] * cf[3][1] * pos[vi[3][1]] +
	    cf[3][2] * cf[3][3] * pos[vi[3][2]] +
	    cf[3][0] * cf[3][3] * pos[vi[3][3]];
      
      f3  = cf[4][0] * cf[4][1] * pos[vi[4][0]] +
	    cf[4][2] * cf[4][1] * pos[vi[4][1]] +
	    cf[4][2] * cf[4][3] * pos[vi[4][2]] +
	    cf[4][0] * cf[4][3] * pos[vi[4][3]];
      
      ff3 = cf[5][0] * cf[5][1] * pos[vi[5][0]] +
	    cf[5][2] * cf[5][1] * pos[vi[5][1]] +
	    cf[5][2] * cf[5][3] * pos[vi[5][2]] +
	    cf[5][0] * cf[5][3] * pos[vi[5][3]];
      
      Vec3_t l1 = f1 - ff1;
      Vec3_t l2 = f2 - ff2;
      Vec3_t l3 = f3 - ff3;
      
      Real_t L1 = l1.norm();
      Real_t L2 = l2.norm();
      Real_t L3 = l3.norm();
      
      Real_t L1_inv = 1.0/L1;
      Real_t L2_inv = 1.0/L2;
      Real_t L3_inv = 1.0/L3;
      
#if DAMPED
      Vec3_t vel[8] =
        {
	  S[p0].vel, S[p1].vel, S[p2].vel, S[p3].vel,
	  S[p4].vel, S[p5].vel, S[p6].vel, S[p7].vel
        };
      
      Vec3_t vf1  = cf[0][0] * cf[0][1] * vel[vi[0][0]] +
	            cf[0][2] * cf[0][1] * vel[vi[0][1]] +
	            cf[0][2] * cf[0][3] * vel[vi[0][2]] +
	            cf[0][0] * cf[0][3] * vel[vi[0][3]];
      
      Vec3_t vff1 = cf[1][0] * cf[1][1] * vel[vi[1][0]] +
	            cf[1][2] * cf[1][1] * vel[vi[1][1]] +
	            cf[1][2] * cf[1][3] * vel[vi[1][2]] +
	            cf[1][0] * cf[1][3] * vel[vi[1][3]];
      
      Vec3_t vf2  = cf[2][0] * cf[2][1] * vel[vi[2][0]] +
	            cf[2][2] * cf[2][1] * vel[vi[2][1]] +
	            cf[2][2] * cf[2][3] * vel[vi[2][2]] +
	            cf[2][0] * cf[2][3] * vel[vi[2][3]];
      
      Vec3_t vff2 = cf[3][0] * cf[3][1] * vel[vi[3][0]] +
	            cf[3][2] * cf[3][1] * vel[vi[3][1]] +
	            cf[3][2] * cf[3][3] * vel[vi[3][2]] +
	            cf[3][0] * cf[3][3] * vel[vi[3][3]];
      
      Vec3_t vf3  = cf[4][0] * cf[4][1] * vel[vi[4][0]] +
	            cf[4][2] * cf[4][1] * vel[vi[4][1]] +
	            cf[4][2] * cf[4][3] * vel[vi[4][2]] +
	            cf[4][0] * cf[4][3] * vel[vi[4][3]];
      
      Vec3_t vff3 = cf[5][0] * cf[5][1] * vel[vi[5][0]] +
	            cf[5][2] * cf[5][1] * vel[vi[5][1]] +
	            cf[5][2] * cf[5][3] * vel[vi[5][2]] +
	            cf[5][0] * cf[5][3] * vel[vi[5][3]];
      
      Vec3_t vl1 = vf1 - vff1;
      Vec3_t vl2 = vf2 - vff2;
      Vec3_t vl3 = vf3 - vff3;
#endif
      
      Real_t cosang12 = animal::geometry::dot(l1,l2)*L1_inv*L2_inv;
      Real_t cosang13 = animal::geometry::dot(l1,l3)*L1_inv*L3_inv;
      Real_t cosang23 = animal::geometry::dot(l2,l3)*L2_inv*L3_inv;
      
#if ALTERN // Faster!
      Vec3_t nl1 = l1*L1_inv;
      Vec3_t nl2 = l2*L2_inv;
      Vec3_t nl3 = l3*L3_inv;
      
#if DAMPED
      Vec3_t F1 = - ( ks1*(L1 - L01) + kd1*animal::geometry::dot(vl1,nl1) ) * nl1;
      Vec3_t F2 = - ( ks2*(L2 - L02) + kd2*animal::geometry::dot(vl2,nl2) ) * nl2;
      Vec3_t F3 = - ( ks3*(L3 - L03) + kd3*animal::geometry::dot(vl3,nl3) ) * nl3;
#else
      Vec3_t F1 = - ( ks1*(L1 - L01) ) * nl1;
      Vec3_t F2 = - ( ks2*(L2 - L02) ) * nl2;
      Vec3_t F3 = - ( ks3*(L3 - L03) ) * nl3;
#endif
      
      Real K12  = - ( ks4*cosang12 );
      Real K13  = - ( ks5*cosang13 );
      Real K23  = - ( ks6*cosang23 );
      
      Vec3_t Ff1  =   F1 + K12*nl2 + K13*nl3;
      Vec3_t Fff1 = - Ff1;
      
      Vec3_t Ff2  =   F2 + K12*nl1 + K23*nl3;
      Vec3_t Fff2 = - Ff2;
      
      Vec3_t Ff3  =   F3 + K13*nl1 + K23*nl2;
      Vec3_t Fff3 = - Ff3;
#else
      Vec3_t normal12  = animal::geometry::cross(l1, l2);
      Vec3_t n112 = animal::geometry::cross(l1, normal12).normalize();
      Vec3_t n212 = animal::geometry::cross(l2, normal12).normalize();
      
      Vec3_t normal13  = animal::geometry::cross(l1, l3);
      Vec3_t n113 = animal::geometry::cross(l1, normal13).normalize();
      Vec3_t n313 = animal::geometry::cross(l3, normal13).normalize();
      
      Vec3_t normal23  = animal::geometry::cross(l2, l3);
      Vec3_t n223 = animal::geometry::cross(l2, normal23).normalize();
      Vec3_t n323 = animal::geometry::cross(l3, normal23).normalize();
      
#if DAMPED
      Vec3_t nl1 = l1*L1_inv;
      Vec3_t nl2 = l2*L2_inv;
      Vec3_t nl3 = l3*L3_inv;
      
      Vec3_t F1 = - ( ks1*(L1 - L01) + kd1*animal::geometry::dot(vl1,nl1) ) * nl1;
      Vec3_t F2 = - ( ks2*(L2 - L02) + kd2*animal::geometry::dot(vl2,nl2) ) * nl2;
      Vec3_t F3 = - ( ks3*(L3 - L03) + kd3*animal::geometry::dot(vl3,nl3) ) * nl3;
#else
      Vec3_t F1 = - ( ks1*(L1 - L01) ) * (l1*L1_inv);
      Vec3_t F2 = - ( ks2*(L2 - L02) ) * (l2*L2_inv);
      Vec3_t F3 = - ( ks3*(L3 - L03) ) * (l3*L3_inv);
#endif
      
      Real K12 = ( ks4*cosang12 );
      Real K13 = ( ks5*cosang13 );
      Real K23 = ( ks6*cosang23 );
      
      Vec3_t Ff1  =   F1 + K12*n112 + K13*n113;
      Vec3_t Fff1 = - Ff1;
      
      Vec3_t Ff2  =   F2 - K12*n212 + K23*n223;
      Vec3_t Fff2 = - Ff2;
      
      Vec3_t Ff3  =   F3 - K13*n313 - K23*n323;
      Vec3_t Fff3 = - Ff3;
#endif
      
      Vec3_t frc[8] =
        {
	  Vec3_t::null(), Vec3_t::null(), Vec3_t::null(), Vec3_t::null(),
          Vec3_t::null(), Vec3_t::null(), Vec3_t::null(), Vec3_t::null()
	};
      
      frc[vi[0][0]] += cf[0][0] * cf[0][1] * Ff1;
      frc[vi[0][1]] += cf[0][2] * cf[0][1] * Ff1;
      frc[vi[0][2]] += cf[0][2] * cf[0][3] * Ff1;
      frc[vi[0][3]] += cf[0][0] * cf[0][3] * Ff1;
      
      frc[vi[1][0]] += cf[1][0] * cf[1][1] * Fff1;
      frc[vi[1][1]] += cf[1][2] * cf[1][1] * Fff1;
      frc[vi[1][2]] += cf[1][2] * cf[1][3] * Fff1;
      frc[vi[1][3]] += cf[1][0] * cf[1][3] * Fff1;
      
      frc[vi[2][0]] += cf[2][0] * cf[2][1] * Ff2;
      frc[vi[2][1]] += cf[2][2] * cf[2][1] * Ff2;
      frc[vi[2][2]] += cf[2][2] * cf[2][3] * Ff2;
      frc[vi[2][3]] += cf[2][0] * cf[2][3] * Ff2;
      
      frc[vi[3][0]] += cf[3][0] * cf[3][1] * Fff2;
      frc[vi[3][1]] += cf[3][2] * cf[3][1] * Fff2;
      frc[vi[3][2]] += cf[3][2] * cf[3][3] * Fff2;
      frc[vi[3][3]] += cf[3][0] * cf[3][3] * Fff2;
      
      frc[vi[4][0]] += cf[4][0] * cf[4][1] * Ff3;
      frc[vi[4][1]] += cf[4][2] * cf[4][1] * Ff3;
      frc[vi[4][2]] += cf[4][2] * cf[4][3] * Ff3;
      frc[vi[4][3]] += cf[4][0] * cf[4][3] * Ff3;
      
      frc[vi[5][0]] += cf[5][0] * cf[5][1] * Fff3;
      frc[vi[5][1]] += cf[5][2] * cf[5][1] * Fff3;
      frc[vi[5][2]] += cf[5][2] * cf[5][3] * Fff3;
      frc[vi[5][3]] += cf[5][0] * cf[5][3] * Fff3;
      
#if CONSTVOL
      Vec3_t g_pos = 0.125*( pos[0] + pos[1] + pos[2] + pos[3] +
			     pos[4] + pos[5] + pos[6] + pos[7]   );
      
      Vec3_t d0 = pos[0] - g_pos;
      Vec3_t d1 = pos[1] - g_pos;
      Vec3_t d2 = pos[2] - g_pos;
      Vec3_t d3 = pos[3] - g_pos;
      Vec3_t d4 = pos[4] - g_pos;
      Vec3_t d5 = pos[5] - g_pos;
      Vec3_t d6 = pos[6] - g_pos;
      Vec3_t d7 = pos[7] - g_pos;
      
      Real_t D0 = d0.norm();
      Real_t D1 = d1.norm();
      Real_t D2 = d2.norm();
      Real_t D3 = d3.norm();
      Real_t D4 = d4.norm();
      Real_t D5 = d5.norm();
      Real_t D6 = d6.norm();
      Real_t D7 = d7.norm();
      
#if ALTERN
#if DAMPED
      Vec3_t g_vel = 0.125*( vel[0] + vel[1] + vel[2] + vel[3] +
			     vel[4] + vel[5] + vel[6] + vel[7]   );
      
      Vec3_t vd0 = vel[0] - g_vel;
      Vec3_t vd1 = vel[1] - g_vel;
      Vec3_t vd2 = vel[2] - g_vel;
      Vec3_t vd3 = vel[3] - g_vel;
      Vec3_t vd4 = vel[4] - g_vel;
      Vec3_t vd5 = vel[5] - g_vel;
      Vec3_t vd6 = vel[6] - g_vel;
      Vec3_t vd7 = vel[7] - g_vel;
      
      Vec3_t nd0 = d0/D0;
      Vec3_t nd1 = d1/D1;
      Vec3_t nd2 = d2/D2;
      Vec3_t nd3 = d3/D3;
      Vec3_t nd4 = d4/D4;
      Vec3_t nd5 = d5/D5;
      Vec3_t nd6 = d6/D6;
      Vec3_t nd7 = d7/D7;
      
      Vec3_t Fd0 = - ( ks*(D0 - D00) + kd*animal::geometry::dot(vd0,nd0) ) * nd0;
      Vec3_t Fd1 = - ( ks*(D1 - D01) + kd*animal::geometry::dot(vd1,nd1) ) * nd1;
      Vec3_t Fd2 = - ( ks*(D2 - D02) + kd*animal::geometry::dot(vd2,nd2) ) * nd2;
      Vec3_t Fd3 = - ( ks*(D3 - D03) + kd*animal::geometry::dot(vd3,nd3) ) * nd3;
      Vec3_t Fd4 = - ( ks*(D4 - D04) + kd*animal::geometry::dot(vd4,nd4) ) * nd4;
      Vec3_t Fd5 = - ( ks*(D5 - D05) + kd*animal::geometry::dot(vd5,nd5) ) * nd5;
      Vec3_t Fd6 = - ( ks*(D6 - D06) + kd*animal::geometry::dot(vd6,nd6) ) * nd6;
      Vec3_t Fd7 = - ( ks*(D7 - D07) + kd*animal::geometry::dot(vd7,nd7) ) * nd7;
      
      M[p0].f += frc[0] + Fd0;
      M[p1].f += frc[1] + Fd1;
      M[p2].f += frc[2] + Fd2;
      M[p3].f += frc[3] + Fd3;
      M[p4].f += frc[4] + Fd4;
      M[p5].f += frc[5] + Fd5;
      M[p6].f += frc[6] + Fd6;
      M[p7].f += frc[7] + Fd7;
#else
      M[p0].f += frc[0] - ( ks*(D0 - D00) ) * (d0/D0);
      M[p1].f += frc[1] - ( ks*(D1 - D01) ) * (d1/D1);
      M[p2].f += frc[2] - ( ks*(D2 - D02) ) * (d2/D2);
      M[p3].f += frc[3] - ( ks*(D3 - D03) ) * (d3/D3);
      M[p4].f += frc[4] - ( ks*(D4 - D04) ) * (d4/D4);
      M[p5].f += frc[5] - ( ks*(D5 - D05) ) * (d5/D5);
      M[p6].f += frc[6] - ( ks*(D6 - D06) ) * (d6/D6);
      M[p7].f += frc[7] - ( ks*(D7 - D07) ) * (d7/D7);
#endif // DAMPED
#else
      Real_t D = D0 + D1 + D2 + D3 + D4 + D5 + D6 + D7;
      
      Real_t K = - ( ks*(D - L0) );
      
      M[p0].f += frc[0] + K * (d0/D0);
      M[p1].f += frc[1] + K * (d1/D1);
      M[p2].f += frc[2] + K * (d2/D2);
      M[p3].f += frc[3] + K * (d3/D3);
      M[p4].f += frc[4] + K * (d4/D4);
      M[p5].f += frc[5] + K * (d5/D5);
      M[p6].f += frc[6] + K * (d6/D6);
      M[p7].f += frc[7] + K * (d7/D7);
#endif // ALTERN
#else
      M[p0].f += frc[0];
      M[p1].f += frc[1];
      M[p2].f += frc[2];
      M[p3].f += frc[3];
      M[p4].f += frc[4];
      M[p5].f += frc[5];
      M[p6].f += frc[6];
      M[p7].f += frc[7];
#endif // CONSTVOL
    }
};

#endif // SCHEME_H
