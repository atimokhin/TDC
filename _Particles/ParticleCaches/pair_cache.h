#ifndef PAIR_CACHE
#define PAIR_CACHE 

#include <iostream> 
#include <string> 
#include <vector> 

#include "particle_cache.h"

class PairCache: public ParticleCache
{
public:
  PairCache();

  void Initialize(int n_max);

  void Add(double weight, 
           double t_cr, double x_cr, double x_em, 
           double e, double psi, 
           char origin, 
           int idts_parent, int id_parent, int idts, int id );

  virtual void Print(std::ostream& os) const;

public:
  std::vector<double> Weight;
  std::vector<char>   Origin;

  std::vector<double> T_cr;
  std::vector<double> X_cr;
  std::vector<double> X_em;
  std::vector<double> E;
  std::vector<double> Psi;

  std::vector<int> IDTS;
  std::vector<int> ID; 
  std::vector<int> IDTS_Parent;
  std::vector<int> ID_Parent;
};


inline void PairCache::Add(double weight, 
                           double t_cr, double x_cr, double x_em, 
                           double e, double psi, 
                           char origin, 
                           int idts_parent, int id_parent, int idts, int id )
{
  // check size
  ParticleCache::CheckSize();

  Weight[_i] = weight;
  Origin[_i] = origin;

  T_cr[_i] = t_cr;
  X_cr[_i] = x_cr;
  X_em[_i] = x_em;
  E[_i]    = e;
  Psi[_i]  = psi;

  IDTS[_i] = idts;
  ID[_i]   = id; 
  IDTS_Parent[_i] = idts_parent;
  ID_Parent[_i]   = id_parent; 

  // increment index
  ParticleCache::Add();
}

#endif
