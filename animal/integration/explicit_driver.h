#ifndef ANIMAL_INTEGRATION_EXPLICIT_DRIVER_H
#define ANIMAL_INTEGRATION_EXPLICIT_DRIVER_H

#include <animal/integration/driver.h>



namespace animal { namespace integration {

template <class SolverF>

struct Solver_Driver : public Driver_Function<SolverF>
{
  typedef Driver_Function<SolverF>                 Driver_Function_t;
  typedef typename Driver_Function_t::Real_t       Real_t;
  typedef typename Driver_Function_t::Model_t      Model_t;
  typedef typename Driver_Function_t::State_t      State_t;
  typedef typename Driver_Function_t::Derivative_t Derivative_t;
  
  /// Current date
  Real_t date;
  
  /// Stepsize
  Real_t time_step;
  
  Solver_Driver()
    {}
  Solver_Driver(const SolverF& sf, const Real_t t, const Real_t h)
  : Driver_Function<SolverF>(sf),
    date(t), time_step(h)
    {}
  
  void operator()(Model_t& M, State_t& S)
    {
      operator()(M, S, S);
    }
  void operator()(Model_t& M,
		  const State_t& initial_S,
                  State_t& final_S)
    {
      this->compute(M, initial_S, final_S, date, time_step);
      date += time_step;
    }
};

template <class StepperF>

struct Stepper_Driver : public Driver_Function<StepperF>
{
  typedef Driver_Function<StepperF>                Driver_Function_t;
  typedef typename Driver_Function_t::Real_t       Real_t;
  typedef typename Driver_Function_t::Model_t      Model_t;
  typedef typename Driver_Function_t::State_t      State_t;
  typedef typename Driver_Function_t::Derivative_t Derivative_t;
  
  /// Current date
  Real_t date;
  
  /// Stepsize to be attempted (initial trial value)
  Real_t step_try;
  
  /// Stepsize that was actually accomplished
  Real_t step_did;
  
  /// Estimated next stepsize
  Real_t step_next;
  
  Stepper_Driver()
    {}
  Stepper_Driver(const StepperF& sf,
		 const Real_t t, const Real_t htry)
  : Driver_Function<StepperF>(sf),
    date(t), step_try(htry), step_did(0.0), step_next(0.0)
    {}
  
  void operator()(Model_t& M,
		  State_t& S)
    {
      operator()(M, S, S);
    }
  void operator()(Model_t& M,
		  const State_t& initial_S,
                  State_t& final_S)
    {
      this->compute(M, initial_S, final_S, date, step_try, step_did, step_next);
      date += step_did;
      step_try = step_next;
    }
};

/** Loop examples using drivers
    
    // Finite loop: solver
    int niter = 1000;
    for (int n = 0; n < niter; ++n)
      {
	driveSolver();
	doSomethingElse();
      }
    
    // Finite loop: stepper
    while ( drive.date < date_end )
      {
	driveStepper();
	doSomethingElse();
      }
    
    // Infinite loop
    run = true;
    keep_on = true;
    while ( run )
      {
	drive();
	doSomethingElse();
	
	run = keep_on; // keep_on = false; // Stop!
      }
*/

} } // namespace animal { namespace integration {



#endif // ANIMAL_INTEGRATION_EXPLICIT_DRIVER_H
