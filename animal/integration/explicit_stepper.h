#ifndef ANIMAL_INTEGRATION_EXPLICIT_STEPPER_H
#define ANIMAL_INTEGRATION_EXPLICIT_STEPPER_H

#include <animal/integration/stepper.h>



namespace animal { namespace integration {

// -------------------------------------------------------------
//  
//  Step_Doubling class.
/** Adaptive stepsize control for explicit integration methods.
    
    This straightforward technique take each step twice, once
    as a full step, and, independently, as two half steps.
    
    The difference between the two final state estimates is a
    convenient indicator of error.
    It is this difference that we shall endeavor to keep to
    a desired degree of accuracy, neither too large nor too
    small. We do this by adjusting time step value.
    
    Declaration/Definition file: animal/integration/stepper.h
    (creation date: March 28, 2000).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 2000 */
//  
// -------------------------------------------------------------

template <
  class SolverF,
  class HSmallerF,
  class HLargerF,
  class IsDifferentF >

struct Step_Doubling
  : public Stepper_Function<SolverF,HSmallerF,HLargerF,IsDifferentF>
{
  typedef Stepper_Function<SolverF,HSmallerF,HLargerF,IsDifferentF> Stepper_Function_t;
  typedef typename Stepper_Function_t::Real_t       Real_t;
  typedef typename Stepper_Function_t::Model_t      Model_t;
  typedef typename Stepper_Function_t::State_t      State_t;
  typedef typename Stepper_Function_t::Derivative_t Derivative_t;
  
  /// Initial state copy and final state full step evaluation
  State_t initial_S_copy, final_S_eval;
  
  /// Derivative first evaluation
  Derivative_t initial_D;
  
  /** Number of good and bad (but retried and fixed) steps taken
      (counted only in -DDEBUG mode) */
  long int ngood, nbad;
  
  Step_Doubling()
    {}
  Step_Doubling(const SolverF& slf,
		const HSmallerF& hsf, const HLargerF& hlf,
		const IsDifferentF& isdf)
  : Stepper_Function<SolverF,HSmallerF,HLargerF,IsDifferentF>(slf, hsf, hlf, isdf),
    initial_S_copy( slf.curr_size ),
    final_S_eval( slf.curr_size ),
    initial_D( slf.curr_size ),
    ngood(0), nbad(0)
    {}
  
  /// To take dynamic modifications of the model into account
  void resize(const typename Derivative_t::size_type size)
    {
      this->solve.resize(size);
      
      Derivative_t initial_S_copy(size);
      Derivative_t final_S_eval(size);
      Derivative_t initial_D(size);
    }
  
  /** @name Call operator */
  //@{
  void operator()(const Model_t& M,
		  State_t& S,
		  const Real_t t,
		  const Real_t htry, Real_t& hdid, Real_t& hnext)
    {
      initial_S_copy = S;
      operator()(M, initial_S_copy, S, t, htry, hdid, hnext);
    }
  void operator()(const Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Real_t t,
		  const Real_t htry, Real_t& hdid, Real_t& hnext)
    {
      bool divide_step = true;
      
      Real_t h = htry; // Stepsize to be attempted (initial trial value)
      Real_t h_2;
      
      // Derivative first evaluation
      this->solve.writeDerivative(M, initial_S, initial_D, t);
      
      while ( divide_step )
	{
	  hdid = h; // Stepsize that was actually accomplished
	  h_2 = 0.5*h;
	  
	  // Take a full step
	  this->solve(M, initial_S, final_S_eval, initial_D, t, h);
	  
          // Take two half steps
          this->solve(M, initial_S, final_S, initial_D, t, h_2);
          this->solve(M, final_S, (t + h_2), h_2);
          
          if ( this->isDifferent(M, final_S_eval, final_S, t) )
	    {
	      h = this->hsmaller(M, initial_S, t, h);
#if DEBUG
	      nbad++;
#endif
	    }
	  else
	    {
	      divide_step = false;
	      h = this->hlarger(M, initial_S, t, h);
#if DEBUG
	      ngood++;
#endif
	    }
	}
      
      hnext = h; // Estimated next stepsize
    }
  //@}
  
}; // struct Step_Doubling



// -------------------------------------------------------------
//  
//  Step_Back_And_Forth class.
/** Adaptive stepsize control for explicit integration methods.
    
    This straightforward technique take each step once.
    The difference, according to some criterion, between the
    current state estimate and the previous state estimate,
    one step before in time, is kept to a desired degree of
    accuracy. This is done by adjusting time step value.
    
    Declaration/Definition file: animal/integration/stepper.h
    (creation date: March 28, 2000).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 2000 */
//  
// -------------------------------------------------------------

template <
  class SolverF,
  class HSmallerF,
  class HLargerF,
  class IsDifferentF >

struct Step_Back_And_Forth
  : public Step_Doubling<SolverF,HSmallerF,HLargerF,IsDifferentF>
{
  typedef Step_Doubling<SolverF,HSmallerF,HLargerF,IsDifferentF> Step_Doubling_t;
  typedef typename Step_Doubling_t::Real_t       Real_t;
  typedef typename Step_Doubling_t::Model_t      Model_t;
  typedef typename Step_Doubling_t::State_t      State_t;
  typedef typename Step_Doubling_t::Derivative_t Derivative_t;
  
  Step_Back_And_Forth(const SolverF& slf,
		      const HSmallerF& hsf, const HLargerF& hlf,
		      const IsDifferentF& isdf)
  : Step_Doubling<SolverF,HSmallerF,HLargerF,IsDifferentF>(slf, hsf, hlf, isdf)
    {}
  
  /** @name Call operator */
  //@{
  void operator()(const Model_t& M,
		  State_t& S,
		  const Real_t t,
		  const Real_t htry, Real_t& hdid, Real_t& hnext)
    {
      this->initial_S_copy = S;
      operator()(M, this->initial_S_copy, S, t, htry, hdid, hnext);
    }
  void operator()(const Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Real_t t,
		  const Real_t htry, Real_t& hdid, Real_t& hnext)
    {
      bool divide_step = true;
      
      Real_t h = htry; // Stepsize to be attempted (initial trial value)
      
      // Derivative first evaluation
      this->solve.writeDerivative(M, initial_S, this->initial_D, t);
      
      while ( divide_step )
	{
	  hdid = h; // Stepsize that was actually accomplished
          
          // Take a step
          this->solve(M, initial_S, final_S, this->initial_D, t, h);
          
          if ( this->isDifferent(M, initial_S, final_S, t) )
	    {
	      h = this->hsmaller(M, initial_S, t, h);
#if DEBUG
	      nbad++;
#endif
	    }
	  else
	    {
	      divide_step = false;
	      h = this->hlarger(M, initial_S, t, h);
#if DEBUG
	      ngood++;
#endif
	    }
	}
      
      hnext = h; // Estimated next stepsize
    }
  //@}
  
}; // struct Step_Back_And_Forth

} } // namespace animal { namespace integration {



#endif // ANIMAL_INTEGRATION_EXPLICIT_STEPPER_H
