#ifndef MAGNETIC_FIELD_H
#define MAGNETIC_FIELD_H

#include <string>
#include <iostream>

#include "ATbase.h"

#include "../../utils/direction.h"

/**
 * \defgroup environment_grp Environment
 */

/**
 * \ingroup  environment_grp
 * \defgroup magnetic_field_grp External Magnetic Field
 */

/**
 * \ingroup magnetic_field_grp
 *
 * \class  MagneticField
 * Base class for magnetic fields.
 */
class MagneticField
{
public:

  virtual void SetupFromConfigGroup(FileInput& in) {};

  //! Radius of magnetic field line curvature
  virtual double Rcur(double x ) const = 0;

  //! Angle between photon momentum and magnetic field
  virtual double Psi( double x0, double l ) const = 0;

  //! Perpendicular component of the magnetic field \f$ B_\perp/B_q \f$
  virtual double Bperp_x0_l( double x0, double l ) const = 0;
  //! Perpendicular component of the magnetic field \f$ B_\perp/B_q \f$
  virtual double Bperp_x_psi( double x, double psi ) const = 0;
  //! Maximum value of \f$ B_\perp/B_q \f$
  virtual double BperpMax( double x0, Direction d ) const = 0;

  //! Maximum value of \f$ B_\perp/B_q \f$
  virtual double XMax( double x0, Direction d ) const = 0;


  //! Print physical paramneters
  virtual std::ostream& Print(std::ostream& s) const;

protected:

  std::string  _ClassName;
};


inline std::ostream& operator<< ( std::ostream& s, const MagneticField& mf )
{
  return mf.Print(s);
};

#endif
