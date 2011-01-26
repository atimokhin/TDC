#include "pulsar_gap_props.h"

#include "../SetupDimensionalConstants/norm_consts.h"
#include "../SetupDimensionalConstants/ns_consts.h"

double PulsarGapProps::_Vvac = 0;
double PulsarGapProps::_Pcf = 0;
double PulsarGapProps::_n_GJ = 0;
double PulsarGapProps::_P = 0;
double PulsarGapProps::_B_12 = 0;



/** 
 * Reads data from the input file and sets pulsar gap parameters
 * 
 */
void PulsarGapProps::Initialize(FileInput &in)
{
  NormConsts nc;
  NSConsts   ns;

  _Vvac = nc.Phi0();
  
  _Pcf = nc.Pcf();

  _n_GJ = nc.Rho0()/4.8032e-10;

  _P = pow( 1.448e4/nc.X0(), 2 ) * pow(ns.RNS_6(), 3);

  _B_12 = _P*_P * _Vvac/(2.198e10 * pow(ns.RNS_6(), 3));
}


void PulsarGapProps::SaveToHDFFile(Save2HDF& hdf) const
{
  hdf.pushLocRel("PulsarGapProps");

  hdf.writeScalar("Vvac",_Vvac);
  hdf.writeScalar("Pcf",_Pcf);
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
  s<<" Vacuum potential drop V_vac = "<<setw(13)<<this->V_vac_Volts()<<" Volts\n\n";

  s<<"          Pcf (e*V_vac/mc^2) = "<<setw(13)<<_Pcf<<"\n\n";

  s<<" GJ particle number density n_GJ = "<<setw(13)<<_n_GJ<<endl;
  s<<"                   Pulsar Period = "<<setw(13)<<_P<<endl;
  s<<"                  Magnetic Field = "<<setw(13)<<_B_12<<" 10^12 [G]"<<endl;
  s<<"\n";
  s<<ns;
  s<<separator<<"\n";
  s<<std::flush;

  return s;
}
