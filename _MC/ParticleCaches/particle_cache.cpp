#include "particle_cache.h"

ParticleCache::ParticleCache(std::string name, int size):
  _Name(name), _N_max(size)
{
  _i = 0;
}

ParticleCache::~ParticleCache()
{};
