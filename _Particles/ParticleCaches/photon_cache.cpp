#include "photon_cache.h"

PhotonCache::PhotonCache():
  ParticleCache("Photons")
{}

void PhotonCache::Initialize(int n_max)
{
  ParticleCache::Initialize(n_max);

  Weight.resize(_N_max);
  Origin.resize(_N_max);

  Psi0.resize(_N_max);
  E.resize(_N_max);
}

//! Save photons to an HDF file
void PhotonCache::Save2HDFFile(Save2HDF &hdf) 
{
  hdf.SaveSTLContainer("Weight",Weight);
  hdf.SaveSTLContainer("Psi0",Psi0);
  hdf.SaveSTLContainer("E",E);

  hdf.writeScalar("N",_i);
  hdf.writeScalar("X0",_X0);
  hdf.writeScalar("Direction",static_cast<int>(_Dir));

  hdf.SaveSTLContainer("Origin",Origin);
}

//! Print PhotonCache content
void PhotonCache::Print(std::ostream& os) const
{
  ParticleCache::Print(os);
  os<<" X0="<<_X0<<" T0="<<_T0<<" Direction="<<_Dir<<std::endl;
  os<<" IDTS="<<_IDTS<<" ID="<<_ID<<std::endl;
  for ( int i=0; i<Size(); i++)
    {
      os<<"# "<<i<<" Weight="<<Weight[i]<<" Psi0="<<Psi0[i]<<" E="<<E[i];
      os<<" Origin="<<Origin[i]<<std::endl;
    }
  os<<std::endl;
}
