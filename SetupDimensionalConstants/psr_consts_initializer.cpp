#include "psr_consts_initializer.h"

#include "../PhysicsLib/constants.h"
#include "../PhysicsLib/small_functions.h"

   		     
double PSR_ConstsInitializer::_P=1;   
double PSR_ConstsInitializer::_Chi=0;


void PSR_ConstsInitializer::SetupFromConfigGroup(FileInput &in)
{
  // Read parameters from "PULSAR" group *****
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup("PSR_ConstsInitializer");

  //parameters local to this class -----------
  _P     = in.get_param("P_SEC");
  _Chi   = 1.745329e-2 * in.get_param("CHI"); // degrees to radians conversion
  //------------------------------------------

  // parameters from  MagneticFieldConsts
  MagneticFieldConsts::SetupFromConfigGroup(in);

  // parameters from  NSConsts
  NSConsts::SetupFromConfigGroup(in);

  in.ChangeGroup(p_current_group);
  // ******************************************


  // Setup normalization constants -----------
  _X0 = 1.448e4 * pow(_RNS_6, 1.5)/sqrt(_P);
  _T0 = _X0/Constants::C;

  _Rho0 = 33.3667 * _B_12*cos(_Chi)/_P;  

  _E0   = Constants::PI * _Rho0 * _X0;
  _Phi0 = _E0 * _X0;

  _Pcf  = _Phi0/1705e0;
  //------------------------------------------
}




