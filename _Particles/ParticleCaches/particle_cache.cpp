#include "particle_cache.h"

ParticleCache::ParticleCache(std::string name):
  _Name(name)
{
  _i = 0;
}

ParticleCache::~ParticleCache()
{};

// Set cache size
void ParticleCache::Initialize(int n_max) 
{ 
  _N_max=n_max; 
};

//! Print container content
void ParticleCache::Print(std::ostream& os) const
{
  os<<"Cache: \""<<_Name<<"\"\n";
  os<<"  size     max: "<<_N_max<<"\n";
  os<<"  size currect: "<<Size()<<"\n";
}


std::ostream& operator<< (std::ostream& o, const ParticleCache& pc)
{
  pc.Print(o);
  return o;
}
