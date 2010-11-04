#include "rho_gj_sclf_lin2.h"

#include <algorithm>

#include "../../SetupDimensionalConstants/magnetic_field_consts.h"

RhoGJFunctor_SCLF_lin2::RhoGJFunctor_SCLF_lin2()
{
  MagneticFieldConsts   mf;
  _SignB = mf.SignB();  
}

//! Setup particle boundary conditions from config file group
void RhoGJFunctor_SCLF_lin2::SetupFromConfigGroup(FileInput& in)
{
  _A  = in.get_param("A");
  _X0 = in.get_param("X0");
}

double RhoGJFunctor_SCLF_lin2::operator() (double x) const
{
  return -_SignB*( 1 - _A*std::min(1e0,x/_X0) );
}


