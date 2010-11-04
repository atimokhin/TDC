#ifndef RHO_GJ_H
#define RHO_GJ_H

#include "ATbase.h"

/**
 * \ingroup  environment_grp
 * \defgroup rho_gj_grp  Goldrech-Julian charge density functors
 * 
 */

/**
 * \ingroup rho_gj_grp
 *
 * \class  RhoGJFunctor
 * \brief  Base class for Goldrech-Julian charge density functors
 */
class RhoGJFunctor
{
public:
  virtual double operator() (double x) const = 0;

  virtual void SetupFromConfigGroup(FileInput& in) = 0;
};

#endif
