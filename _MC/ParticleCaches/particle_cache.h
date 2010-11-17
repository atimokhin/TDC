#ifndef PARTICLE_CACHE_H
#ifndef PARTICLE_CACHE_H

#include <iostream>
#include <string>

class ParticleCache
{
public:
  ParticleCache(name,size);
  ~ParticleCache();

  // add particle to the cache : increase counter
  void Add();

  // Current size of the cache
  int  Size() const {return _i; };

  // clear cache content : set size to 0
  void Clear() { _i=0; };

protected:
  const std::string _Name;
  const int         _N_max;
  int _i;
};


inline void ParticleCache::Add()
{
  if ( ++_i >= _N_max )
    {
      std::cout<<"Particle cache for\""<<_Name<<"\" is too small!\n";
      exit(1);
    }
}

#endif
