#include "x0__lD_GJ_consts_initializer.h"

#include "../PhysicsLib/constants.h"
#include "../PhysicsLib/small_functions.h"

   		     

void X0__lD_GJ_ConstsInitializer::SetupFromConfigGroup(FileInput &in)
{
  // Read parameters from "PULSAR" group *****
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup("X0__lD_GJ_ConstsInitializer");

  // parameters from  MagneticFieldConsts
  _B_12  = abs(in.get_param("B_12"));

  _SignB = SF::Sign( in.get_param("B_12") );
  if ( _B_12 == 0 ) _SignB = 0;

  _Rcur_6 = in.get_param("Rcur_6");
  //------------------------------------------

  in.ChangeGroup(p_current_group);
  // ******************************************


  // Setup normalization constants -----------
  _X0  = 1e0;
  _Pcf = 0.25;

  _T0 = _X0/Constants::C;

  _Phi0 = 1704.49 * _Pcf;
  _E0   = _Phi0/_X0;
  _Rho0 = _E0/( Constants::PI * _X0 );  
  //------------------------------------------
}
