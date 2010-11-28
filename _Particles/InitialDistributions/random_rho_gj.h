#ifndef RANDOM_RHO_GJ_H
#define RANDOM_RHO_GJ_H

#include <vector>

#include "ATbase.h"

#include "independent.h"

#include "../../LIBS/RandomLib/Random.hpp"
#include "../../_MC/Samples/Cutpoint.h"

#include "../../SetupParameters/geometry_params.h"

#include "../../_Environment/RhoGJ/rho_gj_maker.h"




/**
 * \ingroup initial_distr_grp
 *
 * \class RandomRhoGJ
 * \brief Initialize particle attribute (positions) according to \rho_{GJ}
 * 
 * - N_Particles_Uniform will be randomly distributed in the whole domain
 *
 * - the rest of the particles will be distributed according to 
 *   distribution of the Goldrech-Julian charge density,
 *   either positive or negative values. If \rho_{GJ} does not have
 *   the requested sign in the domain, the rest of the particles will
 *   be distributed uniformly too
 *
 * configuration file example
 * 
 \verbatim

   Group "RandomRhoGJ" {		
      ! number of possible positions
      N_Positions = 10000;
      ! number of particles uniformly distributed
      N_Particles_Uniform = 1000;
      ! must be [Positive|Negative]
      ChargeSign = 'Negative';
   }   

 \endverbatim
 * 
 *
 */
template<class A, class A1=A>
class RandomRhoGJ: public Independent<A,A1>
{
  typedef typename A::Element_t Element_t;

public:
  //! accepts particle attribute attr and initialize it
  void operator() (A& attr);

  void SetupFromConfigGroup(FileInput& in);

private:
  int _N_Positions;
  int _N_Particles_Uniform;
  bool _NonUniform_flag;

  Cutpoint _CM;
  RandomLib::Random _Random;     
};


//! accepts particle attribute attr and initialize it
template<class A, class A1>
void RandomRhoGJ<A,A1>::operator() (A& attr)
{
  GeometryParams  geom;

  int n_uniform;

  if ( _NonUniform_flag )
    {
      // distribute the  particles according to RhoGJ distribution
      for ( int i=_N_Particles_Uniform; i<attr.size(); i++)
        {
          attr(i) = Element_t( _CM(_Random.FixedN()) ) ;
        }
      n_uniform = _N_Particles_Uniform;
    }
  else
    {
      n_uniform = attr.size();
      std::cout<<"Warning: Initial particle distribution is UNIFORM!\n";
    }

  // distribute n_uniform particles uniformly in the domain
  for ( int i=0; i<n_uniform; i++)
    {
      attr(i) = Element_t( geom.L() * _Random.FixedN() );
    }    
}


//! setups distribution by reading config file 
template<class A, class A1>
void RandomRhoGJ<A,A1>::SetupFromConfigGroup(FileInput& in)
{
  GeometryParams  geom;
  
  // -------------------------------------------
  // save original group [inside particular Particles]
  ATbase::Group_t* p_g = in.Get_pCurrentGroup();
  // read this class configuration parameters
  in.ChangeGroup("RandomRhoGJ");
  _N_Positions         = in.get_param("N_Positions");
  _N_Particles_Uniform = in.get_param("N_Particles_Uniform");
  string charge_sign   = in.get_name("ChargeSign");
  in.ChangeGroup();

  // ********************************************
  // Setup GJ charge density ********************
  // ********************************************
  in.ChangeGroup("RHO_GJ");

  string rhogj_functor_name = in.get_name("Type");
  RhoGJFunctor* p_rhogj = RhoGJFunctorMaker(in).pMake(rhogj_functor_name); 
  p_rhogj->SetupFromConfigGroup(in);

  in.ChangeGroup();
  // ********************************************
  // return to the original group
  in.ChangeGroup(p_g);
  // -------------------------------------------


  // setup tables for cutpoint method ----------
  vector<double> xx(_N_Positions);
  vector<double> ff(_N_Positions);
  vector<double>::iterator iter_xx = xx.begin();
  vector<double>::iterator iter_ff = ff.begin();

  bool positive_flag;
  if (charge_sign == "Positive")
    positive_flag = true;
  else if (charge_sign == "Negative")
    positive_flag = false;
  else
    {
      std::cout<<"wrong \"ChargeSign\" in RandomRhoGJ!\n";
      exit(1);
    }

  double ff_prev=0;
  for (int i=0; i<_N_Positions; i++)
    {
      double x = i*geom.L()/(_N_Positions-1);
      double f = (*p_rhogj)(x);
      if ( (positive_flag && f > 0) || (!positive_flag && f <= 0) )
        {
          *iter_xx = x;
          iter_xx++;

          *iter_ff = ff_prev + abs(f);
          ff_prev = *iter_ff;
          iter_ff++;
        }
    }
  // erase non-filled values
  xx.erase(iter_xx, xx.end());
  ff.erase(iter_ff, ff.end());
  // set _NonUniform_flag
  _NonUniform_flag = xx.size()>2 ? true : false;
  // renormalize ff
  for (iter_ff=ff.begin(); iter_ff != ff.end(); iter_ff++)
    {
      *iter_ff /= ff.back();
    }
  // Initialize cutpoint table
  _CM.Initialize(xx,ff,ff.size()/2);
};

#endif
