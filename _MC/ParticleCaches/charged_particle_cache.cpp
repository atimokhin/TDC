#include "charged_aprticle_cache.h"

ChargedParticleCache::ChargedParticleCache(std::string name, int size):
  ParticleCache(name,size)
{
  Weight.resuze(_N_max);
  Origin.resuze(_N_max);

  X.resuze(_N_max);
  P_par.resuze(_N_max);
  P_perp.resuze(_N_max);

  IDTS.resuze(_N_max);
  ID.resuze(_N_max); 
}
