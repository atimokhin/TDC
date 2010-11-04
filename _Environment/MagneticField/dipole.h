#ifndef DIPOLE_H
#define DIPOLE_H

#include <cmath>

#include "magnetic_field.h"


/**
 * \ingroup magnetic_field_grp
 *
 * \class  Dipole
 *
 * Dipolar magnetic field
 *
 * This class provides all information about pulsar magnetic field
 * used in the cascade calculations:
 * - Goldreich-Julian Charge density
 * - Radius of curvature of the magnetic field lines
 * - strength of the perpendicular component of the magntic field propagating photon sees
 *
 * <I> it requires the following input parameters and constants: </I>
 * - GeometryParams
 * - MagneticFieldConsts
 * - NormConsts
 *
 * configuration file example
 \verbatim

Group "MAGNETIC_FIELD" {
   Type = 'Dipole';
}

 \endverbatim
 */
class Dipole: public MagneticField
{
public:

  //! constructor
  Dipole();

  //! Radius of magnetic field line curvature
  double Rcur(double x ) const;

  //! Angle between photon momentum and magnetic field
  double Psi( double x0, double l ) const;

  //! Perpendicular component of the magnetic field \f$ B_\perp/B_q \f$
  double Bperp_x0_l( double x0, double l ) const;
  //! Perpendicular component of the magnetic field \f$ B_\perp/B_q \f$
  double Bperp_x_psi( double x, double psi ) const;
  //! Maximum value of \f$ B_\perp/B_q \f$
  double BperpMax( double x0, Direction d ) const;

  //! Maximum value of \f$ B_\perp/B_q \f$
  double XMax( double x0, Direction d ) const;


  //! colatitude of the polar cap edge at the stellar surface
  double ThetaPC() const { return _Theta_PC; };

private:

  double _Theta_PC;    //! colatitude of the polar cap edge at the stellar surface
  
  double _SignB;       //!< Sign of the magnetic field (+1|0|-1)
  
  double _Psi_Coeff;   //!< coeffcient in expression for psi
  double _Rcur_Coeff;  //!< coefficient in formula for the radius of curvature
  
  double  _B0;           //!< coefficient for Bperp_x_psi
  double  _B0_Psi_Coeff; //!< coefficient for Bperp_x0_l
  double  _B_MAX_1;      //!< coefficient for BperpMax

  double  _L;          //!< length of the system
  double  _X_MIN;      //!< critical height of photon emission

  double  _Alpha;      //!< \f$ x_0/R_{\mbox{NS}} \f$
};

#endif
