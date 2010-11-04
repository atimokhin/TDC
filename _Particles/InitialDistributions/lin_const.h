#ifndef LIN_CONST_H
#define LIN_CONST_H

#include "../../LIBS/RandomLib/Random.hpp"

#include "ATbase.h"

#include "independent.h"


/**
 * \ingroup initial_distr_grp
 *
 * \class LinConst
 * \brief Initialize particle attribute randomly within the interval
 *
 * Initialize particle attribute randomly within the interval [a,b]
 * 
 * configuration file example
 \verbatim

   Group "LinConst" {		
      a=0.2;
      b=0.8;
      Nab=20000;
   }   

 \endverbatim
 * 
 *
 */
template<class A, class A1=A>
class LinConst: public Independent<A,A1>
{
  typedef typename A::Element_t Element_t;

public:

  //! accepts particle attribute attr and initialize it
  void operator() (A& attr)
  {
    int n_lin = attr.size() - _Nab;
    // constant part of distribution
    for ( int i=0; i<_Nab; i++)
      attr(i) = Element_t(_A + (_B-_A)/(_Nab-1)*i) ;

    for ( int i=_Nab; i<attr.size(); i++)
      attr(i) = Element_t(_A*sqrt(_Random.FixedN()));
  }


  //! setups distribution by reading config file 
  void SetupFromConfigGroup(FileInput& in)
  {
    in.ChangeGroup("LinConst");

    _A   = in.get_param("a");
    _B   = in.get_param("b");
    _Nab = static_cast<int>(in.get_param("Nab"));

    in.ChangeGroup("..");    
  };


private:
  
  RandomLib::Random _Random;     
  
  double _A;
  double _B;
  int _Nab;
};

#endif



