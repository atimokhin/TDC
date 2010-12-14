#include "pair_cache.h"

#include <iomanip>

PairCache::PairCache():
  ParticleCache("Pairs")
{}

void PairCache::Initialize(int n_max)
{
  ParticleCache::Initialize(n_max);

  Weight.resize(_N_max);
  Origin.resize(_N_max);

  T_cr.resize(_N_max);
  X_cr.resize(_N_max);
  X_em.resize(_N_max);
  E.resize(_N_max);
  Psi.resize(_N_max);

  IDTS.resize(_N_max);
  ID.resize(_N_max); 
  IDTS_Parent.resize(_N_max);
  ID_Parent.resize(_N_max);
}

//! Save photons to an HDF file
void PairCache::Save2HDFFile(Save2HDF &hdf) 
{
  ParticleCache::Save2HDFFile(hdf);

  hdf.SaveSTLContainer("Weight",Weight);
  hdf.SaveSTLContainer("Origin",Origin);

  hdf.SaveSTLContainer("T_cr",T_cr);
  hdf.SaveSTLContainer("X_cr",X_cr);
  hdf.SaveSTLContainer("X_em",X_em);
  hdf.SaveSTLContainer("E",E);
  hdf.SaveSTLContainer("Psi",Psi);

  hdf.SaveSTLContainer("IDTS",IDTS);
  hdf.SaveSTLContainer("ID",ID);
  hdf.SaveSTLContainer("IDTS_Parent",IDTS_Parent);
  hdf.SaveSTLContainer("ID_Parent",ID_Parent);
}


//! Print container content
void PairCache::Print(std::ostream& os) const
{
  ParticleCache::Print(os);  
  os<<"Weight = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<Weight[i]<<" ";
  os<<"\n";
  os<<"Origin = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<Origin[i]<<" ";
  os<<"\n";
  os<<"T_cr   = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<T_cr[i]<<" ";
  os<<"\n";
  os<<"X_cr   = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<X_cr[i]<<" ";
  os<<"\n";
  os<<"X_em   = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<X_em[i]<<" ";
  os<<"\n";
  os<<"E      = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<E[i]<<" ";
  os<<"\n";
  os<<"Psi    = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<Psi[i]<<" ";
  os<<"\n";
  os<<"IDTS   = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<IDTS[i]<<" ";
  os<<"\n";
  os<<"ID     = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<ID[i]<<" ";
  os<<"\n";
  os<<"IDTS_P = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<IDTS_Parent[i]<<" ";
  os<<"\n";
  os<<"ID_P   = ";
  for (int i=0; i<Size(); i++) os<<std::setw(5)<<ID_Parent[i]<<" ";
  os<<"\n";
}
