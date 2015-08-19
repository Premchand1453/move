#ifndef ANIMAL_INTEGRATION_TEST_EXPLICIT_STEPPER_TEST_H
#define ANIMAL_INTEGRATION_TEST_EXPLICIT_STEPPER_TEST_H

#include <animal/integration/explicit_stepper.h>
#include <animal/integration/test/explicit_solver_test.h>



struct Simply_Smaller :
  public animal::integration::HSmaller_Function<Particle_System_Traits>
{
  Real_t hmin()
    {
      return 1.0e-6;
    }
  
  Real_t operator()(const Model_t& M,
		    const State_t& S,
		    const Real_t t, const Real_t h)
    {
      Real_t hnew = 0.5*h; // Divide by two
      
      Real_t hlim = hmin();
      
      if ( hnew < hlim )
	return hlim;
      else
	return hnew;
    }
};

struct Simply_Larger :
  public animal::integration::HLarger_Function<Particle_System_Traits>
{
  Real_t hmax()
    {
      return 1.0; // Minimum required frame rate: 25 Hz
    }
  
  Real_t operator()(const Model_t& M,
		    const State_t& S,
		    const Real_t t, const Real_t h)
    {
      Real_t hnew = 1.259921*h; // Multiply by the cubic root of two
      
      Real_t hlim = hmax();
      
      if ( hnew > hlim )
	return hlim;
      else
	return hnew;
    }
};

struct Different_Accuracy :
  public animal::integration::IsDifferent_Function<Particle_System_Traits>
{
  Real_t errormax()
    {
      return 1.0e-3;
    }
  
  bool operator()(const Model_t& M,
		  const State_t& S1, const State_t& S2,
		  const Real_t t)
    {
      Real_t v1, v2, p1, p2, delta;
      
      State_t::const_iterator first_S1 = S1.begin();
      State_t::const_iterator first_S2 = S2.begin();
      State_t::const_iterator last_S2  = S2.end();
      
      for ( ;
	    first_S2 != last_S2;
	    ++first_S1, ++first_S2
	  )
	{
	  v1 = (*first_S1).vel;
	  v2 = (*first_S2).vel;
	  
	  p1 = (*first_S1).pos;
	  p2 = (*first_S2).pos;
	  
	  delta = Numerics_t::fpabs( 0.5*( p1 - p2 + v1 - v2) );
	  // Criterion: mean numerical error on positions and velocities
	  
          if ( delta > errormax() )
	    return true;
	}
      
      return false;
    }
};

struct Different_Energy :
  public animal::integration::IsDifferent_Function<Particle_System_Traits>
{
  Real_t errormax()
    {
      return 1.0e-3;
    }
  
  bool operator()(const Model_t& M,
		  const State_t& S1, const State_t& S2,
		  const Real_t t)
    {
      Real_t v1, v2, x1, x2, delta;
      
      Model_t::const_iterator first_M  =  M.begin();
      State_t::const_iterator first_S1 = S1.begin();
      State_t::const_iterator first_S2 = S2.begin();
      State_t::const_iterator last_S2  = S2.end();
      
      for ( ;
	    first_S2 != last_S2;
	    ++first_M, ++first_S1, ++first_S2
	  )
	{
	  v1 = (*first_S1).vel;
	  v2 = (*first_S2).vel;
	  
	  x1 = (*first_S1).pos - (*first_M).L0;
	  x2 = (*first_S2).pos - (*first_M).L0;
	  
	  delta =
	    Numerics_t::fpabs( 0.5*( (*first_M).m*(v1*v1 - v2*v2)
				   + (*first_M).k*(x1*x1 - x2*x2) ) );
	  // Criterion: variation of energy E
	  // where E = KE + PE = 0.5*m*v*v + 0.5*k*x*x
	  
          if ( delta > errormax() )
	    return true;
	}
      
      return false;
    }
};



#endif // ANIMAL_INTEGRATION_TEST_EXPLICIT_STEPPER_TEST_H
