#ifndef ANIMAL_INTEGRATION_STEPPER_H
#define ANIMAL_INTEGRATION_STEPPER_H

#include <animal/integration/solver.h>



namespace animal { namespace integration {

// ----------------------------------
//  
//  Stepper class.
/** Generic stepper class.
    
    Declaration/Definition file:
    animal/integration/stepper.h
    (creation date: March 28, 2000).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 2000 */
//  
// ----------------------------------

template <
  class SolverF,
  class HSmallerF,
  class HLargerF,
  class IsDifferentF >

struct Stepper_Function
{
  typedef typename SolverF::Real_t       Real_t;
  typedef typename SolverF::Model_t      Model_t;
  typedef typename SolverF::State_t      State_t;
  typedef typename SolverF::Derivative_t Derivative_t;
  
  SolverF      solve;
  HSmallerF    hsmaller;
  HLargerF     hlarger;
  IsDifferentF isDifferent;
  
  Stepper_Function()
    : solve(), hsmaller(), hlarger(), isDifferent()
    {}
  Stepper_Function(const SolverF& slf,
		   const HSmallerF& hsf, const HLargerF& hlf,
		   const IsDifferentF& isdf)
    : solve(slf), hsmaller(hsf), hlarger(hlf), isDifferent(isdf)
    {}
  
  /** @name Call operator */
  //@{
  /** Compute one stepper iteration, where
      htry is the stepsize to be attempted (initial trial value),
      hdid is the stepsize that was actually accomplished and
      hnext is the estimated next stepsize.
  */
  void operator()(const Model_t& M,
		  State_t& S,
		  const Real_t t,
		  const Real_t htry, Real_t& hdid, Real_t& hnext);
  void operator()(const Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Real_t t,
		  const Real_t htry, Real_t& hdid, Real_t& hnext);
  //@}
};



/** New smaller stepsize.
 */
template <class TraitsT>

struct HSmaller_Function
{
  typedef typename TraitsT::Real_t     Real_t;
  typedef typename TraitsT::Model_t    Model_t;
  typedef typename TraitsT::State_t    State_t;
  typedef typename TraitsT::Numerics_t Numerics_t;
  
  /// Minimum allowed stepsize
  Real_t hmin(const Model_t& M,
	      const State_t& S,
	      const Real_t t) const;
  
  /// Call operator
  Real_t operator()(const Model_t& M,
		    const State_t& S,
		    const Real_t t, const Real_t h) const;
};



/** New larger stepsize.
 */
template <class TraitsT>

struct HLarger_Function
{
  typedef typename TraitsT::Real_t     Real_t;
  typedef typename TraitsT::Model_t    Model_t;
  typedef typename TraitsT::State_t    State_t;
  typedef typename TraitsT::Numerics_t Numerics_t;
  
  /// Maximum allowed stepsize
  Real_t hmax(const Model_t& M,
	      const State_t& S,
	      const Real_t t) const;
  
  /// Call operator
  Real_t operator()(const Model_t& M,
		    const State_t& S,
		    const Real_t t, const Real_t h) const;
};



/** Criterion for modifying time step value
 */
template <class TraitsT>

struct IsDifferent_Function
{
  typedef typename TraitsT::Real_t       Real_t;
  typedef typename TraitsT::Model_t      Model_t;
  typedef typename TraitsT::State_t      State_t;
  typedef typename TraitsT::Derivative_t Derivative_t;
  typedef typename TraitsT::Numerics_t   Numerics_t;
  
  /// Required accuracy
  Real_t errormax(const Model_t& M,
		  const State_t& S,
		  const Real_t t) const;
  
  /** @name Call operator */
  //@{
  /// Compare two states according to a criterion
  bool operator()(const Model_t& M,
		  const State_t& S1, const State_t& S2,
		  const Real_t t) const;
  
  /// Compare one state to a constant
  bool operator()(const Model_t& M,
		  const State_t& S,
		  const Real_t t) const;
  //@}
};

} } // namespace animal { namespace integration {



#endif // ANIMAL_INTEGRATION_STEPPER_H
