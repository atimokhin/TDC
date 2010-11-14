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

  //! Adjust number of Pairs
  template<class P>
  bool AdjustNumberOfPairs(P& p);

  //! reduce number fo particle - the actual merging function
  template<class P> void MergeParticles(P& p);
  //! increase number fo particle - the actual splitting function
  template<class P> void SplitParticles(P& p);

  //! increase number fo pairs - the actual splitting function
  template<class P> void SplitPairs(P& p);

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
  p.LptData.n_steps_since_last_splitting++;

  if ( p.size() < p.LptData.n_min ) 
    {
      if ( p.LptData.f_splitted_last_time > p.LptData.split_f_splitted_min ||
           p.LptData.n_steps_since_last_splitting > p.LptData.split_n_steps_min )
        {      
          SplitParticles(p);
          return true;
        }
      else
        return false;
    }
  else if ( p.size() > p.LptData.n_max ) 
    {
      MergeParticles(p);
      return true;
    }
  else
    {
      return false;
    }    
}

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
inline bool LargeParticlesTools::AdjustNumberOfPairs(P& p)
{
  p.LptData.n_steps_since_last_splitting++;

  if ( p.size() < p.LptData.n_min ) 
    {
      if ( p.LptData.f_splitted_last_time > p.LptData.split_f_splitted_min ||
           p.LptData.n_steps_since_last_splitting > p.LptData.split_n_steps_min )
        {      
          SplitPairs(p);
          return true;
        }
      else
        return false;
    }
  else if ( p.size() > p.LptData.n_max ) 
    {
      MergeParticles(p);
      return true;
    }
  else
    {
      return false;
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
 * This function reduces the number of particles by the factor 1/prob
 * (on overage). It iterates over individual particles and deletes
 * each particle with probability prob
 * 
 * @param p    reference to a My_Particles class
 * @param prob probability of partricle distruction
 */
template<class P>
void LargeParticlesTools::MergeParticles(P& p)
{
  // delete excess particles
  double distributed_weight = 0;
  for ( int i=0; i<p.size(); i++ )
    {
      if ( _Rand.Prob(p.LptData.merge_f_reduce) )
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

/** 
 * This function splits each particle with theweight larger than a
 * certaind minimum weight weight_min in two particles
 * each having half of the original weight and coordinates x+/-dx
 *
 * 
 * @param p    reference to a My_Particles class
 */
template<class P>
void LargeParticlesTools::SplitParticles(P& p)
{
  int n_particles  = p.size();
  int n_unsplitted = 0;
  p.create(n_particles);
  for ( int i=0; i<n_particles; i++ )
    {
      int i2 = i+n_particles;
      if ( p.Weight(i) > p.LptData.split_min_weight )
	{
	  p.Weight(i2) = 0.5*p.Weight(i); 
          p.X(i2)(0)    = p.X(i)(0)+p.LptData.split_dx; 
          p.P_par(i2)  = p.P_par(i);
          p.P_perp(i2) = p.P_perp(i); 
          p.Origin(i2) = p.Origin(i);
          p.IDTS(i2)   = p.IDTS(i); 
          p.ID(i2)     = p.ID(i);

          p.Weight(i) *= 0.5;
          p.X(i)(0)   -= p.LptData.split_dx;
	}
      else
        {
          n_unsplitted++;
	  p.deferredDestroy( Loc<1>(i2) );
        }
    }
  p.performDestroy();
  p.Swap();

  p.LptData.f_splitted_last_time = static_cast<double>(n_unsplitted)/n_particles;
  p.LptData.n_steps_since_last_splitting = 0;
}

/** 
 * This function splits each particle with theweight larger than a
 * certaind minimum weight weight_min in two particles
 * each having half of the original weight and coordinates x+/-dx
 *
 * 
 * @param p    reference to a My_Particles class
 */
template<class P>
void LargeParticlesTools::SplitPairs(P& p)
{
  int n_particles  = p.size();
  int n_unsplitted = 0;
  for ( int i=0; i<n_particles; i++ )
    {
      if ( p.Weight(i) > p.LptData.split_min_weight )
	{
          p.AddPair(0.5*p.Weight(i), 
                    p.T_cr(i), p.X_cr(i)(0)+p.LptData.split_dx,
                    p.X_em(i)(0), 
                    p.E(i), p.Psi(i), 
                    p.Origin(i),
                    p.IDTS_Parent(i), p.ID_Parent(i),
                    p.IDTS(i), p.ID(i));
          p.Weight(i) *= 0.5;
          p.X_cr(i) -= p.LptData.split_dx;
	}
      else
        {
          n_unsplitted++;
        }
    }
  p.Swap();

  p.LptData.f_splitted_last_time = static_cast<double>(n_unsplitted)/n_particles;
  p.LptData.n_steps_since_last_splitting = 0;
}


#endif
