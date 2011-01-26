#ifndef NORM_CONSTS_H
#define NORM_CONSTS_H

#include "ATbase.h"

/**
 * \defgroup setup_consts_grp Setup Constants
 * Contains all normalization constants.
 *
 * The constants are set up using classes from setup_params_grp
 */

/**
 * \ingroup setup_consts_grp
 *
 * \class  NormConsts
 * \brief  contains normalization constants, i.e. dimensional constants 
 *
 * contains normalization constants:
 * - \f$ x_0 \f$     -- normalization length
 * - \f$ t_0 \f$     -- normalization time
 * - \f$ \rho_0 \f$  -- normalization charge density
 *
 * - \f$ P_{\mbox{cf}} \f$ 
 * - \f$ E_0 \f$     -- normalization electric field strength [cgs units]   
 * - \f$ \Phi_0 \f$  -- normalization electric potential [cgs units]   
 */
class NormConsts
{
public:
  //! normalization length   \f$ x_0 \f$
  double X0()   const { return _X0; }; 
  //! normalization time   \f$ t_0 \f$
  double T0()   const { return _T0; };

  //! normalization charge density  \f$ \rho_0 \f$ 
  double Rho0() const { return _Rho0; }; 

  //! \f$ P_{mbox{cf}} \f$ 
  double Pcf()  const { return _Pcf; }; 

  //! normalization electric field strength [cgs units]  \f$ E_0 \f$
  double E0()   const { return _E0; }; 
  //! \normalization electric potential [cgs units]   f$ \Phi_0 \f$
  double Phi0() const { return _Phi0; }; 

protected:
  static double _X0;
  static double _T0;

  static double _Rho0;

  static double _Pcf;

  static double _E0;
  static double _Phi0;
};

#endif
