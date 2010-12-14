#ifndef R6_CUTOFF_H
#define R6_CUTOFF_H

#include <cmath>
#include <iostream>

#include "r6.h"


/**
 * \ingroup magnetic_field_grp
 *
 * \class  R6_CutOff
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
   Type = 'R6_CutOff';
   X_cutoff = 0.5*NParam[GEOMETRY](LENGTH);
}

 \endverbatim
 */
class R6_CutOff: public R6
{
public:

  //! constructor
  R6_CutOff();

  virtual void SetupFromConfigGroup(FileInput& in);

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
  
  double _X_cutoff;
};

#endif
