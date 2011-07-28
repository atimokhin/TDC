#include "rho_gj_sclf_exp2.h"

#include <algorithm>

#include "../../SetupDimensionalConstants/magnetic_field_consts.h"

RhoGJFunctor_SCLF_exp2::RhoGJFunctor_SCLF_exp2()
{
  MagneticFieldConsts   mf;
  _SignB = mf.SignB();  
}

//! Setup particle boundary conditions from config file group
void RhoGJFunctor_SCLF_exp2::SetupFromConfigGroup(FileInput& in)
{
  _A1 = in.get_param("A1");
  _A2 = in.get_param("A2");
  _X0 = in.get_param("X0");
}

double RhoGJFunctor_SCLF_exp2::operator() (double x) const
{
  return -_SignB*( _A1 - _A2*(1e0-exp(-x/_X0) ));
}


