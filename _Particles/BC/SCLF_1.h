#ifndef SCLF_1__H
#define SCLF_1__H

#include "../../SetupProperties/fmp_props.h"

#include "tdc_particles_bc.h"


/**
 * \ingroup particles_bc_grp
 *
 * \class SCLF_1 
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
 Type = 'SCLF_1';
 ! momentum of injected electrons
 P_inject_e = 10;
 ! momentum of injected protons
 P_inject_p = 10;
 ! minimum fractional momentum of injected particles
 MIN_WEIGHT = 1e-4;
 }	 
 
 \endverbatim
 *
 */
template<class EM, class P>
class SCLF_1: public TDC_ParticlesBC<EM,P>
{
  typedef TDC_ParticlesBC<EM,P> Base_t;

public:

  //! Setup particle boundary conditions from config file group
  void SetupFromConfigGroup(FileInput& in);

  //! BC that cannont be expressed in terms of POOMA BC methods
  bool ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, ParticleID& p_id, double t, double dt );

private:

  //! minimal fraction weight of injected particles
  double _MIN_WEIGHT;

  //! initial momentum of injected particles
  double _P_inject_e, _P_inject_p;
};


//! Setup particle boundary conditions from config file group
template<class EM, class P>
void SCLF_1<EM,P>::SetupFromConfigGroup(FileInput& in)
{
  Base_t::_GeneralBCType = Base_t::BC_OUTFLOW;
  _P_inject_e = in.get_param("P_inject_e");
  _P_inject_p = in.get_param("P_inject_p");
  _MIN_WEIGHT = in.get_param("MIN_WEIGHT");
}


template<class EM, class P>
bool SCLF_1<EM,P>::ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, ParticleID& p_id, double t, double dt )
{
  P     *pParticles; 
  double p_inject;
  FMPProps fmp;

  Pooma::blockAndEvaluate();

  double dx = Pooma::cellVolumes(em.Rho).comp(0).read(0);
  // currend discreapance needed to set E(0) to zero
  double j_inj = em.dJ_ToSet_E_i_OnTheNextTimeStep(0, 0, t, dt);

  cout<<"   before:  j(0)="<<setw(10)<<em.J(0)<<" Rho(0)="<<em.Rho(0)<<" E(0)="<<em.E(0); 
  cout<<"  delta_j="<<j_inj<<"\n";

  if ( j_inj < 0 ) // inject electrons ----------------
    {
      pParticles = &( plist.GetParticles("Electrons") );
      p_inject = _P_inject_e;      
    }
  else               // inject protons ------------------
    {
      pParticles = &( plist.GetParticles("Protons") );
      p_inject = _P_inject_p;
    }

  // velocity of injected particles
  double beta_inj = p_inject/sqrt(1+p_inject*p_inject);
  // distance particle travels at injection in units of dx
  double delta = beta_inj*dt/dx;

  // total statistical weight of injected particles
  double w_inj = abs(j_inj)*dx/( beta_inj*fmp.W0() );
  // number of particles with weight 1 to be injected
  int    n_inj = static_cast<int>( floor(w_inj) );	  
  // fractional weight of the last particle
  double w_inj_last = w_inj-n_inj;


  double coeff_j_rho = pParticles->Q * fmp.W0() * delta;
  // inject particles with weight 1
  if ( n_inj > 0 )
    {
      Interval<1> I = pParticles->Create(n_inj);
      pParticles->Origin(I) = 'P';
      pParticles->Weight(I) = 1;
      pParticles->X(I)      = (delta-0.5)*dx;
      pParticles->P_par(I)  = p_inject;
      pParticles->P_perp(I) = 0;
      // assign ID's to injected particles
      for (Interval<1>::iterator iter=I.begin();  iter!=I.end(); iter++)
        {
          pParticles->IDTS(*iter) = p_id.GetIDTS();
          pParticles->ID(*iter)   = p_id.GetID();
        }
      // update current and charge densities
      em.J(0)   += coeff_j_rho/dt*(I.max()-I.min()+1);
      em.Rho(0) += coeff_j_rho/dx*(I.max()-I.min()+1);
    }
  // the last particle is injected with fractional weight 
  if ( w_inj_last > _MIN_WEIGHT )
    {
      Interval<1> I = pParticles->Create(1);
      pParticles->Origin(I) = 'P';
      pParticles->Weight(I) =  w_inj_last;
      pParticles->X(I)      = (delta-0.5)*dx;
      pParticles->P_par(I)  = p_inject;
      pParticles->P_perp(I) = 0;
      // assign ID's to injected particles
      for (Interval<1>::iterator iter=I.begin();  iter!=I.end(); iter++)
        {
          pParticles->IDTS(*iter) = p_id.GetIDTS();
          pParticles->ID(*iter)   = p_id.GetID();
          // update current and charge densities
          em.J(0)   += pParticles->Weight(*iter) * coeff_j_rho/dt;
          em.Rho(0) += pParticles->Weight(*iter) * coeff_j_rho/dx;
        }
    }
  
  cout<<"    after:  j(0)="<<setw(10)<<em.J(0)<<" Rho(0)="<<em.Rho(0)<<"\n"; 

  plist.Sync();

  return true;
}

#endif
