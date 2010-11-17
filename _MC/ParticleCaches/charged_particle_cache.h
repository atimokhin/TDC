#ifndef CHARGED_PARTICLE_CACHE_H
#define CHARGED_PARTICLE_CACHE_H 

#include <string> 
#include <vector>

#include "particle_cache.h"

class ChargedParticleCache: public ParticleCache
{
public:
  ChargedParticleCache(string name, int size);
  
  void Add(double weight, 
           double x, 
           double p_par, double p_perp, 
           char origin,
           int  idts, int id);

public:
  vector<double> Weight;
  vector<char>   Origin;

  vector<double> X;
  vector<double> P_par;
  vector<double> P_perp;

  vector<int> IDTS;
  vector<int> ID; 
};


inline void ChargedParticleCache::Add(double weight, 
                                      double x, 
                                      double p_par, double p_perp, 
                                      char origin,
                                      int  idts, int id)
{
  // increment index
  ParticleCache::Add();

  Weight[_i] = weight;
  Origin[_i] = origin;

  X[_i] = x;
  P_par[_i]  = p_par;
  P_perp[_i] = p_perp;

  IDTS[_i] = idts;
  ID[_i]   = id; 
}

#endif
