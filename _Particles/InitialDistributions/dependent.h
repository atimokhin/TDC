#ifndef DEPENDENT_H
#define DEPENDENT_H

#include <vector>

#include "ATbase.h"

#include "initial_distribution.h"


/**
 * \ingroup initial_distr_grp
 *
 * \class Dependent 
 * \brief Base class for dependent initialization
 */
template<class A, class A1=A>
class Dependent: public InitialDistribution<A,A1>
{
public:

  //! abort calculation if one-argument operator is called
  void operator() (A& attr) 
  {
    cout<<"Dependent:: one argument operator in initial distribution is not defined\n!";
    exit(1);     
  };

};

#endif
