#include "GammaB.h"

#include "../../SetupDimensionalConstants/norm_consts.h"

GammaB::GammaB():
  _KHI_COEFF(117.705)
{
  NormConsts nc;
  _Agb = 9.847e5 * nc.X0();
}

void GammaB::SetMagneticField(MagneticField* p_mf)
{
  _pMF = p_mf;
}

