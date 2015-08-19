#ifndef ANIMAL_INTEGRATION_DRIVER_H
#define ANIMAL_INTEGRATION_DRIVER_H



namespace animal { namespace integration {

template <class TimeF>

struct Driver_Function
{
  typedef typename TimeF::Real_t       Real_t;
  typedef typename TimeF::Model_t      Model_t;
  typedef typename TimeF::State_t      State_t;
  typedef typename TimeF::Derivative_t Derivative_t;
  
  TimeF compute;
  
  Driver_Function() : compute()
    {}
  Driver_Function(const TimeF& tf) : compute(tf)
    {}
};

} } // namespace animal { namespace integration {



#endif // ANIMAL_INTEGRATION_DRIVER_H
