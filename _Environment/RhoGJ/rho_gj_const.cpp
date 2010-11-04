#include "rho_gj_const.h"

#include "../../SetupDimensionalConstants/magnetic_field_consts.h"

RhoGJFunctor_Const::RhoGJFunctor_Const()
{
  MagneticFieldConsts   mf;
  _SignB = mf.SignB();
}
