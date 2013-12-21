#include <string>

#include "dipole.h"

#include "../../SetupDimensionalConstants/norm_consts.h"
#include "../../SetupDimensionalConstants/psr_consts_initializer.h"

#include "../../SetupParameters/geometry_params.h"


/**
 * Constructor. Setup parameters and coefficients
 * 
 */
Dipole::Dipole()
{
  _ClassName="Dipole";

  PSR_ConstsInitializer psr;
  NormConsts            nc;
  GeometryParams        geom;

  _Theta_PC = 1.448e-2 * sqrt( psr.RNS_6()/psr.P() );
  _Alpha    = nc.X0() / 1e6 / psr.RNS_6();

  _Rcur_Coeff = 4./3. * psr.RNS_6() / _Theta_PC / geom.Ksi();

  _Psi_Coeff  = 0.75 * geom.Ksi() * _Theta_PC * _Alpha;

  // magnetic field strength coefficients
  _SignB        = psr.SignB();
  _B0           = psr.B_12();
  _B0_Psi_Coeff = _B0 * _Psi_Coeff;
  _B_MAX_1      = 7.91e-2 *_B0 * geom.Ksi()*_Theta_PC;

  _L     = geom.L();
  _X_MIN = 0.25*( 3*_L - 1e0/_Alpha );
}


/**
 * Magnetic field line radius of curvature
 * 
 * @param x hight above the NS surface
 */
double Dipole:: Rcur(double x ) const
{ 
  return _Rcur_Coeff * sqrt( 1 + x*_Alpha ); 
}


/**
 * Angle between photon momnetum and magnetic field.
 * Photon is emitted at point x0 and has propagated distance l.
 * 
 * @param x0 emission point
 * @param l  distance from the emission point along photon path
 */
double Dipole::Psi( double x0, double l ) const
{   
  return  _Psi_Coeff * fabs(l)*sqrt(1+x0*_Alpha)/( 1+(x0+l)*_Alpha );
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
double Dipole::Bperp_x_psi( double x, double psi ) const
{   
  return  _B0 * psi/pow( 1+x*_Alpha, 3);
}


/**
 * Perpendicular component of the magnetic field as a function of the
 * coordinate of the emission point x0 and distance l travelled by the
 * photon. \f$ B_\perp \f$ is in units of \f$ 10^{12} \f$ Gauss
 * 
 * @param x0 emission point
 * @param l  distance from the emission point along photon path
 */
double Dipole::Bperp_x0_l( double x0, double l ) const
{   
  return  _B0_Psi_Coeff * fabs(l)*sqrt(1+x0*_Alpha)/pow( 1+(x0+l)*_Alpha, 4);
}


/**
 * Maximum  value of perpendicular component of the magnetic field
 * 
 * @param x0 emission point
 * @param d  direction of photon propagation
 */
inline double Dipole::BperpMax( double x0, Direction d ) const
{   
  // downward propagating photon - Bmax at NS surface
  if ( d == DOWN )
    {
      return _B0_Psi_Coeff * x0*sqrt(1+x0*_Alpha);
    }
  // upward propagating photon - Bmax at upper boundary
  else if ( x0 > _X_MIN )
    {
      return Bperp_x0_l( x0, _L-x0 );
    }
  // upward propagating photon - Bmax inside the gap
  else
    {
      return _B_MAX_1/pow( 1+x0*_Alpha, 2.5);
    }
}


/**
 * Coordinate of point where of perpendicular component of the
 * magnetic field reaches its maximum value
 * 
 * @param x0 emission point
 * @param d  direction of photon propagation
 */
inline double Dipole::XMax( double x0, Direction d ) const
{   

  if ( d == DOWN )
    // downward propagating photon - Bmax at NS surface
    return 0;
  else if ( x0 > _X_MIN )
    // upward propagating photon - Bmax at upper boundary
    return _L;
  else
    // upward propagating photon - Bmax inside the gap
    return 0.333333333333333 * ( 4*x0 + 1e0/_Alpha );
}


std::ostream& Dipole::Print(std::ostream& s) const
{
  string separator=string(40,'-')+"\n";
  s<<"\n";
  s<<separator;
  s<<"Magnetic Field : "<<_ClassName<<" <<<<<<<<<<<<< \n";
  s<<separator;
  s<<"   SignB="<<_SignB<<"\n";
  s<<"      B0="<<_B0<<" 10^12 G\n";
  s<<"R_cur(0)="<<Rcur(0)*1e6<<" [cm]\n";
  s<<"R_cur(1)="<<Rcur(1)*1e6<<" [cm]\n";
  s<<separator<<"\n";
  s<<std::flush;
  return s;
}
