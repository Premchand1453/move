#ifndef ANIMAL_INTEGRATION_TEST_EXPLICIT_SOLVER_TEST_H
#define ANIMAL_INTEGRATION_TEST_EXPLICIT_SOLVER_TEST_H

#include <vector>
#include <animal/integration/explicit_solver.h>
#include <animal/integration/test/particle_system.h>



/** This example deals with a simple oscillator consisting
    of one particle and a spring in 1D.
    Gravity, damping, and friction effects are not taken
    into account.
*/

/** Traits
 */
struct Particle_System_Traits :
  public animal::integration::
    Solver_Traits< double,
                   std::vector<Particle_State>,
		   std::vector<Particle_State_Derivative>,
		   std::vector<Particle_System_Model> >
{};

/** Simple derivative determination
 */
struct Simple_Derivative :
  public animal::integration::Derivative_Function<Particle_System_Traits>
{
  void operator()(const Model_t& M,
		  const State_t& S,
		  Derivative_t& D,
		  const Real_t t) const
    {
      Real_t f, a;
      
      Model_t::const_iterator first_M = M.begin();
      State_t::const_iterator first_S = S.begin();
      State_t::const_iterator last_S  = S.end();
      Derivative_t::iterator  first_D = D.begin();
      
      for ( ;
	    first_S != last_S;
	    ++first_M, ++first_S, ++first_D
	  )
	{
	  f = - (*first_M).k * ( (*first_S).pos - (*first_M).L0 );
	  // force = - stiffness * ( actual length - rest length)
	  
	  a = f/(*first_M).m;
	  // Newtonian dynamics
	  
	  (*first_D).acc = a;
	  (*first_D).vel = (*first_S).vel;
	}
    }
};

/** Simple Euler step
 */
struct Simple_Step :
  public animal::integration::Step_Function<Particle_System_Traits>
{
  void operator()(const State_t& initial_S,
		  State_t& final_S,
		  const Derivative_t& D,
		  const Real_t h) const
    {
      State_t::const_iterator      first_iS = initial_S.begin();
      State_t::const_iterator      last_iS  = initial_S.end();
      State_t::iterator            first_fS = final_S.begin();
      Derivative_t::const_iterator first_D  = D.begin();
      
      for ( ;
	    first_iS != last_iS;
	    ++first_iS, ++first_fS, ++first_D
	  )
	{
	  // Apply element step
	  (*first_fS).vel = (*first_iS).vel + h*(*first_D).acc;
	  (*first_fS).pos = (*first_iS).pos + h*(*first_D).vel;
	}
    }
};

/** Modified Euler step.
    It gives a second-order method (with a constant acceleration
    hypothesis) for a first-order method cost.
*/
struct Modified_Step :
  public animal::integration::Step_Function<Particle_System_Traits>
{
  void operator()(const State_t& initial_S,
		  State_t& final_S,
		  const Derivative_t& D,
		  const Real_t h) const
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
	  // Apply element step
	  (*first_fS).vel = (*first_iS).vel + h*(*first_D).acc;
	  (*first_fS).pos = (*first_iS).pos + h*(*first_D).vel + 0.5*sqh*(*first_D).acc;
	  // Second-order!
	}
    }
};

/** Stoermer integration method.
    It's an integration method for second-order systems.
*/
struct Stoermer_Derivative :
  public animal::integration::Derivative_Function<Particle_System_Traits>
{
  void operator()(const Model_t& M,
		  const State_t& S,
		  Derivative_t& D,
		  const Real_t t) const
    {
      Real_t f, a;
      
      Model_t::const_iterator first_M = M.begin();
      State_t::const_iterator first_S = S.begin();
      State_t::const_iterator last_S  = S.end();
      Derivative_t::iterator  first_D = D.begin();
      
      for ( ;
	    first_S != last_S;
	    ++first_M, ++first_S, ++first_D
	  )
	{
	  f = - (*first_M).k * ( (*first_S).pos - (*first_M).L0 );
	  // force = - stiffness * ( actual length - rest length)
	  
	  a = f/(*first_M).m;
	  // Newtonian dynamics
	  
	  (*first_D).acc = a;
	  // No need to update velocity!
	}
    }
};
struct Stoermer_Step :
  public animal::integration::Step_Function<Particle_System_Traits>
{
  void operator()(const State_t& initial_S,
		  State_t& final_S,
		  const Derivative_t& D,
		  const Real_t h) const
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
	  // Apply element step
	  (*first_fS).vel = (*first_iS).vel + sqh*(*first_D).acc;
	  (*first_fS).pos = (*first_iS).pos + (*first_fS).vel;
	}
    }
};

/** Modified midpoint integration method.
 */
struct Modified_Midpoint_Step :
  public animal::integration::Step_Function<Particle_System_Traits>
{
  /** @name Call operator */
  //@{
  /// Simple step
  void operator()(const State_t& initial_S,
		  State_t& final_S,
		  const Derivative_t& D,
		  const Real_t h) const
    {
      State_t::const_iterator      first_iS = initial_S.begin();
      State_t::const_iterator      last_iS  = initial_S.end();
      State_t::iterator            first_fS = final_S.begin();
      Derivative_t::const_iterator first_D  = D.begin();
      
      for ( ;
	    first_iS != last_iS;
	    ++first_iS, ++first_fS, ++first_D
	  )
	{
	  // Apply element step
	  (*first_fS).vel = (*first_iS).vel + h*(*first_D).acc;
	  (*first_fS).pos = (*first_iS).pos + h*(*first_D).vel;
	}
    }
  /// Last step
  void operator()(const State_t& initial_S2, const State_t& initial_S3,
		  State_t& final_S,
		  const Derivative_t& D,
		  const Real_t h) const
    {
      State_t::const_iterator      first_iS2 = initial_S2.begin();
      State_t::const_iterator       last_iS2 = initial_S2.end();
      State_t::const_iterator      first_iS3 = initial_S3.begin();
      State_t::iterator            first_fS  =    final_S.begin();
      Derivative_t::const_iterator first_D   =          D.begin();
      
      for ( ;
	    first_iS2 != last_iS2;
	    ++first_iS2, ++first_iS3, ++first_fS, ++first_D
	  )
	{
	  // Apply element step
	  (*first_fS).vel =
	    0.5*( (*first_iS2).vel + (*first_iS3).vel + h*(*first_D).acc );
	  (*first_fS).pos =
	    0.5*( (*first_iS2).pos + (*first_iS3).pos + h*(*first_D).vel );
	}
    }
  //@}
};
struct Constant_NStep
  : public animal::integration::NStep_Function<Particle_System_Traits>
{
  /** Call operator.
      The number of substeps is constant.
  */
  int operator()(const Model_t& M,
		 const State_t& S,
		 const Real_t t) const
    {
      return 3; /* Corresponds to 4 different evaluations
		   of the derivative (as in RK4) */
    }
};



#endif // ANIMAL_INTEGRATION_TEST_EXPLICIT_SOLVER_TEST_H
