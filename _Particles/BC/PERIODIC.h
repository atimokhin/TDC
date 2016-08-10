#ifndef PERIODIC_H
#define PERIODIC_H

#include "tdc_particles_bc.h"

/**
 * \ingroup particles_bc_grp
 *
 * \class PERIODIC 
 * Periodic BC for particles (particles emerges at the
 * other end of the domain)
 *
 * configuration file example
 \verbatim

   Group "BoundaryConditions" {	 
      Type = 'PERIODIC';
   }	 
 
 \endverbatim
 *
 */
template<class EM, class P>
class PERIODIC: public TDC_ParticlesBC<EM,P>
{
  typedef TDC_ParticlesBC<EM,P> Base_t;
public:

  //! Setup particle boundary conditions from config file group
  void SetupFromConfigGroup(FileInput& in) 
  {
    Base_t::_GeneralBCType = Base_t::BC_PERIODIC;
  };

  //! BC that cannont be expressed in terms of POOMA BC methods
  bool ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, ParticleID& p_id, double t, double dt ) 
  {
    return false;
  };
};

#endif
