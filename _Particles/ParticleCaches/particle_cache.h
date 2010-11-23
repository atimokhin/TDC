#ifndef PARTICLE_CACHE_H
#define PARTICLE_CACHE_H

#include <cstdlib>
#include <iostream>
#include <string>


class ParticleCache
{
public:
  ParticleCache(std::string name);
  ~ParticleCache();

  // Set cache size
  void Initialize(int n_max);
  // Current size of the cache
  int  Size() const { return _i; };

  // add particle to the cache : increase counter
  void Add()   { _i++; };
  // clear cache content : set size to 0
  void Clear() { _i=0; };
  // check size and abort when exceeded
  void CheckSize();

  //! Print container content
  virtual void Print(std::ostream& os) const;

protected:
  std::string _Name;
  int _N_max;
  int _i;
};


/**
 * output operator for Particle Cache
 * 
 */
std::ostream& operator<< (std::ostream& o, const ParticleCache& pc);



inline void ParticleCache::CheckSize()
{
  if ( _i >= _N_max )
    {
      std::cout<<"Particle cache for\""<<_Name<<"\" is too small!\n";
      exit(1);
    }
}

#endif
