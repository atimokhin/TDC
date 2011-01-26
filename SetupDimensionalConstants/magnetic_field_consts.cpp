#include "magnetic_field_consts.h"

#include "../PhysicsLib/small_functions.h"

double MagneticFieldConsts::_B_12  = 0;
double MagneticFieldConsts::_SignB = 0;
double MagneticFieldConsts::_Rcur_6 = 0;

void MagneticFieldConsts::SetupFromConfigGroup(FileInput &in)
{
  // absolute value of B_0 
  _B_12  = abs(in.get_param("B_12"));
  
  // sign of the magnetic field
  _SignB = SF::Sign( in.get_param("B_12") );
  if ( _B_12 == 0 ) _SignB = 0;
  
  // radius of curvarure of magnetic field lines 
  _Rcur_6 = in.get_param("Rcur_6");
}
