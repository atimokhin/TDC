#include "r6_cutoff.h"

#include "../../SetupDimensionalConstants/norm_consts.h"
#include "../../SetupDimensionalConstants/magnetic_field_consts.h"

#include "../../SetupParameters/geometry_params.h"


/**
 * Constructor. Setup parameters and coefficients
 * 
 */
R6_CutOff::R6_CutOff():
  R6()
{
  _ClassName="R6_CutOff";
}

void R6_CutOff::SetupFromConfigGroup(FileInput& in) 
{
  _X_cutoff = in.get_param("X_cutoff");
};


/**
 * Perpendicular component of the magnetic field as a function of the
 * coordinate of the current position of the photons and the angle
 * between photon momentum and the magnetic field. \f$ B_\perp \f$ is
 * in units of \f$ 10^{12} \f$ Gauss
 * 
 * @param x   current point
 * @param psi angle to the magntic field line
 */
double R6_CutOff::Bperp_x_psi( double x, double psi ) const
{   
  return  ( x < _X_cutoff ? _B0 * psi : 0 );
}


/**
 * Perpendicular component of the magnetic field as a function of the
 * coordinate of the emission point x0 and distance l travelled by the
 * photon. \f$ B_\perp \f$ is in units of \f$ 10^{12} \f$ Gauss
 * 
 * @param x0 emission point
 * @param l  distance from the emission point along photon path
 */
double R6_CutOff::Bperp_x0_l( double x0, double l ) const
{   
  return  ( x0+l < _X_cutoff ? _B0_Psi_Coeff * fabs(l) : 0);
}


/**
 * Maximum  value of perpendicular component of the magnetic field
 * 
 * @param x0 emission point
 * @param d  direction of photon propagation
 */
inline double R6_CutOff::BperpMax( double x0, Direction d ) const
{   
  // downward propagating photon - Bmax at NS surface
  if ( d == DOWN )
    {
      return _B0_Psi_Coeff * x0;
    }
  // upward propagating photon - Bmax at upper boundary
  else 
    {
      return ( x0 < _X_cutoff ? _B0_Psi_Coeff*(_X_cutoff-x0) : 0 );
    }
}


/**
 * Coordinate of point where of perpendicular component of the
 * magnetic field reaches its maximum value
 * 
 * @param x0 emission point
 * @param d  direction of photon propagation
 */
inline double R6_CutOff::XMax( double x0, Direction d ) const
{   

  if ( d == DOWN )
    // downward propagating photon - Bmax at NS surface
    return 0;
  else 
    // upward propagating photon - Bmax at upper boundary
    return _X_cutoff;
}

std::ostream& R6_CutOff::Print(std::ostream& s) const
{
  MagneticField::Print(s);
  s<<"  X_cutoff : "<<_X_cutoff<<"\n\n";
  return s;
}

