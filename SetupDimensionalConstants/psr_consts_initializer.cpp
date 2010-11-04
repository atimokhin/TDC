#include "psr_consts_initializer.h"

#include "../PhysicsLib/constants.h"
#include "../PhysicsLib/small_functions.h"

   		     
double PSR_ConstsInitializer::_P=1;   
double PSR_ConstsInitializer::_Chi=0;
double PSR_ConstsInitializer::_RNS_6=1;


void PSR_ConstsInitializer::SetupFromConfigGroup(FileInput &in)
{
  // Read parameters from "PULSAR" group *****
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup("PSR_ConstsInitializer");

  //parameters local to this class -----------
  _P     = in.get_param("P_SEC");
  _Chi   = in.get_param("CHI");
  _RNS_6 = in.get_param("RNS_6");
  //------------------------------------------

  // parameters from  MagneticFieldConsts
  _B_12  = abs(in.get_param("B_12"));

  _SignB = SF::Sign( in.get_param("B_12") );
  if ( _B_12 == 0 ) _SignB = 0;

  _Rcur_6 = in.get_param("Rcur_6");
  //------------------------------------------

  in.ChangeGroup(p_current_group);
  // ******************************************


  // Setup normalization constants -----------
  _X0 = 14477.1 * pow(_RNS_6, 1.5)/sqrt(_P);
  _T0 = _X0/Constants::C;

  _Rho0 = 33.3567 * _B_12/_P;  

  _E0   = Constants::PI * _Rho0 * _X0;
  _Phi0 = _E0 * _X0;

  _Pcf  = 1.28855e7  * _B_12/pow(_P,2)   * pow(_RNS_6, 3);
  //------------------------------------------
}




