#ifndef INJECT__H
#define INJECT__H

#include "../../SetupProperties/fmp_props.h"

#include "tdc_particles_bc.h"


/**
 * \ingroup particles_bc_grp
 *
 * \class INJECT 
 *
 * \brief \f$ E_{||}(0) = 0 \f$ is achieved by injection of
 * monoenergetic particles with momemtum _P_inject_e for electrons and
 * _P_inject_p for protons:
 *
 * It injects exactly the amount of particle to provide electric
 * current shorting out the electric field in the node 0. The accuracy
 * of current injection depends on the parameter MIN_WEIGHT: several
 * particles with unit weight are injected and one with a fractional
 * weight
 *
 * configuration file example
 \verbatim

   Group "BoundaryConditions" {	 
      Type = 'INJECT';
      ! number of particles injected at each timestep
      N_INJECT = 10;
      ! momentum of injected electrons
      P_inject_e = 10;
      ! momentum of injected protons
      P_inject_p = 10;
   }	 
 
 \endverbatim
 *
 */
template<class EM, class P>
class INJECT: public TDC_ParticlesBC<EM,P>
{
  typedef TDC_ParticlesBC<EM,P> Base_t;
public:

  //! Setup particle boundary conditions from config file group
  void SetupFromConfigGroup(FileInput& in)
  {
    Base_t::_GeneralBCType = Base_t::BC_OUTFLOW;
    _P_INJECT_E = in.get_param("P_INJECT_E");
    _N_INJECT_E = static_cast<int>(in.get_param("N_INJECT_E"));
    _P_INJECT_P = in.get_param("P_INJECT_P");
    _N_INJECT_P = static_cast<int>(in.get_param("N_INJECT_P"));
  }

  //! BC that cannont be expressed in terms of POOMA BC methods
  bool ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, double t, double dt );


private:

  //! number of particles injected at each timestep
  int _N_INJECT_E, _N_INJECT_P;

  //! initial momentum of injected particles
  double _P_INJECT_E, _P_INJECT_P;
};




template<class EM, class P>
bool INJECT<EM,P>::ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, double t, double dt )
{
  P     *pParticles; 
  double p;
  int n;
  FMPProps fmp;

  Pooma::blockAndEvaluate();

  double dx = Pooma::cellVolumes(em.Rho).comp(0).read(0);

  for (int i=0; i<2; i++)
    {
      if ( i==0 ) // inject electrons 
	{
	  pParticles = &( plist.GetParticles("Electrons") );
	  p = _P_INJECT_E;
	  n = _N_INJECT_E;
	}
      else // inject protons 
	{
	  pParticles = &( plist.GetParticles("Positrons") );
	  p = _P_INJECT_P;
	  n = _N_INJECT_P;
	}
      
      // velocity of injected particles
      double beta = p/sqrt(1+p*p);
      // distance particle travels at injection
      double delta = beta*dt/dx;
      double coeff_j_rho = pParticles->Q * fmp.W0() * delta;
      
      // inject particles with weight 1
      if ( n > 0 )
	{
	  Interval<1> I = pParticles->Create(n);
	  pParticles->Origin(I) = 'P';
	  pParticles->Weight(I) = 1;
	  pParticles->X(I)      = (delta-0.5)*dx;
	  pParticles->P_par(I)  = p;
	  pParticles->P_perp(I) = 0;
	  // update current and charge densities
	  for (int i=I.min(); i<=I.max(); i++)
	    {
	      em.J(0)   += coeff_j_rho/dt;
	      em.Rho(0) += coeff_j_rho/dx;
	    }
	}
    }
  plist.Sync();

  return true;
}

#endif
