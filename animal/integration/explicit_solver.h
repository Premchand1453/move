#ifndef ANIMAL_INTEGRATION_EXPLICIT_SOLVER_H
#define ANIMAL_INTEGRATION_EXPLICIT_SOLVER_H

#include <animal/integration/solver.h>



namespace animal { namespace integration {

// ------------------------------------------------------------
//  
//  Euler class.
/** Explicit Euler integration method
    (aka explicit first-order Runge-Kutta integration method).
    
    Declaration/Definition file:
    animal/integration/explicit_solver.h
    (creation date: March 28, 2000).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 2000 */
//  
// ------------------------------------------------------------

template <
  class TraitsT,
  class DerivativeF,
  class StepF >

struct Euler : public Solver_Function<TraitsT,DerivativeF,StepF>
{
  typedef Solver_Function<TraitsT,DerivativeF,StepF> Solver_Function_t;
  typedef typename Solver_Function_t::Real_t         Real_t;
  typedef typename Solver_Function_t::Model_t        Model_t;
  typedef typename Solver_Function_t::State_t        State_t;
  typedef typename Solver_Function_t::Derivative_t   Derivative_t;
  
  /// Derivative evaluation
  Derivative_t D;
  
  typename Derivative_t::size_type curr_size;
  
  Euler()
    {}
  Euler(const State_t& s, const DerivativeF& df, const StepF& sf)
  : Solver_Function<TraitsT,DerivativeF,StepF>(df,sf),
    D( s.size() ),
    curr_size( s.size() )
    {}
  
  /// To take dynamic modifications of the model into account
  void resize(const typename Derivative_t::size_type size)
    {
      Derivative_t D(size);
    }
  
  /** @name Call operator */
  //@{
  void operator()(Model_t& M,
		  State_t& S,
		  const Real_t t, const Real_t h)
    {
      operator()(M, S, S, t, h);
    }
  void operator()(Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Real_t t, const Real_t h)
    {
      writeDerivative(M, initial_S, D, t);
      operator()(M, initial_S, final_S, D, t, h);
    }
  void operator()(Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Derivative_t& initial_D,
		  const Real_t t, const Real_t h)
    {
      applyStep(initial_S, final_S, initial_D, h);
    }
  //@}
  
}; // struct Euler



// ------------------------------------------------------
//  
//  Runge_Kutta_2 class.
/** Explicit second-order Runge-Kutta integration method
    (aka midpoint integration method).
    
    Declaration/Definition file:
    animal/integration/explicit_solver.h
    (creation date: March 28, 2000).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 2000 */
//  
// ------------------------------------------------------

template <
  class TraitsT,
  class DerivativeF,
  class StepF >

struct Runge_Kutta_2 : public Solver_Function<TraitsT,DerivativeF,StepF>
{
  typedef Solver_Function<TraitsT,DerivativeF,StepF> Solver_Function_t;
  typedef typename Solver_Function_t::Real_t         Real_t;
  typedef typename Solver_Function_t::Model_t        Model_t;
  typedef typename Solver_Function_t::State_t        State_t;
  typedef typename Solver_Function_t::Derivative_t   Derivative_t;
  
  /// Temporary state value
  State_t tmp_S;
  
  /// Derivative first and second evaluations
  Derivative_t D1, D2;
  
  typename Derivative_t::size_type curr_size;
  
  Runge_Kutta_2()
    {}
  Runge_Kutta_2(const State_t& s, const DerivativeF& df, const StepF& sf)
  : Solver_Function<TraitsT,DerivativeF,StepF>(df,sf),
    tmp_S( s.size() ),
    D1( s.size() ), D2( s.size() ),
    curr_size( s.size() )
    {}
  
  /// To take dynamic modifications of the model into account
  void resize(const typename Derivative_t::size_type size)
    {
      State_t      tmp_S(size);
      Derivative_t D1   (size);
      Derivative_t D2   (size);
    }
  
  /** @name Call operator */
  //@{
  void operator()(const Model_t& M,
		  State_t& S,
		  const Real_t t, const Real_t h)
    {
      operator()(M, S, S, t, h);
    }
  void operator()(const Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Real_t t, const Real_t h)
    {
      writeDerivative(M, initial_S, D1, t);
      operator()(M, initial_S, final_S, D1, t, h);
    }
  void operator()(const Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Derivative_t& initial_D,
		  const Real_t t, const Real_t h)
    {
      Real_t h_2 = 0.5*h;
      
      applyStep(initial_S, tmp_S, initial_D, h_2);
      
      writeDerivative( M, tmp_S, D2, (t + h_2) );
      applyStep(initial_S, final_S, D2, h);
    }
  //@}
  
}; // struct Runge_Kutta_2



// -------------------------------------------------------
//  
//  Runge_Kutta_4 class.
/** Explicit fourth-order Runge-Kutta integration method.
    
    Declaration/Definition file:
    animal/integration/explicit_solver.h
    (creation date: March 28, 2000).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 2000 */
//  
// -------------------------------------------------------

template <
  class TraitsT,
  class DerivativeF,
  class StepF >

struct Runge_Kutta_4 : public Solver_Function<TraitsT,DerivativeF,StepF>
{
  typedef Solver_Function<TraitsT,DerivativeF,StepF> Solver_Function_t;
  typedef typename Solver_Function_t::Real_t         Real_t;
  typedef typename Solver_Function_t::Model_t        Model_t;
  typedef typename Solver_Function_t::State_t        State_t;
  typedef typename Solver_Function_t::Derivative_t   Derivative_t;
  
  /// Temporary state value
  State_t tmp_S;
  
  /// Derivative first, second, third and fourth evaluations
  Derivative_t D1, D2, D3, D4;
  
  /// Final derivative value
  Derivative_t final_D;
  
  typename Derivative_t::size_type curr_size;
  
  Runge_Kutta_4()
    {}
  Runge_Kutta_4(const State_t& s, const DerivativeF& df, const StepF& sf)
  : Solver_Function<TraitsT,DerivativeF,StepF>(df,sf),
    tmp_S( s.size() ),
    D1( s.size() ), D2( s.size() ), D3( s.size() ), D4( s.size() ),
    final_D( s.size() ),
    curr_size( s.size() )
    {}
  
  /// To take dynamic modifications of the model into account
  void resize(const typename Derivative_t::size_type size)
    {
      State_t      tmp_S  (size);
      Derivative_t D1     (size);
      Derivative_t D2     (size);
      Derivative_t D3     (size);
      Derivative_t D4     (size);
      Derivative_t final_D(size);
    }
  
  /** @name Call operator */
  //@{
  void operator()(const Model_t& M,
		  State_t& S,
		  const Real_t t, const Real_t h)
    {
      operator()(M, S, S, t, h);
    }
  void operator()(const Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Real_t t, const Real_t h)
    {
      writeDerivative(M, initial_S, D1, t);
      operator()(M, initial_S, final_S, D1, t, h);
    }
  void operator()(const Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Derivative_t& initial_D,
		  const Real_t t, const Real_t h)
    {
      Real_t h_2 = 0.5*h;
      
      applyStep(initial_S, tmp_S, initial_D, h_2);
      
      writeDerivative ( M, tmp_S, D2, (t + h_2) );
      applyStep(initial_S, tmp_S, D2, h_2);
      
      writeDerivative ( M, tmp_S, D3, (t + h_2) );
      applyStep(initial_S, tmp_S, D3, h);
      
      writeDerivative ( M, tmp_S, D4, (t + h) );
      
      typename Derivative_t::iterator       first_fD =   final_D.begin();
      typename Derivative_t::iterator        last_fD =   final_D.end();
      typename Derivative_t::const_iterator first_iD = initial_D.begin();
      typename Derivative_t::const_iterator first_D2 =        D2.begin();
      typename Derivative_t::const_iterator first_D3 =        D3.begin();
      typename Derivative_t::const_iterator first_D4 =        D4.begin();
      
      for ( ;
	    first_fD != last_fD;
	    ++first_fD, ++first_iD, ++first_D2, ++first_D3, ++first_D4
	  )
	{
	  (*first_fD) = (*first_iD) + ( (*first_D2) + (*first_D3) )*2.0 + (*first_D4);
	}
      
      Real_t h_6 = h/6.0;
      
      applyStep(initial_S, final_S, final_D, h_6);
    }
  //@}
  
}; // struct Runge_Kutta_4



// --------------------------------------
//  
//  Modified_Midpoint class.
/** Modified midpoint method.
    
    Declaration/Definition file:
    animal/integration/explicit_solver.h
    (creation date: March 28, 2000).
    
    @author David Bourguignon
    
    iMAGIS/GRAVIR, 2000 */
//  
// --------------------------------------

template <
  class TraitsT,
  class DerivativeF,
  class StepF,
  class NStepF >

struct Modified_Midpoint : public Solver_Function<TraitsT,DerivativeF,StepF>
{
  typedef Solver_Function<TraitsT,DerivativeF,StepF> Solver_Function_t;
  typedef typename Solver_Function_t::Real_t         Real_t;
  typedef typename Solver_Function_t::Model_t        Model_t;
  typedef typename Solver_Function_t::State_t        State_t;
  typedef typename Solver_Function_t::Derivative_t   Derivative_t;
  
  NStepF nstep;
  
  /// State (m-1)th, mth and (m+1)th evaluations
  State_t S1, S2, S3;
  
  /// Derivative evaluation
  Derivative_t D;
  
  typename Derivative_t::size_type curr_size;
  
  Modified_Midpoint()
    {}
  Modified_Midpoint(const State_t& s, const DerivativeF& df,
		    const StepF& sf, const NStepF& nsf)
  : Solver_Function<TraitsT,DerivativeF,StepF>(df,sf),
    nstep(nsf),
    S1( s.size() ), S2( s.size() ), S3( s.size() ),
    D( s.size() ),
    curr_size( s.size() )
    {}
  
  /// To take dynamic modifications of the model into account
  void resize(const typename Derivative_t::size_type size)
    {
      State_t      S1(size);
      State_t      S2(size);
      State_t      S3(size);
      Derivative_t D (size);
    }
  
  /** @name Call operator */
  //@{
  void operator()(const Model_t& M,
		  State_t& S,
		  const Real_t t, const Real_t h)
    {
      operator()(M, S, S, t, h);
    }
  void operator()(const Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Real_t t, const Real_t h)
    {
      writeDerivative(M, initial_S, D, t);
      operator()(M, initial_S, final_S, D, t, h);
    }
  void operator()(const Model_t& M,
		  const State_t& initial_S,
		  State_t& final_S,
		  const Derivative_t& initial_D,
		  const Real_t t, const Real_t h)
    {
      int N = nstep(M, initial_S, t);
      
      Real_t hsub = h/N;       // h: total step to be made
      Real_t hhsub = 2.0*hsub; // hsub: substep value
      
      S1 = initial_S;
      
      applyStep(S1, S2, initial_D, hsub); // First substep taken
      
      // Take remaining substeps minus one
      for (int m = 1; m < (N - 1); m++)
	{
	  writeDerivative( M, S2, D, (t + m*hsub) );
          applyStep(S1, S3, D, hhsub);
          S1 = S2;
          S2 = S3;
	}
      
      // Take last substep
      writeDerivative( M, S2, D, (t + (N - 1)*hsub) );
      applyStep(S1, S3, D, hhsub);
      
      // Take full step
      writeDerivative( M, S3, D, (t + h) );
      applyStep(S2, S3, final_S, D, hsub ); // Different call operator!
    }
  //@}
  
}; // struct Modified_Midpoint

/** Return the number of substeps to be used
 */
template <class TraitsT>

struct NStep_Function
{
  typedef typename TraitsT::Real_t     Real_t;
  typedef typename TraitsT::Model_t    Model_t;
  typedef typename TraitsT::State_t    State_t;
  typedef typename TraitsT::Numerics_t Numerics_t;
  
  /// Call operator
  int operator()(const Model_t& M,
		 const State_t& S,
		 const Real_t t) const;
};

} } // namespace animal { namespace integration {



#endif // ANIMAL_INTEGRATION_EXPLICIT_SOLVER_H
