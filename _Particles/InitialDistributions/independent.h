#ifndef INDEPENDENT_H
#define INDEPENDENT_H

#include <vector>

#include "ATbase.h"

#include "initial_distribution.h"


/**
 * \ingroup initial_distr_grp
 *
 * \class Independent 
 * \brief Base class for independent initialization
 */
template<class A, class A1=A>
class Independent: public InitialDistribution<A,A1>
{
public:

  //! abort calculation if one-argument operator is called
  void operator() (A& attr, A1& attr_base) 
  {
    cout<<"Independent:: two argument operator in initial distribution is not defined\n!";
    exit(1);     
  };

};

#endif
