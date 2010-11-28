#include "RS_cascade_props.h"

#include "../SetupDimensionalConstants/psr_consts_initializer.h"
#include "../SetupDimensionalConstants/norm_consts.h"
#include "../SetupDimensionalConstants/magnetic_field_consts.h"


double RSCascadeProps::_H;   
double RSCascadeProps::_H_cm;   
double RSCascadeProps::_V;   
double RSCascadeProps::_V_volts;   
double RSCascadeProps::_GammaMax;


/** 
 * Sets Ruderman-Sutherland cascade parameters
 * 
 */
void RSCascadeProps::Initialize(FileInput &in)
{
  MagneticFieldConsts   mf; 

  PSR_ConstsInitializer psr;
  NormConsts            nc;

  _H_cm = 5e3 * pow( psr.B_12(), -4./7.) * pow( psr.P(), 3./7.) * pow( mf.Rcur_6(), 2./7. );
  _H    = _H_cm / nc.X0();

  _V       = 2 * pow(_H,2);
  _V_volts = _V * nc.Phi0() * 299.7;

  _GammaMax = nc.Pcf() * _V;
}


void RSCascadeProps::SaveToHDFFile(Save2HDF& hdf) const
{
  hdf.pushLocRel("RSCascadeProps");

  hdf.writeScalar("H",_H);
  hdf.writeScalar("H_cm",_H_cm);
  hdf.writeScalar("V",_V);
  hdf.writeScalar("V_volts",_V_volts);
  hdf.writeScalar("GammaMax",_GammaMax);

  hdf.popLoc();
}

void RSCascadeProps::ReadFromHDFFile(Save2HDF& hdf)
{
  hdf.pushLocRel("RSCascadeProps");

  hdf.readScalar("H",_H);
  hdf.readScalar("H_cm",_H_cm);
  hdf.readScalar("V",_V);
  hdf.readScalar("V_volts",_V_volts);
  hdf.readScalar("GammaMax",_GammaMax);

  hdf.popLoc();
}


/** 
 * Print Ruderman-Sutherland cascade parameters
 * 
 */
ostream& RSCascadeProps::Print(ostream& s) const
{
  using namespace std;
  string separator=string(40,'-')+"\n";

  s<<"\n";
  s<<separator;
  s<<" Ruderman-Sutherland cascade parameters:\n";
  s<<separator;
  s<<"                 gap height h_RS = "<<setw(13)<<this->H_cm()<<" [cm]\n";
  s<<"             Potential drop v_RS = "<<setw(13)<<this->V_volts()<<" [V]\n";
  s<<"\n";
  s<<"   Dimensionless gap height H_RS = "<<setw(13)<<this->H()<<"\n";
  s<<"             Potential drop V_RS = "<<setw(13)<<this->V()<<" [V_vac]\n";
  s<<" Maximum Lorentz factor GammaMax = "<<setw(13)<<this->GammaMax()<<endl;
  s<<separator<<"\n";
  s<<std::flush;

  return s;
}
