#ifndef RANDOM_UNIFORM_H
#define RANDOM_UNIFORM_H

#include <vector>

#include "../../LIBS/RandomLib/Random.hpp"

#include "ATbase.h"

#include "independent.h"


/**
 * \ingroup initial_distr_grp
 *
 * \class RandomUniform
 * \brief Initialize particle attribute randomly within the interval
 *
 * Initialize particle attribute randomly within the interval [a,b]
 * 
 * configuration file example
 \verbatim

   Group "RandomUniform" {		
      a=0.2;
      b=0.8;
   }   

 \endverbatim
 * 
 *
 */
template<class A, class A1=A>
class RandomUniform: public Independent<A,A1>
{
  typedef typename A::Element_t Element_t;

public:

  //! accepts particle attribute attr and initialize it
  void operator() (A& attr)
  {
    for ( int i=0; i<attr.size(); i++)
      attr(i) = Element_t(mA + (mB-mA) * mRandom.FixedN()) ;
  }


  //! setups distribution by reading config file 
  void SetupFromConfigGroup(FileInput& in)
  {
    in.ChangeGroup("RandomUniform");

    mA = in.get_param("a");
    mB = in.get_param("b");

    in.ChangeGroup("..");    
  };


private:
  
  RandomLib::Random mRandom;     
  
  double mA;
  double mB;
};

#endif



