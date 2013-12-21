#ifndef R6_H
#define R6_H

#include <cmath>
#include <iostream>

#include "magnetic_field.h"


/**
 * \ingroup magnetic_field_grp
 *
 * \class  R6
 *
 * Constant magnetic field with constant (small) radius of curvature of magnetic field line
 *
 * This class provides all information about pulsar magnetic field
 * used in the cascade calculations:
 *
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
   Type = 'R6';
}

 \endverbatim
 */
class R6: public MagneticField
{
public:

  //! constructor
  R6();

  //! Radius of magnetic field line curvature
  virtual double Rcur(double x ) const;

  //! Angle between photon momentum and magnetic field
  virtual double Psi( double x0, double l ) const;

  //! Perpendicular component of the magnetic field \f$ B_\perp/B_q \f$
  virtual double Bperp_x0_l( double x0, double l ) const;
  //! Perpendicular component of the magnetic field \f$ B_\perp/B_q \f$
  virtual double Bperp_x_psi( double x, double psi ) const;
  //! Maximum value of \f$ B_\perp/B_q \f$
  virtual double BperpMax( double x0, Direction d ) const;

  //! Maximum value of \f$ B_\perp/B_q \f$
  virtual double XMax( double x0, Direction d ) const;

  //! Print physical paramneters
  virtual std::ostream& Print(std::ostream& s) const;

protected:

  double _SignB;       //!< Sign of the magnetic field (+1|0|-1)
  
  double _Psi_Coeff;   //!< coeffcient in expression for psi
  double _Rcur_Coeff;  //!< coefficient in formula for the radius of curvature
  
  double  _B0;           //!< coefficient for Bperp_x_psi
  double  _B0_Psi_Coeff; //!< coefficient for Bperp_x0_l

  double  _L;          //!< length of the system
};




#endif
