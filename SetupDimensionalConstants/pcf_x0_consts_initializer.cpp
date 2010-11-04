#include "pcf_x0_consts_initializer.h"

#include "../PhysicsLib/constants.h"
#include "../PhysicsLib/small_functions.h"

   		     

void PcfX0_ConstsInitializer::SetupFromConfigGroup(FileInput &in)
{
  // Read parameters from "PULSAR" group *****
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup("PcfX0_ConstsInitializer");

  _Pcf = in.get_param("Pcf");
  _X0  = in.get_param("X0");

  // parameters from  MagneticFieldConsts
  _B_12  = abs(in.get_param("B_12"));

  _SignB = SF::Sign( in.get_param("B_12") );
  if ( _B_12 == 0 ) _SignB = 0;

  _Rcur_6 = in.get_param("Rcur_6");
  //------------------------------------------

  in.ChangeGroup(p_current_group);
  // ******************************************


  // Setup normalization constants -----------
  _T0 = _X0/Constants::C;

  _Phi0 = 1704.49 * _Pcf;
  _E0   = _Phi0/_X0;
  _Rho0 = _E0/( Constants::PI * _X0 );  
  //------------------------------------------
}




