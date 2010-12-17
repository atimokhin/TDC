#ifndef SCLF_2__H
#define SCLF_2__H


#include "tdc_particles_bc.h"

#include "../../SetupProperties/pulsar_gap_props.h"


/**
 * \ingroup particles_bc_grp
 *
 * \class SCLF_2 
 *
 *
 * At each timestep it injects E(I)__N_inj electrons and protons at
 * position - ( 1 - E(I)__Delta_inj )*dx.  So, if electrons/protons
 * are not needed, they are already out of the grid boundary and are
 * deleted on the next timestep as they move out of the grid
 *
 * NB:  E(I)__N_inj can be fractional!
 *
 * configuration file example
 \verbatim

   Group "BoundaryConditions" {	 
      Type = 'SCLF_2';
      ! Electron parameters
      E__N_inj = 2.5;
      E__P_inj = 0;
      ! must be < 0.5  <<=========
      E__Delta_inj = 0.499999;
      ! Proton parameters
      I__N_inj = 2.5;
      I__P_inj = 0;
      ! must be < 0.5  <<=========
      I__Delta_inj = 0.499999;
    }	 
 
 \endverbatim
 *
 */
template<class EM, class P>
class SCLF_2: public TDC_ParticlesBC<EM,P>
{
  typedef TDC_ParticlesBC<EM,P> Base_t;
public:

  //! Setup particle boundary conditions from config file group
  void SetupFromConfigGroup(FileInput& in);

  //! BC that cannont be expressed in terms of POOMA BC methods
  bool ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, double t, double dt );


private:

  double _E__P_inj,     _I__P_inj;     //!< momentum of injected particles
  double _E__Delta_inj, _I__Delta_inj; //!< delta of injection point
  double _E__N_inj,     _I__N_inj;     //!< number of injected particles

};

//! Setup particle boundary conditions from config file group
template<class EM, class P>
void SCLF_2<EM,P>::SetupFromConfigGroup(FileInput& in)
{
  Base_t::_GeneralBCType = Base_t::BC_OUTFLOW;
  _E__P_inj     = in.get_param("E__P_inj");
  _E__Delta_inj = in.get_param("E__Delta_inj");
  _E__N_inj     = in.get_param("E__N_inj");

  _I__P_inj     = in.get_param("I__P_inj");
  _I__Delta_inj = in.get_param("I__Delta_inj");
  _I__N_inj     = in.get_param("I__N_inj");
}



template<class EM, class P>
bool SCLF_2<EM,P>::ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, double t, double dt )
{
  double x_inj;
  // cell size
  double dx = Pooma::cellVolumes(em.Rho).comp(0).read(0);

  // inject electrons ----------------------
  P& p_E = plist.GetParticles("Electrons");
  // injection point
  x_inj = - ( 1 -_E__Delta_inj )*dx;
  // create particles and set their parameters
  Interval<1> Ie = p_E.Create( static_cast<int>(ceil(_E__N_inj)) );
  p_E.Origin(Ie) = 'P';
  p_E.Weight(Ie) = ceil(_E__N_inj)/_E__N_inj;
  p_E.X(Ie)      = x_inj;
  p_E.P_par(Ie)  = _E__P_inj;
  p_E.P_perp(Ie) = 0;
  p_E.Swap(); // swap because new particles are created <<<
  // --------------------------------------

  // inject ions --------------------------
  P& p_I = plist.GetParticles("Protons");
  // injection point
  x_inj = - ( 1 -_I__Delta_inj )*dx;
  // create particles and set their parameters  
  Interval<1> Ii = p_I.Create( static_cast<int>(ceil(_I__N_inj)) );
  p_I.Origin(Ii) = 'P';
  p_I.Weight(Ii) = ceil(_I__N_inj)/_I__N_inj;
  p_I.X(Ii)      = x_inj;
  p_I.P_par(Ii)  = _I__P_inj;
  p_I.P_perp(Ii) = 0;
  p_I.Swap(); // swap because new particles are created <<<
  // --------------------------------------
      
  return true;
}

#endif
