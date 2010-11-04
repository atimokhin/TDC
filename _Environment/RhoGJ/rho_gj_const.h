#ifndef RHO_GJ_CONST_H
#define RHO_GJ_CONST_H

#include "rho_gj.h"

/**
 * \ingroup rho_gj_grp
 *
 * \class  RhoGJFunctor_Const
 * GJ charge density given by: \f$ -sign(B) \f$
 *
 * configuration file example
 \verbatim

Group "RHO_GJ" {
   Type = 'RhoGJFunctor_Const';
}

 \endverbatim
 */

class RhoGJFunctor_Const: public RhoGJFunctor
{
public:
  RhoGJFunctor_Const();

  double operator() (double x) const { return -_SignB; };

  void SetupFromConfigGroup(FileInput& in) {};

private:
  double _SignB;
};

#endif
