#ifndef INITIAL_DISTRIBUTION_H
#define INITIAL_DISTRIBUTION_H

#include "ATbase.h"



/**
 * \defgroup initial_distr_grp {ID} Initial Distributions
 *
 * \ingroup particles_grp
 */


/**
 * \ingroup initial_distr_grp
 *
 * \class InitialDistribution
 *
 * Base class for initial distribution of particles parametrized by
 * the type of Particle attribute (some of DynamicArray<>'s). 
 */
template<class A, class A1=A>
class InitialDistribution
{
public:

  //! Destructor
  virtual ~InitialDistribution() {};

  //! accepts particle attribute attr and initialize it
  virtual void operator() (A& attr) = 0;
  //! accepts particle attribute attr, attr_base and initialize attr(attr_base)
  virtual void operator() (A& attr, A1& attr_base) = 0;

  //! setups distribution by reading config file 
  virtual void SetupFromConfigGroup(FileInput& in) {};
};

#endif



