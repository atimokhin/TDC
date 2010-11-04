#include "r6.h"

#include "../../SetupDimensionalConstants/norm_consts.h"
#include "../../SetupDimensionalConstants/magnetic_field_consts.h"

#include "../../SetupParameters/geometry_params.h"


/**
 * Constructor. Setup parameters and coefficients
 * 
 */
R6::R6()
{
  _ClassName="R6";

  MagneticFieldConsts   mf;
  NormConsts            nc;
  GeometryParams        geom;

  _Rcur_Coeff = mf.Rcur_6();

  _Psi_Coeff  = nc.X0()/( _Rcur_Coeff * 1e6 );

  // magnetic field strength coefficients
  _SignB        = mf.SignB();
  _B0           = mf.B_12();
  _B0_Psi_Coeff = _B0 * _Psi_Coeff;

  _L = geom.L();
}


/**
 * Magnetic field line radius of curvature
 * 
 * @param x hight above the NS surface
 */
double R6::Rcur(double x ) const
{ 
  return _Rcur_Coeff; 
}


/**
 * Angle between photon momnetum and magnetic field.
 * Photon is emitted at point x0 and has propagated distance l.
 * 
 * @param x0 emission point
 * @param l  distance from the emission point along photon path
 */
double R6::Psi( double x0, double l ) const
{   
  return  _Psi_Coeff * fabs(l);
}


/**
 * Perpendicular component of the magnetic field as a function of the
 * coordinate of the current position of the photons and the angle
 * between photon momentum and the magnetic field. \f$ B_\perp \f$ is
 * in units of \f$ 10^{12} \f$ Gauss
 * 
 * @param x   current point
 * @param psi angle to the magntic field line
 */
double R6::Bperp_x_psi( double x, double psi ) const
{   
  return  _B0 * psi;
}


/**
 * Perpendicular component of the magnetic field as a function of the
 * coordinate of the emission point x0 and distance l travelled by the
 * photon. \f$ B_\perp \f$ is in units of \f$ 10^{12} \f$ Gauss
 * 
 * @param x0 emission point
 * @param l  distance from the emission point along photon path
 */
double R6::Bperp_x0_l( double x0, double l ) const
{   
  return  _B0_Psi_Coeff * fabs(l);
}


/**
 * Maximum  value of perpendicular component of the magnetic field
 * 
 * @param x0 emission point
 * @param d  direction of photon propagation
 */
inline double R6::BperpMax( double x0, Direction d ) const
{   
  // downward propagating photon - Bmax at NS surface
  if ( d == DOWN )
    {
      return _B0_Psi_Coeff * x0;
    }
  // upward propagating photon - Bmax at upper boundary
  else 
    {
      return Bperp_x0_l( x0, _L-x0 );
    }
}


/**
 * Coordinate of point where of perpendicular component of the
 * magnetic field reaches its maximum value
 * 
 * @param x0 emission point
 * @param d  direction of photon propagation
 */
inline double R6::XMax( double x0, Direction d ) const
{   

  if ( d == DOWN )
    // downward propagating photon - Bmax at NS surface
    return 0;
  else 
    // upward propagating photon - Bmax at upper boundary
    return _L;
}
