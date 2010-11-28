#ifndef SINGLE_VALUE_H
#define SINGLE_VALUE_H

#include "independent.h"


/**
 * \ingroup initial_distr_grp
 *
 * \class SingleValue
 * 
 * \brief Initialize particle attribute with a single value
 *
 * configuration file example
 \verbatim

     Group "SingleValue" {		
        Value=0.1;
     }

 \endverbatim
 */
template<class A, class A1=A>
class SingleValue: public Independent<A,A1>
{
  typedef typename A::Element_t Element_t;

public:

  //! setups all aprticles
  void operator() (A& attr) {  attr = mValue; };

  //! setups distribution by reading config file 
  void SetupFromConfigGroup(FileInput& in)
  {  
    in.ChangeGroup("SingleValue");
    mValue = Element_t(in.get_param("Value"));
    in.ChangeGroup("..");
  }
  
private:
  
  Element_t mValue;
};

#endif
