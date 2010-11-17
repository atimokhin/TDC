#include "pair_cache.h"

PairCache::PairCache(std::string name, int size):
  ParticleCache(name,size)
{
  Weight.resuze(_N_max);
  Origin.resuze(_N_max);

  T_cr.resuze(_N_max);
  X_cr.resuze(_N_max);
  X_em.resuze(_N_max);
  E.resuze(_N_max);
  Psi.resuze(_N_max);

  IDTS.resuze(_N_max);
  ID.resuze(_N_max); 
  IDTS_Parent.resuze(_N_max);
  ID_Parent.resuze(_N_max);
}
