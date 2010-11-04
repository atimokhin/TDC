#include "fmp_props.h"

#include "../SetupDimensionalConstants/norm_consts.h"

#include "../SetupParameters/geometry_params.h"
#include "../SetupParameters/particles_params.h"


double FMPProps::_W0 = 0;
double FMPProps::_WFMP = 0;


/** 
 * Reads data from the input file and sets pulsar gap parameters
 * 
 */
void FMPProps::Initialize()
{
  NormConsts  nc;

  GeometryParams  geom;
  ParticlesParams ps;

  // Set physical parameters
  _W0 = geom.L()/static_cast<double>( ps.NGJ() );

  _WFMP = _W0 * nc.Rho0()*nc.X0()/4.8032e-10;
}


void FMPProps::SaveToHDFFile(Save2HDF& hdf) const
{
  hdf.pushLocRel("FMPProps");

  hdf.writeScalar("W0",_W0);
  hdf.writeScalar("WFMP",_WFMP);

  hdf.popLoc();
}

void FMPProps::ReadFromHDFFile(Save2HDF& hdf) 
{
  hdf.pushLocRel("FMPProps");

  hdf.readScalar("W0",_W0);
  hdf.readScalar("WFMP",_WFMP);

  hdf.popLoc();
}




/** 
 * Print FMP params
 * 
 */
std::ostream& FMPProps::Print(std::ostream& s) const
{
  std::string separator=string(40,'-')+"\n";

  s<<"\n";
  s<<separator;
  s<<" Fundamental Macro Particle:\n";
  s<<separator;
  s<<" FMP consists of -> "<<_WFMP<<" particles \n\n";
  s<<" Normalized FMP charge W0 = "<<_W0<<"\n";
  s<<separator<<"\n";
  s<<std::flush;

  return s;
}
