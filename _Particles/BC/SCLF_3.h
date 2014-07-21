#ifndef SCLF_3__H
#define SCLF_3__H

#include "tdc_particles_bc.h"

/**
 * \ingroup particles_bc_grp
 *
 * \class SCLF_1
 * keeps density in the first cell at the level at least NPairsInjected
 * electron momentum is P_Pairs_e
 * protonon momentum is P_Pairs_p
 *
 */
template<class EM, class P>
class SCLF_3: public TDC_ParticlesBC<EM,P>
{
  typedef TDC_ParticlesBC<EM,P> Base_t;
public:

  //! Setup particle boundary conditions from config file group
  void SetupFromConfigGroup(FileInput& in)
  {
    Base_t::_GeneralBCType = Base_t::BC_OUTFLOW;

    _NPairsInjected = static_cast<int>(in.get_param("NPairsInjected"));
    _P_Pairs_e = in.get_param("P_Pairs_e");
    _P_Pairs_p = in.get_param("P_Pairs_p");
  }

  //! BC that cannont be expressed in terms of POOMA BC methods
  bool ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, double t, double dt );


private:
  
  int NumberOfParticlesInCell(P& p, 					  
                              const typename P::Traits_t::Mesh_t& m, int i_cell);



private:

  int    _NPairsInjected;
  double _P_Pairs_e;
  double _P_Pairs_p;
};



template<class EM, class P>
bool SCLF_3<EM,P>::ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, double t, double dt )
{
  P *p_particles; 
  double delta_x = Pooma::cellVolumes(em.Rho).comp(0).read(0);

  // *************************************************************
  // inject _SCLF__NFirstCell pairs in the first cell @ each timestep 
  // *************************************************************
  P *p_electrons = &( plist.GetParticles("Electrons") );
  P *p_protons   = &( plist.GetParticles("Protons") );

  int n_e = NumberOfParticlesInCell(*p_electrons, em.Rho.mesh(), 0); 
  int n_p = NumberOfParticlesInCell(*p_electrons, em.Rho.mesh(), 0); 

  int n_inj = _NPairsInjected - min(n_e,n_p);

  if ( n_inj > 0)
    {
      Interval<1> I_e = p_electrons->Create(n_inj);
      p_electrons->Origin(I_e) = 'P';
      p_electrons->Weight(I_e) = 1;
      p_electrons->X(I_e)      = 0.5*delta_x;
      p_electrons->P_par(I_e)  = _P_Pairs_e;
      p_electrons->P_perp(I_e) = 0;
	  	  
      Interval<1> I_p = p_protons->Create(n_inj);
      p_protons->Origin(I_p) = 'P';
      p_protons->Weight(I_p) = 1;
      p_protons->X(I_p)      = 0.5*delta_x;
      p_protons->P_par(I_p)  = _P_Pairs_p;
      p_protons->P_perp(I_p) = 0;
    }
  
  return true;
}

template<class EM, class P>
int SCLF_3<EM,P>::NumberOfParticlesInCell(P& p, 
                                          const typename P::Traits_t::Mesh_t& m, int i_cell)
{
  int number_of_particles=0;
  Loc<1> cell(i_cell);

  for ( int i=0; i<p.size(); i++ )
    {
      if ( cell == m.cellContaining ( p.X(i) ) )
        number_of_particles++;
    }
  
  return number_of_particles;
}


#endif
