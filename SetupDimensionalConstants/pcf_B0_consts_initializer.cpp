#include "pcf_B0_consts_initializer.h"

#include "../PhysicsLib/constants.h"

   		     
void PcfB0_ConstsInitializer::SetupFromConfigGroup(FileInput &in)
{
  // Read parameters from "PULSAR" group *****
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup("PcfB0_ConstsInitializer");

  //parameters local to this class -----------
  _Pcf = in.get_param("Pcf");
  //------------------------------------------

  // parameters from  MagneticFieldConsts
  MagneticFieldConsts::SetupFromConfigGroup(in);

  // parameters from  NSConsts
  NSConsts::SetupFromConfigGroup(in);

  in.ChangeGroup(p_current_group);
  // ******************************************


  // Setup normalization constants -----------
  // pulsar period - derive from magnetic field stegth and voltage
  double Period = sqrt( 1.29e7 * pow(_RNS_6, 3) * _B_12/_Pcf );

  _X0 = 1.448e4 * pow(_RNS_6, 1.5)/sqrt(Period);
  _T0 = _X0/Constants::C;

  _Phi0 = 1705 * _Pcf;
  _E0   = _Phi0/_X0;
  _Rho0 = _E0/( Constants::PI * _X0 );  
  //------------------------------------------
}




