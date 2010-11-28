#ifndef SINX_H
#define SINX_H

#include "ATbase.h"

#include "../../../SetupParameters/geometry_params.h"

#include "dependent.h"


/**
 * \ingroup initial_distr_grp
 *
 * \class SinX_Base
 *
 * \brief base class for setting attribute as function of other attribute x :
 * \f$ \sin(kx) \f$
 *
 * Sets up parameters from config file
 *
 * configuration file example
 \verbatim

	 Group "Momentum" {		
            DependsOnAttribute='Position';
            DistributionName='SinX';
            Group "SinX" {		
               Amplitude = 0.005;
               nk=2;

	       a=0;
	       b=NParam[GEOMETRY](LENGTH);	       	       
            }
	 }
 
 \endverbatim
 */
class SinX_Base
{

public:

  //! setups distribution by reading config file 
  void SetupFromConfigGroup(FileInput& in)
  {
    in.ChangeGroup("SinX");

    _Amplitude = in.get_param("Amplitude");
    _A = in.get_param("a");
    _B = in.get_param("b");

    int nk = static_cast<int>(in.get_param("nk"));
    _K = 2*ATbase::constant::PI*nk/(_B-_A);

    in.ChangeGroup("..");
  };

protected:

  double _K;
  double _Amplitude;

  double _A;
  double _B;
};


/**
 * this general template sets attribute depending on different
 * attribute type (Dynamic Array of Vector<1>)
 * 
 */
template<class A, class A1>
class SinX: public Dependent<A,A1>, public SinX_Base
{
  typedef typename A::Element_t T;
  
public:

  void SetupFromConfigGroup(FileInput& in)
  {
    SinX_Base::SetupFromConfigGroup(in);
  };


  //! accepts particle attribute attr and initialize it
  void operator() (A& attr, A1& attr_base ) 
  { 
    int n = attr_base.size();
    for ( int i=0; i< n; i++ )
      {
	attr(i) = static_cast<T>( _Amplitude*sin( _K*(attr_base(i)(0)-_A) ) );
      }
  };


};



/**
 * this specialized template sets attribute depending on the same
 * attribute type
 * 
 */
template<class A>
class SinX<A,A>: public Dependent<A,A>, public SinX_Base
{
public:

  void SetupFromConfigGroup(FileInput& in)
  {
    SinX_Base::SetupFromConfigGroup(in);
  };

  //! accepts particle attribute attr and initialize it
  void operator() (A& attr, A& attr_base ) 
  { 
    attr = _Amplitude*sin( _K*(attr_base-_A) );
  };

};

#endif




