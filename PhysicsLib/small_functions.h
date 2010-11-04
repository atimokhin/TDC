#ifndef SMALL_FUNCTIONS_H
#define SMALL_FUNCTIONS_H

#include <cmath>

/**
 * \file small_functions.h contains collections of small fuctions mainly for
 * emergy-momentum-velocity transformations
 * 
 */


// small functions
namespace SF
{
  namespace SFConst 
  {
    const double P_MIN_2BETA = 40;

    const double P_MIN_2E = 5;
    const double P_MAX_2E = 25;

    const double E_MIN_2P = 5;
    const double E_MAX_2P = 25;
  };


  /** 
   * converts momentum into energy
   * 
   * @param p \f$ p =\beta\gamma \f$ 
   * 
   * @return energy \f$ \gamma \f$
   */
  inline double P2E(double p)
  {
    if ( p > SFConst::P_MAX_2E ) 
      return p;
    else if ( p > SFConst::P_MIN_2E )
      return p + 0.5/p;
    else
      return sqrt(1+p*p);
  }


  /** 
   * converts energy into momentum
   *
   * @param e energy \f$ \gamma \f$
   * 
   * @return p \f$ p =\beta\gamma \f$ 
   * 
   */
  inline double E2P(double e)
  {
    if ( e > SFConst::E_MAX_2P ) 
      return e;
    else if ( e > SFConst::E_MIN_2P )
      return e - 0.5/e;
    else
      return sqrt(e*e-1);
  }


  /** 
   * converts momentum into velocity
   * 
   * @param p \f$ p =\beta\gamma \f$ 
   * 
   * @return velocity \f$ \beta \f$
   */
  inline double P2Beta(double p)
  {
    if ( p > SFConst::P_MIN_2BETA )
      return 1 - 0.5/(p*p);
    else if ( p < - SFConst::P_MIN_2BETA )
      return - 1 + 0.5/(p*p);
    else
      return p/sqrt(1+p*p);
  }

 
  /**  
   * Sign of x : -1 if x<0, otherwise 1
   *
   * @param  x 
   * 
   * @return sign(x)
   */
  inline double Sign( double x )
  {
    return ( (x<0) ? -1 : 1 );
  }

}
#endif
