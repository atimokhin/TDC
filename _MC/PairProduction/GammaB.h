#ifndef GAMMA_B_H
#define GAMMA_B_H

#include "../../_Environment/MagneticField/magnetic_field.h"

/**
 * \ingroup pair_creation_grp 
 *
 * \class GammaB
 * Class responsible for calculation of \f$ \gamma - B \f$ attenuation
 * coefficients (cross-sections)
 *
 */
class GammaB
{
public:
  //! Constructor
  GammaB();

  //! Set pointer to the MagneticField functor
  void SetMagneticField(MagneticField* p_mf);

  //! cross-section as function of x0 and l
  double Sigma_x0_l(double e, double x0, double l) const;
  //! cross-section as function of x and psi
  double Sigma_x_psi(double e, double x, double psi) const;

  //! maximum cross-section for photons in direction d
  double SigmaMax(double e, double x0, Direction d) const;

private:
  //! internal function for cross-section
  double Sigma_EphBperp(double e, double b_perp) const; 

private:
  //! magnetic field functor
  MagneticField* _pMF;
  //! coefficients for cross-section calculation
  double _Agb;
  const double _KHI_COEFF;
};



inline double GammaB::Sigma_x_psi(double e, double x, double psi) const
{
  return Sigma_EphBperp(e, _pMF->Bperp_x_psi(x,psi) );
}

inline double GammaB::Sigma_x0_l(double e, double x0, double l) const
{
  return Sigma_EphBperp(e, _pMF->Bperp_x0_l(x0,l) );
}

inline double GammaB::SigmaMax(double e, double x0, Direction d) const
{
  return Sigma_EphBperp(e, _pMF->BperpMax(x0,d) );
}

/** 
 * Cross-section for photon absorption in strong magnetic field
 * 
 * @param e       energy of the photons in units of  \f$ m_e c^2  \f$
 * @param b_perp  perperdicular magnetic field in units of \f$ 10^{12} \f$ Gauss
 * 
 * @return  \f$ \sigma_{\gamma{}B} \f$
 */

inline double GammaB::Sigma_EphBperp(double e, double b_perp) const
{
  return _Agb * b_perp * exp( - _KHI_COEFF/(e*b_perp) );
}

#endif





