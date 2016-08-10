#include "rho_gj_OG_lin.h"

//! Setup particle boundary conditions from config file group
void RhoGJFunctor_OG_lin::SetupFromConfigGroup(FileInput& in)
{
  _RHO_0 = in.get_param("Rho0");
  _X0 = in.get_param("X0");
}

double RhoGJFunctor_OG_lin::operator() (double x) const
{
  return _RHO_0*( 1 - x/_X0 );
}

