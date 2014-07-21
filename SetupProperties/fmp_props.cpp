#include "fmp_props.h"

#include "../SetupDimensionalConstants/norm_consts.h"
#include "../SetupParameters/particles_params.h"
#include "../SetupParameters/geometry_params.h"
#include "../SetupParameters/grid_params.h"


double FMPProps::_W0 = 0;
double FMPProps::_WGJ = 0;


/** 
 * Reads data from the input file and sets pulsar gap parameters
 * 
 */
void FMPProps::Initialize(FileInput &in)
{
  NormConsts      nc;
  ParticlesParams ps;
  GeometryParams  geom;

  // Set physical parameters
  _W0 = geom.L()/static_cast<double>( ps.NGJ() );

  _WGJ = nc.Rho0()/4.8032e-10;
}


void FMPProps::SaveToHDFFile(Save2HDF& hdf) const
{
  hdf.pushLocRel("FMPProps");

  hdf.writeScalar("W0",_W0);
  hdf.writeScalar("WGJ",_WGJ);

  hdf.popLoc();
}

void FMPProps::ReadFromHDFFile(Save2HDF& hdf) 
{
  hdf.pushLocRel("FMPProps");

  hdf.readScalar("W0",_W0);
  hdf.readScalar("WGJ",_WGJ);

  hdf.popLoc();
}




/** 
 * Print FMP params
 * 
 */
std::ostream& FMPProps::Print(std::ostream& s) const
{
  NormConsts     nc;
  GeometryParams geom;
  GridParams     grid;

  std::string separator=string(40,'-')+"\n";

  s<<"\n";
  s<<separator;
  s<<" Fundamental Macro Particle:\n";
  s<<separator;
  s<<" W0 : "<<_W0<<"\n";
  s<<" GJ charge density is provided by -> "<<geom.L()/grid.NumberOfCells()/_W0<<" FMPs in the Cell\n";
  s<<" FMP consists of   -> "<<_WGJ*_W0*nc.X0()<<" physical particles/cm**2\n";
  s<<" GJ number density -> "<<_WGJ<<" 1/cm**3\n";
  s<<separator<<"\n";
  s<<std::flush;

  return s;
}
