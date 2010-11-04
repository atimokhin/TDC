#include "photons.h"


void Photons::Initialize(int n_max)
{
  Weight.resize(n_max);
  Psi0.resize(n_max);
  E.resize(n_max);

  Origin.resize(n_max);

  _N=0;
}



//! Save photons to an HDF file
void Photons::Save2HDFFile(Save2HDF &hdf) 
{
  hdf.SaveSTLContainer("Weight",Weight);
  hdf.SaveSTLContainer("Psi0",Psi0);
  hdf.SaveSTLContainer("E",E);

  hdf.writeScalar("N",_N);
  hdf.writeScalar("X0",_X0);
  hdf.writeScalar("Direction",static_cast<int>(_Dir));

  hdf.SaveSTLContainer("Origin",Origin);
}


//! Print container content
void Photons::Print(std::ostream& os) const
{
  os<<"Photons: max size="<<Weight.size();
  os<<" size="<<_N<<" X0="<<_X0<<" T0="<<_T0<<" Direction="<<_Dir;
  os<<" IDTS="<<_IDTS<<" ID="<<_ID<<std::endl;
  for ( int i=0; i<_N; i++)
    {
      os<<"# "<<i<<" Weight="<<Weight[i]<<" Psi0="<<Psi0[i]<<" E="<<E[i];
      os<<" Origin="<<Origin[i]<<std::endl;
    }
  os<<std::endl;
}


std::ostream& operator<< (std::ostream& o, const Photons& ph)
{
  ph.Print(o);
  return o;
}
