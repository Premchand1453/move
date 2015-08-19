#ifndef ANIMAL_INTEGRATION_SOLVER_H
#define ANIMAL_INTEGRATION_SOLVER_H

#include <animal/traits/numerics.h>



namespace animal { namespace integration {

// ----------------------------------
//  
//  Solver class.
/** Generic solver class.
    
    Declaration/Definition file:
    animal/integration/solver.h
    (creation date: March 28, 2000).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 2000 */
//  
// ----------------------------------

template <
  class TraitsT,
  class DerivativeF,
  class StepF >

struct Solver_Function
{
  typedef typename TraitsT::Real_t       Real_t;
  typedef typename TraitsT::Model_t      Model_t;
  typedef typename TraitsT::State_t      State_t;
  typedef typename TraitsT::Derivative_t Derivative_t;
  
  DerivativeF writeDerivative;
  StepF       applyStep;
  
  Solver_Function()
    : writeDerivative(), applyStep()
    {}
  Solver_Function(const DerivativeF& df, const StepF& sf)
    : writeDerivative(df), applyStep(sf)
    {}
  
  /** @name Call operator */
  //@{
  /** Give functions defined for S (S += h*D)
      or initial_S and final_S (final_S = initial_S + h*D)
      (where D is the container of derivatives of state variables).
  */
  void operator()(const Model_t& M,
		  State_t& S,
		  const Real_t t, const Real_t h);
  void operator()(const Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Real_t t, const Real_t h);
  
  /** This declaration is used by stepper algorithms: it avoids computing
      derivative at the starting point, but requires initial_D value.
  */
  void operator()(const Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Derivative_t& initial_D,
		  const Real_t t, const Real_t h);
  //@}
};



/** Generic traits class
 */
template <
  class RealT,
  class StateT,
  class DerivativeT,
  class ModelT,
  class NumTraitsT = animal::traits::Numerics<RealT> >

struct Solver_Traits
{
  /** @name Traits typedefs */
  //@{
  /// Time representation
  typedef RealT Real_t;
  
  /// Model description (physical data)
  typedef ModelT Model_t;
  
  /** Container of state variables.
      
      State_t must define State_t::value_type and State_t::size_type
      and have the following members:
      - a default constructor and a constructor specifying the size of
      the container ;
      - an iterator type and STL-like begin() and end() functions ;
      - a STL-like size() function.
      
      State_t::value_type must have the following members:
      - a default constructor, a simple constructor
      and a copy constructor ;
      - the =, +, * operators.
  */
  typedef StateT State_t;
  
  /** Container of derivatives of state variables.
      
      Derivative_t must define State_t::value_type and State_t::size_type
      and have the following members:
      - a default constructor and a constructor specifying the size of
      the container ;
      - an iterator type and STL-like begin() and end() functions ;
      - a STL-like size() function.
      
      Derivative_t::value_type must have the following members:
      - a default constructor, a simple constructor
      and a copy constructor ;
      - the =, +, * operators.
  */
  typedef DerivativeT Derivative_t;
  
  /// Numerical traits
  typedef  NumTraitsT Numerics_t;
  //@}
};



/** Define derivative D of state S
 */
template <class TraitsT>

struct Derivative_Function
{
  typedef typename TraitsT::Real_t       Real_t;
  typedef typename TraitsT::Model_t      Model_t;
  typedef typename TraitsT::State_t      State_t;
  typedef typename TraitsT::Derivative_t Derivative_t;
  typedef typename TraitsT::Numerics_t   Numerics_t;
  
  /// Call operator
  void operator()(const Model_t& M,
		  const State_t& S,
		  Derivative_t& D,
		  const Real_t t) const;
};



/** Move forward state S of length h in direction D
 */
template <class TraitsT>

struct Step_Function
{
  typedef typename TraitsT::Real_t       Real_t;
  typedef typename TraitsT::State_t      State_t;
  typedef typename TraitsT::Derivative_t Derivative_t;
  typedef typename TraitsT::Numerics_t   Numerics_t;
  
  /// Call operator
  void operator()(const State_t& initial_S,
		  State_t& final_S,
		  const Derivative_t& D,
		  const Real_t h) const;
};

} } // namespace animal { namespace integration {



#endif // ANIMAL_INTEGRATION_SOLVER_H
