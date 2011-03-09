#include "x0_LambdaD_consts_initializer.h"

#include "../PhysicsLib/constants.h"

   		     
void X0_LambdaD_ConstsInitializer::SetupFromConfigGroup(FileInput &in)
{
  // Read parameters from "PULSAR" group *****
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup("X0_LambdaD_ConstsInitializer");

  //parameters local to this class -----------
  _X0 = in.get_param("X0");
  //------------------------------------------

  // parameters from  MagneticFieldConsts
  MagneticFieldConsts::SetupFromConfigGroup(in);

  // parameters from  NSConsts
  NSConsts::SetupFromConfigGroup(in);

  in.ChangeGroup(p_current_group);
  // ******************************************


  // Setup normalization constants -----------
  _T0 = _X0/Constants::C;

  _Rho0 = pow(11.6429/_X0, 2);  

  _Phi0 = 4.259e2;
  _E0   = Constants::PI * _Rho0 * _X0;
  _Pcf  = _Phi0/1705;  
  //------------------------------------------
}




