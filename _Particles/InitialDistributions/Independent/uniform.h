#ifndef UNIFORM_H
#define UNIFORM_H

#include <vector>

#include "ATbase.h"

#include "independent.h"


/**
 * \ingroup initial_distr_grp
 *
 * \class Uniform
 * \brief Initialize particle attribute randomly within the interval
 *
 * Initialize particle attribute uniformly distributed within the interval [a,b]
 * 
 * configuration file example
 \verbatim

   Group "Uniform" {		
      a=0.2;
      b=0.8;
   }   

 \endverbatim
 * 
 *
 */
template<class A, class A1=A>
class Uniform: public Independent<A,A1>
{
  typedef typename A::Element_t Element_t;

public:

  //! accepts particle attribute attr and initialize it
  void operator() (A& attr)
  {
    for ( int i=0; i<attr.size(); i++)
      {
	attr(i) = Element_t( mA + (mB-mA)/static_cast<double>(attr.size()) * (i+0.5) ) ;
      }
  }


  //! setups distribution by reading config file 
  void SetupFromConfigGroup(FileInput& in)
  {
    in.ChangeGroup("Uniform");

    mA = in.get_param("a");
    mB = in.get_param("b");

    in.ChangeGroup("..");    
  };


private:
    
  double mA;
  double mB;
};

#endif



