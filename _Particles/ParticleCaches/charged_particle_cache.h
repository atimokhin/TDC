#ifndef CHARGED_PARTICLE_CACHE_H
#define CHARGED_PARTICLE_CACHE_H 

#include <iostream> 
#include <string> 
#include <vector>

#include "particle_cache.h"

class ChargedParticleCache: public ParticleCache
{
public:
  ChargedParticleCache(std::string name);

  void Initialize(int n_max);
 
  void Add(double weight, 
           double x, 
           double p_par, double p_perp, 
           char origin,
           int  idts, int id);

  //! Print container content
  virtual void Print(std::ostream& os) const;

public:
  std::vector<double> Weight;
  std::vector<char>   Origin;

  std::vector<double> X;
  std::vector<double> P_par;
  std::vector<double> P_perp;

  std::vector<int> IDTS;
  std::vector<int> ID; 
};


inline void ChargedParticleCache::Add(double weight, 
                                      double x, 
                                      double p_par, double p_perp, 
                                      char origin,
                                      int  idts, int id)
{
  // check size
  ParticleCache::CheckSize();

  Weight[_i] = weight;
  Origin[_i] = origin;

  X[_i] = x;
  P_par[_i]  = p_par;
  P_perp[_i] = p_perp;

  IDTS[_i] = idts;
  ID[_i]   = id; 

  // increment index
  ParticleCache::Add();
}

#endif
