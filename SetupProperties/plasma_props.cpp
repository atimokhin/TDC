#include "plasma_props.h"

#include "../SetupDimensionalConstants/norm_consts.h"

#include "../SetupParameters/geometry_params.h"
#include "../SetupParameters/time_params.h"
#include "../_Mesh/my_grid.h"

double PlasmaProps::_OmegaPl = 0;
double PlasmaProps::_Tau = 0;
double PlasmaProps::_LambdaDebye = 0;

/** 
 * Reads data from the input file and sets pulsar gap parameters
 * 
 */
void PlasmaProps::Initialize()
{
  NormConsts     nc;
  GeometryParams geom;

  // Set physical parameters
  _OmegaPl = 2.5741e9 * sqrt( nc.Rho0() );
  
  _Tau = 73.1545/sqrt(nc.Rho0())/nc.X0();

  _LambdaDebye = 11.6429/sqrt(nc.Rho0())/nc.X0();
}


void PlasmaProps::SaveToHDFFile(Save2HDF& hdf) const
{
  hdf.pushLocRel("PlasmaProps");

  hdf.writeScalar("OmegaPl",_OmegaPl);
  hdf.writeScalar("Tau",_Tau);
  hdf.writeScalar("LambdaDebye",_LambdaDebye);

  hdf.popLoc();
}

void PlasmaProps::ReadFromHDFFile(Save2HDF& hdf)
{
  hdf.pushLocRel("GJPlasmaProps");

  hdf.readScalar("OmegaPl",_OmegaPl);
  hdf.readScalar("Tau",_Tau);
  hdf.readScalar("LambdaDebye",_LambdaDebye);

  hdf.popLoc();
}


/** 
 * Print Pulsar Gap parameters
 * 
 */
std::ostream& PlasmaProps::Print(std::ostream& s) const
{
  TimeParams     time;
  My_Grid        grid;

  using namespace std;

  string separator=string(40,'-')+"\n";

  ios_base::fmtflags old_flags=s.flags();

  s<<"\n";
  s<<separator;
  s<<"  Plasma parameters (for GJ density, non-relativistic):\n";
  s<<separator;
  s<<"            Plasma frequency =  "<<scientific<<_OmegaPl<<" rad/sec\n\n";

  s<<" dimensionless period of\n";
  s<<" plasma oscillations  Tau_pl =  "<<_Tau<<"\n";
  s<<"                   dT/Tau_pl -> "<<fixed<<time.dt()/_Tau<<"\n\n";

  s<<" dimensionless          \n";
  s<<" Debye length        Lamda_D =  "<<scientific<<_LambdaDebye<<"\n";
  s<<"                  dX/Lamda_D -> "<<fixed<<grid.dX()/_LambdaDebye<<"\n";

  s<<separator<<"\n";
  s<<flush;

  s.flags(old_flags);

  return s;
}
