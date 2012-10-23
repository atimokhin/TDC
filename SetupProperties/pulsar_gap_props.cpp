#include "pulsar_gap_props.h"

#include "../SetupDimensionalConstants/norm_consts.h"
#include "../SetupDimensionalConstants/magnetic_field_consts.h"
#include "../SetupDimensionalConstants/ns_consts.h"

#include "../SetupParameters/geometry_params.h"

double PulsarGapProps::_Vvac = 0;
double PulsarGapProps::_Pcf = 0;
double PulsarGapProps::_Vvac_L = 0;
double PulsarGapProps::_Pcf_L = 0;
double PulsarGapProps::_n_GJ = 0;
double PulsarGapProps::_P = 0;
double PulsarGapProps::_Rpc = 0;
double PulsarGapProps::_B_12 = 0;



/** 
 * Reads data from the input file and sets pulsar gap parameters
 * 
 */
void PulsarGapProps::Initialize(FileInput &in)
{
  NormConsts nc;
  NSConsts   ns;
  MagneticFieldConsts mf;

  GeometryParams  geom;

  _Vvac = nc.Phi0();
  
  _Pcf = nc.Pcf();

  _n_GJ = nc.Rho0()/4.8032e-10;

  _P = 33.3667*mf.B_12()/nc.Rho0();

  _Rpc = nc.X0();

  _B_12 = mf.B_12();

  // potential drop over the whole domain:
  _Pcf_L  = _Pcf  * 2*pow(geom.L(),2);
  _Vvac_L = _Vvac * 2*pow(geom.L(),2);
}


void PulsarGapProps::SaveToHDFFile(Save2HDF& hdf) const
{
  hdf.pushLocRel("PulsarGapProps");

  hdf.writeScalar("Vvac",_Vvac);
  hdf.writeScalar("Pcf",_Pcf);
  hdf.writeScalar("Vvac_L",_Vvac);
  hdf.writeScalar("Pcf_L",_Pcf);
  hdf.writeScalar("n_GJ",_n_GJ);
  hdf.writeScalar("P",_P);
  hdf.writeScalar("B_12",_B_12);

  hdf.popLoc();
}

void PulsarGapProps::ReadFromHDFFile(Save2HDF& hdf)
{
  hdf.pushLocRel("PulsarGapProps");

  hdf.readScalar("Vvac",_Vvac);
  hdf.readScalar("Pcf",_Pcf);
  hdf.readScalar("Vvac_L",_Vvac);
  hdf.readScalar("Pcf_L",_Pcf);
  hdf.readScalar("n_GJ",_n_GJ);
  hdf.readScalar("P",_P);
  hdf.readScalar("B_12",_B_12);

  hdf.popLoc();
}


/** 
 * Print Pulsar Gap parameters
 * 
 */
ostream& PulsarGapProps::Print(ostream& s) const
{
  NSConsts   ns;
  using namespace std;
  string separator=string(40,'-')+"\n";

  s<<"\n";
  s<<separator;
  s<<" Pulsar Gap parameters:\n";
  s<<separator;

  s<<"Pulsar:\n";
  s<<" Period = "<<setw(13)<<_P<<" sec\n";
  s<<"    Rpc = "<<setw(13)<<_Rpc<<" cm \n\n";

  s<<"Over L:\n";
  s<<" Vacuum potential drop V_vac = "<<setw(13)<<this->V_vac_L_Volts()<<" Volts\n";
  s<<"                         Pcf = "<<setw(13)<<_Pcf_L<<" (e*V_vac/mc^2) \n\n";

  s<<"Over X0:\n";
  s<<" Vacuum potential drop V_vac = "<<setw(13)<<this->V_vac_Volts()<<" Volts\n";
  s<<"                         Pcf = "<<setw(13)<<_Pcf<<" (e*V_vac/mc^2) \n\n";

  s<<" GJ particle number density n_GJ = "<<setw(13)<<_n_GJ<<endl;
  s<<"                   Pulsar Period = "<<setw(13)<<_P<<endl;
  s<<"                  Magnetic Field = "<<setw(13)<<_B_12<<" 10^12 [G]"<<endl;
  s<<"\n";
  s<<ns;
  s<<separator<<"\n";
  s<<std::flush;

  return s;
}
