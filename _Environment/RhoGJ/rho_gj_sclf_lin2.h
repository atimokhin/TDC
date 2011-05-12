#ifndef RHO_GJ_SCLF_LIN2_H
#define RHO_GJ_SCLF_LIN2_H

#include "rho_gj.h"


/**
 * \ingroup rho_gj_grp
 *
 * \class  RhoGJFunctor_SCLF_lin2
 * GJ charge density given by: \f$ - sign(B)*( A_1 - A_2 min(1,x/x_0) ) \f$
 *
 * configuration file example
 \verbatim

Group "RHO_GJ" {
   Type = 'RhoGJFunctor_SCLF_lin2';
   ! RhoGJFunctor_SCLF_lin2 parameters
   A1 = 1;
   A2 = -.5;
   X0 = 0.3*NParam[GEOMETRY](LENGTH);
}

 \endverbatim
 */

class RhoGJFunctor_SCLF_lin2: public RhoGJFunctor
{
public:
  RhoGJFunctor_SCLF_lin2();

  double operator() (double x) const;

  void SetupFromConfigGroup(FileInput& in);

private:
  double _SignB;
  double _A1;
  double _A2;
  double _X0;
};

#endif
