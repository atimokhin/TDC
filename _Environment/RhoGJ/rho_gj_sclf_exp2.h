#ifndef RHO_GJ_SCLF_EXP2_H
#define RHO_GJ_SCLF_EXP2_H

#include "rho_gj.h"


/**
 * \ingroup rho_gj_grp
 *
 * \class  RhoGJFunctor_SCLF_exp2
 * GJ charge density given by: \f$ - sign(B)*( A_1 - A_2 * (1e0 - exp(x/_X0) ) \f$
 *
 * configuration file example
 \verbatim

Group "RHO_GJ" {
   Type = 'RhoGJFunctor_SCLF_exp2';
   ! RhoGJFunctor_SCLF_exp2 parameters
   A1 = 1;
   A2 = -.5;
   X0 = 0.3*NParam[GEOMETRY](LENGTH);
}

 \endverbatim
 */

class RhoGJFunctor_SCLF_exp2: public RhoGJFunctor
{
public:
  RhoGJFunctor_SCLF_exp2();

  double operator() (double x) const;

  void SetupFromConfigGroup(FileInput& in);

private:
  double _SignB;
  double _A1;
  double _A2;
  double _X0;
};

#endif
