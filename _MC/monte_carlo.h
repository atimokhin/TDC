#ifndef MONTE_CARLO_H
#define MONTE_CARLO_H

#include <iostream>

#include "ATbase.h"

#include "../LIBS/RandomLib/Random.hpp"

#include "../inout/save2hdf.h"

#include "../PhysicsLib/small_functions.h"

#include "../_Environment/MagneticField/magnetic_field.h"

#include "../_Particles/particle_list.h"
#include "../_Particles/charged_patch.h"
#include "../_Particles/pairs_patch.h"
#include "../_Particles/ID/particle_id.h"

#include "../_Mesh/my_grid.h"

#include "Photons/photons.h"
#include "Emission/CR.h"
#include "PairProduction/gamma2pair.h"


/**
 * \defgroup monte_carlo_grp {MC} Monte Carlo
 * 
 */

/**
 * \ingroup monte_carlo_grp
 * 
 * \class MonteCarlo
 * 
 * \brief This class is responsible for organizing sequence of Monte Carlo
 * part of the code
 *
 *
 * config file example
 \verbatim

Group "MONTE_CARLO" {

   ! Dimension of the internal Photon array
   MaxNumberOfPhotons = 150;

   ! -------------------------------------
   ! Pair creation smoothing   (if not set - false)
   ! smooth pair injection over several cells?
   DoPairInjectionSmoothing ? Yes;
   NSmoothCells = 75;
   ! -------------------------------------

   ! -------------------------------------
   ! >>>> test run parameters
   ! -------------------------------------
   !* On the spot pair creation (if not set - false)
   DoOnTheSpotPairCreation  ? No;
   ! -------------------------------------
   !* Reduce rate of pair injection (if not set - false)
   DoReducePairInjectionRate ? No;
   PairInjectionRate_Factor = 0.25;
   ! -------------------------------------
   !* using for tests: fix the seed of RNG
   SetSeedForRandomNumberGenerator ? No;
   ! the seed for RNG
   Seed = 1234;
   ! -------------------------------------
    
     ...
     ...
} 

 \endverbatim
 */
class MonteCarlo
{
public:
  //! constructor
  MonteCarlo();

  //! create virtual pairs - one of two functions called in Cascade::RunSimulations()
  template<class Particles, class Pairs>
  bool Particles2Pairs( ParticleList<Particles>& plist, Pairs& pairs, 
			double t, double dt, ParticleID& id );

  //! inject particles from virtual pairs - one of two functions called in Cascade::RunSimulations()
  template<class Particles, class Pairs>
  bool Pairs2Particles(Pairs& pairs, ParticleList<Particles>& plist, double t, double dt);

  //! Setup from config file
  void SetupFromConfig(FileInput& in);

  void SetMagneticField(MagneticField* p_mf);

  //! All photon emission processes of individual particles
  template<class Particles>
  bool SingleParticleEmission(double dt, int i, Particles& p);
  //! Create new pairs from Photons lists
  template<class Pairs>
  bool CreatePairs(Pairs& pairs, ParticleID& id);

  //! set photons (for test purpouses)
  void SetPhotons(Photons& ph) { _Ph = ph; };

private:
  //! Internal container holding photons emitted by particles
  Photons _Ph;
  //! class for photon propagation and pair creation 
  Gamma2Pair _G2P;

  //@{
  //! Members describing physical processes of photon emission
  CR _CR;
  //!@}

  //! flag controlling general MC behavior
  bool _DoPairInjectionSmoothing_Flag;
  bool _DoOnTheSpotPairCreation_Flag;
  bool _DoReducePairInjectionRate_Flag;

  //! length over which injection will be smoothed
  double _dX_Smooth;
  //! reduce number of injected pair by this factor
  double _PairInjectionRate_Factor;
  //! random number generator for sampling (pair creation smoothing,injection rate)
  RandomLib::Random _Rand; 
};



/** 
 * This function produces Pairs from Particles. It is used in the main
 * Cascade routine
 *
 * Call sequence:
 * it iterates over all charged particles in ParticleList.
 * For each particle:
 * - it calls SingleParticleEmission(double dt, int i, Particles& p).
 *   it fills internal photon container with emitted photons (if any).
 * - If photons were emitted:
 *   [-# Particle velocity will be updated -not yet -- wait for ICS!]
 *   -# CreatePairs(Pairs& pairs) is called, it produces virtual pairs
 *      from the emitted photons, adding members to the Pairs object and
 *      <b> clears the Photon container _Ph </b>
 *   -# Pairs are swapped
 * 
 *
 * @param pl    [in/out] ParticleList - emitters
 * @param pairs [out]    Pairs - resulting virtual pairs
 * @param t     current time
 * @param dt    timestep
 *
 * @return true if pairs were created
 */
template<class Particles, class Pairs>
bool MonteCarlo::Particles2Pairs(ParticleList<Particles>& pl, 
                                 Pairs& pairs, 
                                 double t, double dt,
                                 ParticleID& id)
{
  bool pairs_created = false;

  // iterate over particle list
  for (int ipl=0; ipl<pl.Size(); ipl++)
    {
      // patch number
      int i_patch = 0;
      // local patch of particles
      Charged_Patch<Particles>   p(*pl[ipl],i_patch);
      // set particle id generator
      id.SetPatch(i_patch);

      // iterate over individual particles in the local patch
      int n = p.size();
      for ( int i=0; i<n; i++)
	{
	  // If this particle emits photons then:
	  if ( SingleParticleEmission(dt, i, p) )
	    {
	      // set common photons parameters
	      _Ph.SetInheritedParams( p.X(i)(0),t, p.P_par(i), p.IDTS(i),p.ID(i) );
	      
	      // Create pairs 
	      pairs_created = CreatePairs(pairs,id) || pairs_created;

	      // clean all photons container
	      _Ph.Clear();
	    }
	}
    }
  if ( pairs_created ) pairs.Swap();

  return pairs_created;
}


/** 
 * This function produces Particles from Pairs. It is used in the main
 * Cascade routine. 
 *
 * At each time step iterates over Pairs class and looks for virtual
 * pairs wich must be injected in the time inteval [t,t+dt]. 
 * 
 * For each such virtual pair it:
 * - Calculates particle (electron and positron)  parameters 
 *   (momentum componets) 
 * - Injects this pairs by creating new particles. 
 * - Stores request for deletion of the virtual pair.
 * - if particles are injected injected Pairs are detsroyed, and 
 *    Pairs and Particles are swapped
 *
 * <b> In this version particles equally shear the energy of the prodicing photon </b>
 * 
 * @param t     current time
 * @param dt    current timestep
 * @param pairs reference to Pairs class object
 * @param pl    reference to Particle List object
 *
 * @return true if pairs were created
 */
template<class Particles, class Pairs>
bool MonteCarlo::Pairs2Particles(Pairs& pairs_global, 
                                 ParticleList<Particles>& pl, 
                                 double t, double dt)
{
  bool do_inject_particles = true;
  bool particles_injected  = false;

  // patch number
  int i_patch = 0;
  // local patch of particles
  Pairs_Patch<Pairs>   pairs(pairs_global,i_patch);

  Particles& electrons = pl.GetParticles("Electrons");
  Particles& positrons = pl.GetParticles("Positrons");

  double p_par, p_perp;

  double t_new = t+dt;
  // iterate over individual pairs in the local patch
  int n = pairs.size();
  for ( int i=0; i<n; i++)
    {
      if ( pairs.T_cr(i) < t_new )
	{
	  if ( !particles_injected ) particles_injected = true;

          // if asked reduce number of injected particles
          // by setting flag 'do_inject_particles' to 'true' with probability
          // '_PairInjectionRate_Factor'
          if ( _DoReducePairInjectionRate_Flag )
            do_inject_particles = _Rand.Prob(_PairInjectionRate_Factor);

          // inject particles if flag 'do_inject_particles' is 'true'
          if ( do_inject_particles )
            {
              // particles chracteristics
              double e2   = pairs.E(i)*pairs.E(i);
              double psi2 = pairs.Psi(i)*pairs.Psi(i);

              p_par  = SF::Sign(pairs.E(i)) * sqrt( (e2-4)/(4+e2*psi2) );
              p_perp = 0;

              // create particles
              positrons.AddParticle(pairs.Weight(i), pairs.X_cr(i)(0), p_par, p_perp, 
                                    pairs.Origin(i), pairs.IDTS(i), pairs.ID(i) );
              electrons.AddParticle(pairs.Weight(i), pairs.X_cr(i)(0), p_par, p_perp, 
                                    pairs.Origin(i), pairs.IDTS(i), pairs.ID(i) );
            }

	  // store destroy request for pair in any case
	  pairs_global.deferredDestroy (Loc<1>(i),i_patch);
	}
    }

  if ( particles_injected )
    {
      pairs_global.performDestroy();
      pairs_global.Swap();

      pl.Swap();
    }

  return  particles_injected;
};





/** 
 * Photon emission of individual particles is done in this function.
 * 
 * @param dt timestep
 * @param i  number of particle
 * @param p  Particles class reference
 * 
 * @return true if photons were emitted 
 */
template<class Particles>
inline bool MonteCarlo::SingleParticleEmission(double dt, int i, Particles& p)
{
  return _CR.EmitPhotons(dt, i,p, _Ph);
};


/** 
 * This fuction iterates over internal Photons list _Ph and for each photon
 * - calls _G2P.IsAbsorbed(x_cr, psi_cr) for doing Monte Carlo sampling 
 *   of photon absorption
 * - if photon is absorbed it calculates injection time and creates new virtual pair.
 * 
 * @param pairs reference to the Pairs object
 */
template<class Pairs>
bool MonteCarlo::CreatePairs(Pairs& pairs, ParticleID& id)
{
  bool pairs_created = false;
  double x_cr, psi_cr;

  int e_sign = _Ph.Get_MomentumSign();

  // iterate over all photons *********
  for (int i=0; i<_Ph.Size(); i++)
    {
#ifdef TEST_GAMMA2PAIR
      bool test_photon_absorbed = false;
#endif

      // setup photons parameters in Gamma2Pair class
      _G2P.SetupPhoton(_Ph.E[i],_Ph.Get_X0(),_Ph.Get_Direction() );
      // if absorbed - create pair
      if ( _G2P.IsAbsorbed(x_cr, psi_cr) ) 
	{
	  if ( !pairs_created ) pairs_created = true;
	  // pair injection time
	  double t_cr = _Ph.Get_T0() + fabs( x_cr - _Ph.Get_X0() );

	  // if requested use on the spot approximation <<============= (!!!)
	  if ( _DoOnTheSpotPairCreation_Flag )
	    {
	      t_cr = _Ph.Get_T0();
	      x_cr = _Ph.Get_X0();
	    }	  
          // ----------------------------------------------------------
	  // if requested do pair creation position smoothing <<====== (!!!)
	  if ( _DoPairInjectionSmoothing_Flag )
	    {
	      t_cr += _dX_Smooth * _Rand.Fixed();
	    }	  
          // ----------------------------------------------------------

	  // add newly created pair
	  pairs.AddPair(_Ph.Weight[i], 
                        t_cr, x_cr, _Ph.Get_X0(), 
                        e_sign*_Ph.E[i], psi_cr, 
                        _Ph.Origin[i],
			_Ph.Get_IDTS(), _Ph.Get_ID(), id.GetIDTS(), id.GetID() );

#ifdef TEST_GAMMA2PAIR
	  test_photon_absorbed = true;
	  std::cout<<"travelled l="<<x_cr - _Ph.Get_X0()<<"\n";
	  std::cout<<">>>>>>>[ABSORBED]<<<<<<<\n\n";
#endif
	}

#ifdef TEST_GAMMA2PAIR
      if (!test_photon_absorbed)  std::cout<<".......[ESCAPED]........\n\n";
#endif
    }

  return pairs_created;
}


#endif
