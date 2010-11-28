#ifndef SINX_PERTURBED_UNIFORM_H
#define SINX_PERTURBED_UNIFORM_H

#include "ATbase.h"

#include "../../../SetupParameters/geometry_params.h"

#include "independent.h"

/**
 * \ingroup initial_distr_grp
 *
 * \class SinXPerturbedUniform_Base
 *
 * \brief base class for setting attribute as function of other attribute x :
 * \f$ \sin(kx) \f$
 *
 * Sets up parameters from config file
 *
 * configuration file example
 \verbatim

	 Group "X" {		
            DistributionName='SinXPerturbedUniform';
            Group "SinXPerturbedUniform" {		
               Amplitude = 0.001;
               nk=2;

	       a=0;
	       b=NParam[GEOMETRY](LENGTH);	       	       
            }
	 }
 
 \endverbatim
 */


template<class A, class A1=A>
class SinXPerturbedUniform: public Independent<A,A1>
{
  typedef typename A::Element_t Element_t;

public:

  //! accepts particle attribute attr and initialize it
  void operator() (A& attr)
  {
    int     n = attr.size();
    double da = (_B-_A)/static_cast<double>(n);
    for ( int i=0; i< n; i++ )
      {
	double x = _A + da * (i+0.5);
	attr(i) = Element_t(x + _Amplitude*sin( _K*(x-_A) ) );
      }
  }


  //! setups distribution by reading config file 
  void SetupFromConfigGroup(FileInput& in)
  {
    in.ChangeGroup("SinXPerturbedUniform");

    _Amplitude = in.get_param("Amplitude");
    _A = in.get_param("a");
    _B = in.get_param("b");

    int nk = static_cast<int>(in.get_param("nk"));
    _K = 2*ATbase::constant::PI*nk/(_B-_A);

    in.ChangeGroup("..");
  };

private:
    
  double _K;
  double _Amplitude;

  double _A;
  double _B;
};

#endif




