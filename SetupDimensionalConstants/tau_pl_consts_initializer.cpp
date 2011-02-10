#include "tau_pl_consts_initializer.h"

#include "../PhysicsLib/constants.h"

void TauPl_ConstsInitializer::SetupFromConfigGroup(FileInput &in)
{
  // Read parameters from "PULSAR" group *****
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup("TauPl_ConstsInitializer");

  //parameters local to this class -----------
  _T0 = in.get_param("T0");
  //------------------------------------------

  // parameters from  MagneticFieldConsts
  MagneticFieldConsts::SetupFromConfigGroup(in);

  in.ChangeGroup(p_current_group);
  // ******************************************


  // Setup normalization constants -----------
  _X0 = Constants::C * _T0;

  _Rho0 = pow(2.4409e-9/_T0, 2);

  _E0   = Constants::PI * _Rho0 * _X0;
  _Phi0 = _E0 * _X0;

  _Pcf  = _Phi0/1705e0;
  //------------------------------------------
}




