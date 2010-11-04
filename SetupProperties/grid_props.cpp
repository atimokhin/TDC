#include "grid_props.h"

#include "../SetupParameters/geometry_params.h"
#include "../SetupParameters/time_params.h"

#include "../SetupDimensionalConstants/norm_consts.h"

#include "../_Mesh/my_grid.h"

double GridProps::_dT = 0;
int GridProps::_N_Timesteps = 0;
double GridProps::_L = 0;
double GridProps::_L_cm = 0;
double GridProps::_dX = 0;
int GridProps::_NCells = 0;


void GridProps::Initialize()
{
  TimeParams     time;
  GeometryParams geom;
  My_Grid        grid;

  NormConsts     nc;

  _dT = time.dt();
  _N_Timesteps = time.NumberOfSteps();
  _L  = geom.L();
  _dX = grid.dX();
  _NCells = grid.NumberOfCells();  

  _L_cm  = _L * nc.X0();
}


void GridProps::SaveToHDFFile(Save2HDF& hdf) const
{
  hdf.pushLocRel("GridProps");

  hdf.writeScalar("dT",_dT);
  hdf.writeScalar("N_Timesteps",_N_Timesteps);
  hdf.writeScalar("L",_L);
  hdf.writeScalar("L_cm",_L_cm);
  hdf.writeScalar("dX",_dX);
  hdf.writeScalar("NCells",_NCells);

  hdf.popLoc();
}

void GridProps::ReadFromHDFFile(Save2HDF& hdf)
{
  hdf.pushLocRel("GridProps");

  hdf.readScalar("dT",_dT);
  hdf.readScalar("N_Timesteps",_N_Timesteps);
  hdf.readScalar("L",_L);
  hdf.readScalar("L_cm",_L_cm);
  hdf.readScalar("dX",_dX);
  hdf.readScalar("NCells",_NCells);

  hdf.popLoc();
}


ostream& GridProps::Print(ostream& s) const
{
  std::string separator=string(40,'-')+"\n";

  s<<"\n";
  s<<separator;
  s<<" Grid parameters:\n";
  s<<separator;
  s<<" Dimensionless domain length L = "<<std::setw(13)<<_L<<"\n";
  s<<"               domain length l = "<<std::setw(13)<<_L_cm<<" [cm]\n";
  s<<"\n";
  s<<"             dx = "<<std::setw(13)<<_dX<<"\n";
  s<<"     # of cells = "<<std::setw(13)<<_NCells<<std::endl;
  s<<"             dT = "<<std::setw(13)<<_dT<<"\n";
  s<<" # of timesteps = "<<std::setw(13)<<_N_Timesteps<<std::endl;
  s<<"\n";
  s<<" Light cross  the gap in  -> "<<std::setw(11)<<_L/_dT<<" steps\n";
  s<<" Light cross one cell in  -> "<<std::setw(11)<<_dX/_dT<<" steps\n";
  s<<" Anticipated # of flybies -> "<<std::setw(11)<<_dT*_N_Timesteps/_L<<std::endl;;
 
  s<<separator<<"\n";
  s<<std::flush;

  return s;
}
