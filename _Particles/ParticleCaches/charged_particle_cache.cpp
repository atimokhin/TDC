#include "charged_particle_cache.h"

#include <iomanip>

ChargedParticleCache::ChargedParticleCache(std::string name):
  ParticleCache(name)
{}

void ChargedParticleCache::Initialize(int n_max)
{
  ParticleCache::Initialize(n_max);
  
  Weight.resize(_N_max);
  Origin.resize(_N_max);

  X.resize(_N_max);
  P_par.resize(_N_max);
  P_perp.resize(_N_max);

  IDTS.resize(_N_max);
  ID.resize(_N_max); 
}

//! Print container content
void ChargedParticleCache::Print(std::ostream& os) const
{
  ParticleCache::Print(os);
  os<<"Weight = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<Weight[i]<<" ";
  os<<"\n";
  os<<"Origin = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<Origin[i]<<" ";
  os<<"\n";
  os<<"X      = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<X[i]<<" ";
  os<<"\n";
  os<<"P_par  = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<P_par[i]<<" ";
  os<<"\n";
  os<<"P_perp = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<P_perp[i]<<" ";
  os<<"\n";
  os<<"IDTS   = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<IDTS[i]<<" ";
  os<<"\n";
  os<<"ID     = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<ID[i]<<" ";
  os<<"\n";
}
