#ifndef RS_2_H
#define RS_2_H

#include "tdc_particles_bc.h"

/**
 * \ingroup particles_bc_grp
 *
 * \class RS_2 
 *
 * \brief Ruderman-Sutherland condition: Particles leaves domain;
 * there is no particle inflow
 *
 * Particles energies exceeding _E_max are set to _Emax
 *
 * configuration file example
 \verbatim

   Group "BoundaryConditions" {	 
      Type = 'RS_2';

      E_max = 3e3;
   }	 
 
 \endverbatim
 *
 */
template<class EM, class P>
class RS_2: public TDC_ParticlesBC<EM,P>
{
  typedef TDC_ParticlesBC<EM,P> Base_t;
public:

  //! BC using POOMA BC methods
  void SetupGeneralBC(ParticleList<P>& plist);

  //! Setup particle boundary conditions from config file group
  void SetupFromConfigGroup(FileInput& in); 

  //! BC that cannont be expressed in terms of POOMA BC methods
  bool ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, ParticleID& p_id, double t, double dt ) 
  {
    return false;
  };

private:
  double _E_max;
};

//! Setup particle boundary conditions from config file group
template<class EM, class P>
void RS_2<EM,P>::SetupFromConfigGroup(FileInput& in) 
{
  Base_t::_GeneralBCType = Base_t::BC_OUTFLOW;
  _E_max = in.get_param("E_max");
};


template<class EM, class P>
void RS_2<EM,P>::SetupGeneralBC(ParticleList<P>& plist)
{

  Base_t::SetupGeneralBC(plist);

  typename P::Scalar_t lower(-_E_max);
  typename P::Scalar_t upper( _E_max);
  // bc class
  AbsorbBC<typename  P::Scalar_t> bc(lower,upper);
  for (int i=0; i<plist.Size(); i++)
    plist[i]->addBoundaryCondition(plist[i]->P_par,plist[i]->P_par,bc);
}

#endif
