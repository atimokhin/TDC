#ifndef RS_1_H
#define RS_1_H

#include "tdc_particles_bc.h"

/**
 * \ingroup particles_bc_grp
 *
 * \class RS_1 
 *
 * \brief Ruderman-Sutherland condition: Particles leaves domain;
 * there is no particle inflow
 *
 * configuration file example
 \verbatim

   Group "BoundaryConditions" {	 
      Type = 'RS_1';
   }	 
 
 \endverbatim
 *
 */
template<class EM, class P>
class RS_1: public TDC_ParticlesBC<EM,P>
{
  typedef TDC_ParticlesBC<EM,P> Base_t;
public:

  //! Setup particle boundary conditions from config file group
  void SetupFromConfigGroup(FileInput& in) 
  {
    Base_t::_GeneralBCType = Base_t::BC_OUTFLOW;
  };

  //! BC that cannont be expressed in terms of POOMA BC methods
  bool ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, ParticleID& p_id, double t, double dt ) 
  {
    return false;
  };
};

#endif
