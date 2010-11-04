#ifndef LARGE_PARTICLES_TOOLS_H
#define LARGE_PARTICLES_TOOLS_H

#include "ATbase.h"

#include "../../LIBS/RandomLib/Random.hpp"

#include "../particle_list.h"

/**
 * \ingroup particles_grp
 * 
 * \defgroup lpt_grp  {LPT} Large Particle Tools 
 */

/**
 * \ingroup lpt_grp
 *
 * \class LargeParticlesTools This class contains method for reducing
 * number of particles to (on average) optimal number N_OPT, when number
 * of particles exceeds the maximum allowed number N_MAX
 * 
 */
class LargeParticlesTools
{
public:
  //! Adjust number of particle Particles interface
  template<class P>
  bool AdjustNumberOfParticles(P& p);
  //! Adjust number of particle ParticleList interface
  template<class P>
  bool AdjustNumberOfParticles(ParticleList<P>& pl);

  //! Adjust number of particle because of pair overproduction Particles interface
   template<class P>
  bool AdjustNumberOfParticlesForPairOverproduction(P& p);
  //! Adjust number of particle because of pair overproduction ParticleList interface
   template<class P>
  bool AdjustNumberOfParticlesForPairOverproduction(ParticleList<P>& pl);

  //! reduce number fo particle - the actual reducing function
  template<class P>
  void ReduceNumberOfParticles(P& p, double prob);

  //! setup all parameters from config file group
  void SetupFromConfig(FileInput& in);

private: 
  //! random number generator for sampling particle deletion
  RandomLib::Random _Rand; 
};


/** 
 * This function checks particle number and if it exceeds the maximum
 * allowed number N_MAX (stored as a property in My_Particles class)
 * reduces the number of particles to (on overage) N_OPT particles. It
 * iterates over all particles and deletes each particle with
 * probability 1-N_OPT/N_MAX
 * 
 * @param p  reference to a My_Particles class
 * 
 * @return <b> true </b> is particle number was adjusted, <b> false
 * </b> otherwise
 */
template<class P>
inline bool LargeParticlesTools::AdjustNumberOfParticles(P& p)
{
  // particle number does not exceed the upper limit
  // do nothing and return false
  if ( p.size() < p.N_MAX ) 
    {
      return false;
    }
  else
    {
      double prob = 1 - static_cast<double>(p.N_OPT)/p.N_MAX;
      ReduceNumberOfParticles(p, prob);
      return true;
    }
}


/** 
 * This function iterates over all My_Particles in PartriclesList and
 * calls AdjustNumberOfParticles(p) for each ot them 
 * 
 * @param p  reference to a ParticlesList class
 * 
 * @return <b> true </b> is any particle number was adjusted, <b> false
 * </b> otherwise
 */
template<class P>
inline bool LargeParticlesTools::AdjustNumberOfParticles(ParticleList<P>& pl)
{
  bool is_adjusted = false;
  for (int i=0; i<pl.Size(); i++) 
    {
      bool is_adjusted_now = AdjustNumberOfParticles( *(pl[i]) );
      is_adjusted = is_adjusted || is_adjusted_now;
    }
  return is_adjusted;
}



/** 
 * This function iterates over all particles and
 * calls AdjustNumberOfParticles(p, prob) for each ot them for
 * prob=1-1/F_REDUCE_FOR_PAIR_PROD, i.e. a particle will be deleted
 * with that probability
 * 
 * @param p  reference to a My_Particles class
 * 
 * @return <b> true </b> is any particle number was adjusted, <b> false
 * </b> otherwise
 */
template<class P>
inline bool LargeParticlesTools::AdjustNumberOfParticlesForPairOverproduction(P& p)
{
  double prob = 1 - 1e0/p.F_REDUCE_FOR_PAIR_PROD;

  if ( prob == 0 ) 
    {
      return false;
    }
  else
    {
      ReduceNumberOfParticles( p, prob );
      return true;
    }
}

/** 
 * This function iterates over all My_Particles in PartriclesList and
 * calls AdjustNumberOfParticlesForPairOverproduction(p) for each ot them 
 * 
 * @param p  reference to a ParticlesList class
 * 
 * @return <b> true </b> is any particle number was adjusted, <b> false
 * </b> otherwise
 */
template<class P>
inline bool LargeParticlesTools::AdjustNumberOfParticlesForPairOverproduction(ParticleList<P>& pl)
{
  bool is_adjusted = false;
  for (int i=0; i<pl.Size(); i++) 
    {
      bool is_adjusted_now = AdjustNumberOfParticlesForPairOverproduction( *(pl[i]) );
      is_adjusted = is_adjusted || is_adjusted_now;
    }
  return is_adjusted;
}



/** 
 * This function reduces the number of particles by the factor 1/prob
 * (on overage). It iterates over individual particles and deletes
 * each particle with probability prob
 * 
 * @param p    reference to a My_Particles class
 * @param prob probability of partricle distruction
 */
template<class P>
void LargeParticlesTools::ReduceNumberOfParticles(P& p, double prob)
{
  // delete excess particles
  double distributed_weight = 0;
  for ( int i=0; i<p.size(); i++ )
    {
      if ( _Rand.Prob(prob) )
	{
	  distributed_weight += p.Weight(i);
	  p.deferredDestroy( Loc<1>(i) );
	}
    }
  p.performDestroy();
  p.Swap();

  //adjust weights of remaining particles
  p.Weight += distributed_weight/p.size();
}


#endif
