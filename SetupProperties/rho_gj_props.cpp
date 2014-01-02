#include "rho_gj_props.h"

#include "../SetupParameters/grid_params.h"
#include "../SetupParameters/geometry_params.h"
#include "../SetupParameters/particles_params.h"

#include "../_Environment/RhoGJ/rho_gj_maker.h"


double RhoGJProps::_N_Total = 0;


/** 
 * Reads data from the input file and sets pulsar gap parameters
 * 
 */
void RhoGJProps::Initialize(FileInput &in)
{
  GridParams      grid;
  GeometryParams  geom;
  ParticlesParams ps;

  // Set physical parameters
  int n_gj = ps.NGJ();
  int nx   = grid.NumberOfCells();
  double L = geom.L();

  // ********************************************
  // Setup GJ charge density ********************
  // ********************************************
  in.ChangeGroup("RHO_GJ");

  string rhogj_functor_name = in.get_name("Type");
  RhoGJFunctor* p_rhogj = RhoGJFunctorMaker(in).pMake(rhogj_functor_name); 
  p_rhogj->SetupFromConfigGroup(in);

  in.ChangeGroup();
  // ********************************************

  _N_Total=0;
  for (int i=0; i<nx; i++)
    {
      double x = i*L/(nx-1);
      _N_Total += (*p_rhogj)(x);
    }
  _N_Total *= static_cast<double>(n_gj)/nx;

  delete p_rhogj;
}


void RhoGJProps::SaveToHDFFile(Save2HDF& hdf) const
{
  hdf.pushLocRel("RhoGJProps");

  hdf.writeScalar("N_Total",_N_Total);

  hdf.popLoc();
}

void RhoGJProps::ReadFromHDFFile(Save2HDF& hdf) 
{
  hdf.pushLocRel("RhoGJProps");

  hdf.readScalar("N_Total",_N_Total);

  hdf.popLoc();
}




/** 
 * Print FMP params
 * 
 */
std::ostream& RhoGJProps::Print(std::ostream& s) const
{
  std::string separator=string(40,'-')+"\n";

  s<<"\n";
  s<<separator;
  s<<" Goldreich-Julian charge density:\n";
  s<<separator;
  s<<" GJ charge requires -> "<<_N_Total<<" particles \n\n";
  s<<separator<<"\n";
  s<<std::flush;

  return s;
}
