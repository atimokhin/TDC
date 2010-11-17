#ifndef PAIR_CACHE
#define PAIR_CACHE 

#include <string> 
#include <vector> 

#include "particle_cache.h"

class PairCache: public ParticleCache
{
public:
  PairCache(string name, int size);

  void Add(double weight, 
           double t_cr, double x_cr, double x_em, 
           double e, double psi, 
           char origin, 
           int idts_parent, int id_parent, int idts, int id );

public:
  vector<double> Weight;
  vector<char>   Origin;

  vector<double> T_cr;
  vector<double> X_cr;
  vector<double> X_em;
  vector<double> E;
  vector<double> Psi;

  vector<int> IDTS;
  vector<int> ID; 
  vector<int> IDTS_Parent;
  vector<int> ID_Parent;
};


inline void PairCache::Add(double weight, 
                           double t_cr, double x_cr, double x_em, 
                           double e, double psi, 
                           char origin, 
                           int idts_parent, int id_parent, int idts, int id )
{
  // increment index
  ParticleCache::Add();

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
}

#endif
