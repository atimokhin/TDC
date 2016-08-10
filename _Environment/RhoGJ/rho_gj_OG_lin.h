#ifndef _RHO_GJ_OG_LIN_H_
#define _RHO_GJ_OG_LIN_H_

#include "rho_gj.h"


/**
 * \ingroup rho_gj_grp
 *
 * \class  RhoGJFunctor_OG_lin
 * GJ charge density given by: \f$ Rho0*(x0-x)/x0 \f$
 *
 * configuration file example
 \verbatim

Group "RHO_GJ" {
   Type = 'RhoGJFunctor_OG_lin';
   ! RhoGJFunctor_OG_lin parameters
   Rho0 = 1;
   X0 = 0.5*NParam[GEOMETRY](LENGTH);
}

 \endverbatim
 */

class RhoGJFunctor_OG_lin: public RhoGJFunctor
{
public:
  RhoGJFunctor_OG_lin() {};

  double operator() (double x) const;

  void SetupFromConfigGroup(FileInput& in);

private:
  double _RHO_0;
  double _X0;
};

#endif /* _RHO_GJ_OG_LIN_H_ */
