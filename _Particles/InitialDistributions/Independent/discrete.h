#ifndef DISCRETE_H
#define DISCRETE_H

#include <vector>

#include "ATbase.h"

#include "independent.h"


/**
 * \ingroup initial_distr_grp
 *
 * \class Discrete
 * \brief Initialize first n particle in attribute individually.
 *
 * Initialize first n particle in attribute individually.
 *
 * configuration file example:
 \verbatim

	 Group "X" {		
	    DistributionName='Discrete';
	    Group "Discrete" {		
	       NumberOfValues = 2;
	       Value_#0=0.2e-5;
	       Value_#1=0.8e-5;
	    }   
	 }

 \endverbatim
 *
 */
template<class A, class A1=A>
class Discrete: public Independent<A,A1>
{
  typedef typename A::Element_t Element_t;

public:

  //! accepts particle attribute attr and initialize it
  void operator() (A& attr)
  {
    for ( int i=0; i<initial_values.size(); i++)
	attr(i) = initial_values[i];
  }



  //! setups distribution by reading config file 
  void SetupFromConfigGroup(FileInput& in)
  {
    in.ChangeGroup("Discrete");

    int n_vals = static_cast<int>(in.get_param("NumberOfValues"));
    for ( int i=0; i<n_vals; i++)
	initial_values.push_back(Element_t(in.get_param("Value",i)));

    in.ChangeGroup("..");
  }


private:

  std::vector<Element_t> initial_values;
};

#endif



